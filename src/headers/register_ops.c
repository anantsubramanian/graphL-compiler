#include "register.h"

REGISTER registers [ NUMREG ];
int roundrobinreg = 0;

char registerNames[][4] = {

  "eax",
  "ebx",
  "ecx",
  "edx",
  "esi",
  "edi"
};

/**
 * Function that gets the name of a register given
 * its int ID
 *
 * @param regid int The int ID of the reg
 *
 * @return char* The printable name of the register
 */

char* getRegisterName ( int regid )
{
  return registerNames [ regid ];
}

/**
 * Function that sets the properties for the given register
 * The parameters names are self explanatory
 */

void setRegisterProperties (
    int regid, int flushed, int isglobal, int istemp, int hasoffset,
    int stbindex, int offset1, int offset2, int offset3 )
{
  registers [ regid ] . flushed = flushed;
  registers [ regid ] . isglobal = isglobal;
  registers [ regid ] . istemp = istemp;
  registers [ regid ] . hasoffset = hasoffset;
  registers [ regid ] . stbindex = stbindex;
  registers [ regid ] . offset1 = offset1;
  registers [ regid ] . offset2 = offset2;
  registers [ regid ] . offset3 = offset3;
}

/**
 * Function that flushes the given register if necessary
 * to the file specified by the parameters
 *
 * @param topick int The register to flush
 * @param codefile FILE* The file to dump asm instructions to
 * @param symboltable SYMBOLTABLE* The symboltable to use while dumping
 */

void flushRegister ( int topick, FILE *codefile, SYMBOLTABLE *symboltable )
{
  // Registers with offsets don't have write data, so they don't need to be flushed
  if ( registers [ topick ] . hasoffset )
    return;

  if ( registers [ topick ] . isglobal == IS_GLOBAL )
  {
    char *varname = getEntryByIndex ( symboltable, registers [ topick ] . stbindex ) -> data . var_data . name;
    char midc = ((topick == EAX_REG) ? 'b' : 'a');

    if ( registers [ topick ] . offset2 == OFFSET_ANY )
      fprintf ( codefile, "\tmov\t[%s], %s\n", varname, getRegisterName ( topick ) );
    else if ( registers [ topick ] . offset3 == OFFSET_ANY )
      fprintf ( codefile, "\tmov\t[%s+%d], %s\n", varname, registers [ topick ] . offset2, getRegisterName ( topick ) );
    else
    {
      fprintf ( codefile, "\tpush\te%cx\n", midc );
      fprintf ( codefile, "\tmov\te%cx, [%s+%d]\n", midc, varname, registers [ topick ] . offset2 );
      fprintf ( codefile, "\tadd\te%cx, %d\n", midc, registers [ topick] . offset3 );
      fprintf ( codefile, "\tmov\t[e%cx], %s\n", midc, getRegisterName ( topick ) );
      fprintf ( codefile, "\tpop\te%cx\n", midc );
    }
  }
  else
  {
    char midc = ((topick == EAX_REG) ? 'b' : 'a');

    if ( registers [ topick ] . offset2 == OFFSET_ANY )
      fprintf ( codefile, "\tmov\t[ebp-%d], %s\n", registers [ topick ] . offset1, getRegisterName ( topick ) );
    else if ( registers [ topick ] . offset3 == OFFSET_ANY )
    {
      int resultant = registers [ topick ] . offset1 + registers [ topick ] . offset2;
      fprintf ( codefile, "\tmov\t[ebp-%d], %s\n", resultant, getRegisterName ( topick ) );
    }
    else
    {
      fprintf ( codefile, "\tpush\te%cx\n", midc );
      int resultant = registers [ topick ] . offset1 + registers [ topick ] . offset2;
      fprintf ( codefile, "\tmov\te%cx, [ebp-%d]\n", midc, resultant );
      fprintf ( codefile, "\tsub\te%cx, %d\n", midc, registers [ topick ] . offset3 );
      fprintf ( codefile, "\tmov\t[e%cx], %s\n", midc, getRegisterName ( topick ) );
      fprintf ( codefile, "\tpop\te%cx\n", midc );
    }
  }

  registers [ topick ] . flushed = 1;
}

/**
 * Function that gets a register given the necessary parameters
 * The parameters names are self explanatory.
 * 
 * It tries to pick unused registers in a round robin manner
 *
 * @return int The ID of the allocated register
 */

int getRegister (
    FILE *codefile, SYMBOLTABLE *symboltable, int symboltable_index, int offset1,
    int offset2, int offset3, int topick, int istemp, int donttouch1, int donttouch2 )
{
  int i;
  if ( ! istemp )
  {
    for ( i = 0; i < NUMREG; i++ )
      if ( registers [i] . stbindex == symboltable_index
           && ( registers [i] . offset1 == offset1 || offset1 == OFFSET_ANY )
           && ( registers [i] . offset2 == offset2 || offset2 == OFFSET_ANY )
           && ( registers [i] . offset3 == offset3 || offset3 == OFFSET_ANY )
           && registers [i] . flushed == 0
           && registers [i] . istemp != 1 )
      {
        if ( DEBUG_REGISTER_ALLOC )
          fprintf ( stderr, "Found register %d for %d\n", i, symboltable_index );
        return i;
      }

    if ( DEBUG_REGISTER_ALLOC )
      fprintf ( stderr, "Data of %d doesn't exists in any register\n", symboltable_index );
  }
  else if ( istemp == IS_LITERAL )
  {
    for ( i = 0; i < NUMREG; i++ )
      if ( registers [i] . stbindex == symboltable_index
           && registers [i] . flushed == 0
           && registers [i] . istemp == IS_LITERAL )
      {
        if ( DEBUG_REGISTER_ALLOC )
          fprintf ( stderr, "Found register %d for %d\n", i, symboltable_index );
        return i;
      }

    if ( DEBUG_REGISTER_ALLOC )
      fprintf ( stderr, "Data of %d doesn't exists in any register\n", symboltable_index );
  }

  // All registers need to be flushed, so pick one and flush
  if ( topick == NO_SPECIFIC_REG )
  {
    for ( i = 0; i < NUMREG; i++ )
      if ( registers [i] . flushed )
        return i;

    int roundrobinstart = roundrobinreg;    // To detect infinite loops
    topick = (roundrobinreg + 1) % NUMREG;
    while ( registers [ topick ] . istemp || topick == donttouch1 || topick == donttouch2 )
    {
      topick = (topick + 1) % NUMREG;
      if ( topick == roundrobinstart )
        return -1;
    }
    roundrobinreg = topick;
  }

  if ( registers [ topick ] . istemp != IS_LITERAL && ! registers [ topick ] . flushed )
    flushRegister ( topick, codefile, symboltable );

  registers [topick] . flushed = 1;
  registers [topick] . hasoffset = 1;

  return topick;
}

/**
 * Function that reads an AST ASSIGNABLE node and gets the necessary offset
 * in a register
 *
 * @param assignable ANODE* The AST node of the assignable to obtain
 * @param codefile FILE* The code file to dump asm instructions to
 * @param symboltable SYMBOLTABLE* The symbol table to use for aux data
 *
 * @return int The register ID of the register containing the obtained offset
 */

int getOffsetInReg ( ANODE *assignable, FILE *codefile, SYMBOLTABLE *symboltable )
{
  int o1 = assignable -> offset1;
  int o2 = (assignable -> offsetcount > 1) ? assignable -> offset2 : OFFSET_ANY;
  int o3 = (assignable -> offsetcount > 2) ? assignable -> offset3 : OFFSET_ANY;

  int target = getRegister (
      codefile, symboltable, getFirstChild ( assignable ) -> extra_data . symboltable_index,
      o1, o2, o3, NO_SPECIFIC_REG, 0, NO_REGISTER, NO_REGISTER );

  if ( target == -1 )
  {
    fprintf ( stderr, "Error: Out of registers, cannot get offset in temp register\n" );
    exit ( -1 );
  }

  if ( registers [ target ] . flushed == 0 )
    return target;

  if ( assignable -> global_or_local == IS_LOCAL )
  {
    if ( assignable -> offsetcount == ONE_OFFSET )
    {
      fprintf ( codefile, "\tmov\t%s, ebp\n", getRegisterName ( target ) );
      fprintf ( codefile, "\tsub\t%s, %d\n", getRegisterName ( target ), assignable -> offset1 );
    }
    else
    {
      int resultant = assignable -> offset2 + assignable -> offset1;
      fprintf ( codefile, "\tmov\t%s, ebp\n", getRegisterName ( target ) );
      fprintf ( codefile, "\tsub\t%s, %d\n", getRegisterName ( target ), resultant );
    }

    if ( assignable -> offsetcount == THREE_OFFSETS )
    {
      fprintf ( codefile, "\tmov\t%s, [%s]\n", getRegisterName ( target ), getRegisterName ( target ) );
      fprintf ( codefile, "\tsub\t%s, %d\n", getRegisterName ( target ), assignable -> offset3 );
    }
  }
  else
  {
    // Is a global variable
    char *varname = getEntryByIndex ( symboltable, assignable -> offset1 ) -> data . var_data . name;
    fprintf ( codefile, "\t; Getting offset for %s\n", varname );

    if ( assignable -> offsetcount == ONE_OFFSET )
      fprintf ( codefile, "\tmov\t%s, %s\n", getRegisterName ( target ), varname );
    else
    {
      fprintf ( codefile, "\tmov\t%s, %s\n", getRegisterName ( target ), varname );
      fprintf ( codefile, "\tadd\t%s, %d\n", getRegisterName ( target ), assignable -> offset2 );
    }

    if ( assignable -> offsetcount == THREE_OFFSETS )
    {
      fprintf ( codefile, "\tmov\t%s, [%s]\n", getRegisterName ( target ), getRegisterName ( target ) );
      fprintf ( codefile, "\tadd\t%s, %d\n", getRegisterName ( target ), assignable -> offset3 );
    }
  }

  setRegisterProperties ( target, 0, assignable -> global_or_local, 0, 1,
    getFirstChild ( assignable ) -> extra_data . symboltable_index, assignable -> offset1, 0,
    assignable -> offset3 );

  if ( assignable -> offsetcount > 1 )
    registers [ target ] . offset2 = assignable -> offset2;
  else
    registers [ target ] . offset2 = OFFSET_ANY;

  if ( assignable -> offsetcount > 2 )
    registers [ target ] . offset3 = assignable -> offset3;
  else
    registers [ target ] . offset3 = OFFSET_ANY;

  return target;
}

/**
 * Function that gets a literal's offset in a register
 * Parameters are self explanatory.
 *
 * @return int The register ID of the register containing the literal offset
 */

int getLiteralInRegister (
    ANODE *literalnode, FILE *codefile, SYMBOLTABLE *symboltable,
    TRIE *literaltrie, LITDATA *literals )
{
  LITERAL *litdata =
    & ( getEntryByIndex ( symboltable,
                          literalnode -> extra_data . symboltable_index
                        ) -> data . lit_data );

  TNODE *foundlit = findString ( literaltrie, litdata -> value );

  int targetreg = getRegister ( codefile, symboltable, literalnode -> extra_data . symboltable_index,
                  OFFSET_ANY, OFFSET_ANY, OFFSET_ANY, NO_SPECIFIC_REG, IS_LITERAL, NO_REGISTER,
                  NO_REGISTER );

  if ( targetreg == -1 )
  {
    fprintf ( stderr, "Error: Out of temporary registers to get literal\n" );
    exit ( -1 );
  }

  fprintf ( codefile, "\tmov\t%s, [%s]\n",
            getRegisterName ( targetreg ),
            literals [ foundlit -> data . int_val ] . name );

  setRegisterProperties ( targetreg, 0, -1, IS_LITERAL, 0, literalnode -> extra_data . symboltable_index,
                          OFFSET_ANY, OFFSET_ANY, OFFSET_ANY );

  return targetreg;
}

