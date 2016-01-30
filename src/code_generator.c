// Authors: Anant Subramanian <anant.subramanian15@gmail.com>
//          Aditya Bansal <adityabansal_adi@yahoo.co.in>
//
// BITS PILANI ID NOs: 2012A7TS010P
//                     2012A7PS122P
//
// Project Team Num: 1
// Project Group No. 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include "headers/ast.h"
#include "headers/symboltable.h"
#include "headers/constants.h"

#ifndef TRIE_DEFINED
  #include "headers/trie.h"
#endif

#define DEBUG_AST_CONSTRUCTION 0
#define DEBUG_STB_AUXOPS 0
#define DEBUG_REGISTER_ALLOC 0

#define NUMREG 6
#define BUFFERLEN 400
#define NEWLINE '\n'
#define STB_DUMP_FILE "STBDUMP"
#define AST_DUMP_FILE "ASTDUMP"
#define ASSEMBLY_FILE "VARFILE"
#define ASSEMBLY_CODE_FILE "CODEFILE"
#define ASSEMBLY_FUNCTIONS_FILE "FUNCTIONFILE"
#define ASSEMBLY_DATA_FILE "ASMFILE.asm"
#define T_INDEX_FILE "config/terminals_index"
#define NT_INDEX_FILE "config/nonterminals_index"
#define ROOTNODENAME "AST_ROOT_NODE"
#define LOWER16MASK 0b00000000000000001111111111111111
#define UPPER16MASK 0b11111111111111110000000000000000

// End AST node types #defines

#define DOWN 0
#define UP 1

#define ONE_OFFSET 1
#define TWO_OFFSETS 2
#define THREE_OFFSETS 3
#define DATA_IN_REG 4
#define OFFSET_IN_REG 5
#define IS_GLOBAL 0
#define IS_LOCAL 1
#define IS_LITERAL 2
#define NO_SPECIFIC_REG -1
#define OFFSET_ANY -1
#define NO_REGISTER -2
#define EAX_REG 0
#define EBX_REG 1
#define ECX_REG 2
#define EDX_REG 3

// The structure that is pushed on the stack to check whether this node is being
// poppsed on the way down or the way up, i.e. top-down traversal or bottom-up traversal
typedef struct stack_entry
{
  ANODE *node;
  int upordown;
} STACKENTRY;

typedef struct register_data
{
  int flushed;
  int hasoffset;
  int stbindex;
  int offset1;
  int offset2;
  int offset3;
  int istemp;
  int isglobal;
} REGISTER;

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

char* getRegisterName ( int regid )
{
  return registerNames [ regid ];
}

void setRegisterProperties ( int regid, int flushed, int isglobal, int istemp, int hasoffset,
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

int getRegister ( FILE *codefile, SYMBOLTABLE *symboltable, int symboltable_index, int offset1,
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

int forlabel = 0;
int iflabel = 0;
int curroffset = 0;
int erroroccured = 0;
int hasglobalvars = 0;
int hasfunctions = 0;
int startwritten = 0;
int curlitindex = 0;

typedef struct literal_data
{
  char *name;
} LITDATA;


char* getAropName ( AROPTYPE type )
{
  if ( type == A_PLUS_TYPE )
    return aropTypes [1];
  if ( type == A_MINUS_TYPE )
    return aropTypes [2];
  if ( type == A_MUL_TYPE )
    return aropTypes [3];
  if ( type == A_DIV_TYPE )
    return aropTypes [4];
  if ( type == A_MODULO_TYPE )
    return aropTypes [5];

  return aropTypes [0];
}

char* getNodeTypeName ( int type )
{
  return nodeTypes [type];
}

char* getDataTypeName ( DATATYPE type )
{
  if ( type == D_INT_TYPE )
    return dataTypes [1];
  if ( type == D_FLOAT_TYPE )
    return dataTypes [2];
  if ( type == D_STRING_TYPE )
    return dataTypes [3];
  if ( type == D_VERTEX_TYPE )
    return dataTypes [4];
  if ( type == D_EDGE_TYPE )
    return dataTypes [5];
  if ( type == D_TREE_TYPE )
    return dataTypes [6];
  if ( type == D_GRAPH_TYPE )
    return dataTypes [7];
  if ( type == D_NOTHING_TYPE )
    return dataTypes [8];

  return dataTypes [0];
}

void populateTrie ( FILE *mapfile, int blocksize, TRIE* trie, int *count )
{
  char buffers [2] [ blocksize ];
  int curbuff = -1;
  int charindx = -1;
  int charsread = 0;
  int tokenindex = 0;
  int torval = 0;

  char c;
  char token [ BUFFERLEN ];
  int value = 0;

  while ( TRUE )
  {
    // Get char from appropriate buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( (charsread = fread ( buffers [ curbuff ], sizeof ( char ),
                                blocksize, mapfile ) ) == 0 )
        break;
    }
    c = buffers [ curbuff ] [ charindx ];

    if ( charsread < blocksize && charindx >= charsread )
      break;

    if ( c == ' ' )
    {
      torval = 1;
      tokenindex = 0;
    }
    else if ( c == NEWLINE )
    {
      token [ tokenindex ] = '\0';
      TNODE *temp = NULL;
      temp = insertString ( trie, token );
      temp -> data.int_val = value;
      *count = value;
      value = 0;
      torval = 0;
    }
    else if ( torval == 1 )
      token [ tokenindex++ ] = c;
    else
      value = value * 10 + c - 48;
  }
}

void readAstDumpFile ( ANODE *node, FILE *astdumpfile )
{
  if ( DEBUG_AST_CONSTRUCTION )
    printf ( "At node: %s\n", getNodeTypeName ( node -> node_type ) );

  ANODE *createdNode = readDumpNode ( node, astdumpfile );

  createdNode -> parent = node;

  if ( DEBUG_AST_CONSTRUCTION )
    printf ( "Created node: %s with %d children\n\n",
             getNodeTypeName ( createdNode -> node_type ), createdNode -> num_of_children );

  int i;
  int childcount = createdNode -> num_of_children;

  // addChild will modify this too, so we must reset it after getting the info for the loop below
  createdNode -> num_of_children = 0;

  for ( i = 0; i < childcount; i++ )
  {
    if ( DEBUG_AST_CONSTRUCTION )
      printf ( "Child %d / %d for %s:\n\n", i, childcount,
               getNodeTypeName ( createdNode -> node_type ) );

    readAstDumpFile ( createdNode, astdumpfile );
  }

  if ( DEBUG_AST_CONSTRUCTION )
    printf ( "Done with: %s\n\n", getNodeTypeName ( createdNode -> node_type ) );
}


void handleTypeSpecificActions ( ANODE *currnode, SYMBOLTABLE *symboltable, FILE *stbdumpfile )
{
  ANODE *parent = currnode -> parent;

  if ( currnode -> node_type == AST_IDENTIFIER_NODE
       || currnode -> node_type == AST_LITERAL_NODE )
  {
    if ( parent -> node_type == AST_FUNCTION_NODE
         || parent -> node_type == AST_GLOBALDEFINE_NODE
         || parent -> node_type == AST_DEFINE_NODE
         || parent -> node_type == AST_QUALIFIEDPARAMETER_NODE )
    {
      // Entry to be read from dump file is a define entry
      unsigned int entryindex = readDumpEntry ( symboltable, stbdumpfile, 'd' );

      if ( DEBUG_STB_AUXOPS )
      {
        char *name = NULL;
        if ( parent -> node_type == AST_FUNCTION_NODE )
          name = getEntryByIndex ( symboltable, entryindex ) -> data . func_data . name;
        else
          name = getEntryByIndex ( symboltable, entryindex ) -> data . var_data . name;

        printf ( "Definition of %s registered\n", name);
      }

      VARIABLE *vardata = & ( getEntryByIndex ( symboltable, entryindex ) -> data . var_data );

      if ( parent -> node_type == AST_GLOBALDEFINE_NODE )
        vardata -> var_type = V_GLOBAL_TYPE;
      else if ( parent -> node_type == AST_QUALIFIEDPARAMETER_NODE )
        vardata -> var_type = V_PARAM_TYPE;
      else if ( parent -> node_type == AST_DEFINE_NODE )
        vardata -> var_type = V_LOCAL_TYPE;

      currnode -> extra_data . symboltable_index = entryindex;
    }
    else
    {
      // Entry to be read is a variable reference entry (or a literal entry)
      // Perform the same action for both
      unsigned int entryindex = readDumpEntry ( symboltable, stbdumpfile, 'r' );

      if ( DEBUG_STB_AUXOPS )
      {
        char *name = NULL;
        if ( currnode -> node_type == AST_LITERAL_NODE )
        {
          printf ( "sdf %s\n", getEntryByIndex ( symboltable, entryindex ) -> data . lit_data . value );
          name = getEntryByIndex ( symboltable, entryindex ) -> data . lit_data . value;
        }
        else if ( getEntryByIndex ( symboltable, entryindex ) -> entry_type == ENTRY_FUNC_TYPE )
          name = getEntryByIndex ( symboltable, entryindex ) -> data . func_data . name;
        else
          name = getEntryByIndex ( symboltable, entryindex ) -> data . var_data . name;

        printf ( "Reference of %s registered\n", name );
      }

      currnode -> extra_data . symboltable_index = entryindex;
    }
  }
  else if ( currnode -> node_type == AST_QUALIFIEDPARAMETERS_NODE )
  {
    // Parent WILL be a function node, so set its number and type of parameters
    ANODE *funciden = getFirstChild ( parent );

    unsigned int funcindex = funciden -> extra_data . symboltable_index;

    STBENTRY *funcentry = getEntryByIndex ( symboltable, funcindex );

    if ( funcentry == NULL )
    {
      fprintf ( stderr, "Function entry not found in symbol table for some reason...\n" );
      erroroccured = 1;
      return;
    }

    FUNCTION *funcdata = & ( funcentry -> data . func_data );

    // The number of parameters of the function = num of children of qualified parameters node
    funcdata -> num_params = currnode -> num_of_children;

    // Iterate over children of qualified parameters node, each one is a qualified parameter
    // whose first child is a datatype node from which we can extract the data types of the
    // parameters to this function
    LNODE iterator;

    getIterator ( currnode -> children, & iterator );

    while ( hasNext ( & iterator ) )
    {
      getNext ( currnode -> children, & iterator );

      ANODE *qualifiedparamnode = * ( ANODE ** ) ( iterator . data . generic_val );
      DATATYPE dtype = getFirstChild ( qualifiedparamnode ) -> extra_data . data_type;

      funcdata -> paramtypes = insertAtBack ( funcdata -> paramtypes, & dtype );

      if ( DEBUG_STB_AUXOPS )
        printf ( "Set the data type of %s's param as %s\n", funcdata -> name, getDataTypeName ( dtype ) );
    }
  }
  else if ( currnode -> node_type == AST_RETURNTYPE_NODE )
  {
    // A returntype node could have no children, in which case the return type is nothing

    ANODE *returniden = getFirstChild ( parent );
    unsigned int retindex = returniden -> extra_data . symboltable_index;
    STBENTRY *retentry = getEntryByIndex ( symboltable, retindex );

    if ( retentry == NULL )
    {
      fprintf ( stderr, "Return type entry not found in the symbol table" );
      erroroccured = 1;
      return;
    }

    FUNCTION *funcdata = & ( retentry -> data . func_data );
    if ( currnode -> num_of_children > 0 )
      funcdata -> ret_type = getFirstChild ( currnode ) -> extra_data . data_type;
    else
      funcdata -> ret_type = D_NOTHING_TYPE;

    if ( DEBUG_STB_AUXOPS )
      printf ( "Set the return type of %s as %s\n", funcdata -> name, getDataTypeName ( funcdata -> ret_type ) );

  }
}

void performSemanticChecks ( ANODE *currnode, SYMBOLTABLE *symboltable, int *infunction, int *loopcount, int *bdftcount )
{
  if ( currnode -> node_type == AST_BREAK_NODE && *loopcount <= 0 )
  {
    fprintf ( stderr, "Error at line %d:\n\tBreak statements can only occur inside loops.\n\n", currnode -> line_no );

    erroroccured = 1;
    // Continue processing after ignoring the node
    return;
  }
  else if ( currnode -> node_type == AST_RETURNSTMT_NODE && *infunction == 0 )
  {
    fprintf ( stderr, "Error at line %d:\n\tReturn statements can only occur inside function definitions.\n\n", currnode -> line_no );
    erroroccured = 1;
    return;
  }
  else if ( currnode -> node_type == AST_DEPTH_NODE && *loopcount <= 0 && *bdftcount <= 0 )
  {
    fprintf ( stderr, "Error at line %d:\n\tDepth can only be used inside a BFT / DFT based loop.\n\n", currnode -> line_no );
    erroroccured = 1;
    return;
  }
  else if ( currnode -> node_type == AST_LET_NODE )
  {
    // The left and right children of Let node should have the same type
    if ( getFirstChild ( currnode ) -> result_type != getSecondChild ( currnode ) -> result_type )
    {
      STBENTRY *entry = getEntryByIndex ( symboltable, getFirstChild ( getFirstChild ( currnode ) ) -> extra_data . symboltable_index );
      char *name = entry -> data . var_data . name;
      if ( getFirstChild ( currnode ) -> result_type == D_FLOAT_TYPE
           && getSecondChild ( currnode ) -> result_type == D_INT_TYPE )
      {
        fprintf ( stderr, "Warning at line %d:\n\tImplicit conversion from Int to Float in Let statement\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: %s has type Float\n\n", name );
      }
      else
      {
        fprintf ( stderr, "Error at line %d:\n\tAssigning incompatible types in let statement\n", currnode -> line_no );
        if ( getFirstChild ( currnode ) -> num_of_children == 1 )
          fprintf ( stderr, "\tNote: %s has type %s, but RHS has type %s\n\n", name, getDataTypeName ( getFirstChild ( currnode ) -> result_type ), getDataTypeName ( getSecondChild ( currnode ) -> result_type ) );
        else
          fprintf ( stderr, "\tNote: LHS has type %s, but RHS has type %s\n\n", getDataTypeName ( getFirstChild ( currnode ) -> result_type ), getDataTypeName ( getSecondChild ( currnode ) -> result_type ) );

        erroroccured = 1;
      }
      return;
    }

    STBENTRY *entry = getEntryByIndex (
        symboltable, getFirstChild ( getFirstChild ( currnode ) ) -> extra_data . symboltable_index );

    if ( entry -> entry_type == ENTRY_FUNC_TYPE )
    {
      fprintf ( stderr, "Error at line %d:\n\tAttempting to assign to a function identifier\n", currnode -> line_no );
      fprintf ( stderr, "\tNote: %s declared as function on line %d\n\n", entry -> data . func_data . name,
                                                                          entry -> data . func_data . decl_line );
      erroroccured = 1;
      return;
    }
  }
  else if ( currnode -> node_type == AST_ASSIGNABLE_NODE )
  {
    // The type of an assignable node is either the type of the identifier
    // or the member of the identifier if there are more than one children

    if ( currnode -> num_of_children == 1 )
      currnode -> result_type = getFirstChild ( currnode ) -> result_type;
    else if ( currnode -> num_of_children == 2 )
    {
      fprintf ( stderr, "Invalid tree produced! Assignable can't have exactly 2 children\n" );
      erroroccured = 1;
      return;
    }
    else if ( currnode -> num_of_children == 3 )
    {
      // If Assignable has three children, then its first child should be a complex type
      currnode -> result_type = getThirdChild ( currnode ) -> result_type;
      STBENTRY *entry = getEntryByIndex ( symboltable, getFirstChild ( currnode ) -> extra_data . symboltable_index );

      if ( getFirstChild ( currnode ) -> result_type == D_STRING_TYPE
           || getFirstChild ( currnode ) -> result_type == D_INT_TYPE
           || getFirstChild ( currnode ) -> result_type == D_FLOAT_TYPE )
      {
        fprintf ( stderr, "Error at line %d:\n\tCannot get members of primitive type\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: %s was declared as a/an %s on line %d\n\n", entry -> data . var_data . name,
                  getDataTypeName ( getFirstChild ( currnode ) -> result_type ), entry -> data . var_data . decl_line );
        erroroccured = 1;
        return;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_VERTEX_TYPE && getThirdChild ( currnode ) -> num_of_children > 0 )
      {
        fprintf ( stderr, "Error at line %d:\n\tCannot get non-primitive type member of a Vertex object\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: %s was declared as a VERTEX on line %d\n\n", entry -> data . var_data . name,
                                                                                entry -> data . var_data . decl_line );
        erroroccured = 1;
        return;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_EDGE_TYPE )
      {
        if ( getThirdChild ( currnode ) -> num_of_children > 0
             && getFirstChild ( getThirdChild ( currnode ) ) -> node_type == AST_ROOT_NODE )
        {
          fprintf ( stderr, "Error at line %d:\n\tEdge object has no root member\n", currnode -> line_no );
          fprintf ( stderr, "\tNote: %s was declared as an EDGE on line %d\n\n", entry -> data . var_data . name,
                                                                                 entry -> data . var_data . decl_line );
          erroroccured = 1;
          return;
        }
      }

      if ( getFirstChild ( currnode ) -> result_type == D_GRAPH_TYPE )
      {
        fprintf ( stderr, "Error at line %d:\n\tGraph object has no members that can be referenced\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: %s was declared as an GRAPH on line %d\n\n", entry -> data . var_data . name,
                                                                                entry -> data . var_data . decl_line );
        erroroccured = 1;
        return;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_TREE_TYPE
           && getThirdChild ( currnode ) -> num_of_children > 0
           && getFirstChild ( getThirdChild ( currnode ) ) -> node_type != AST_ROOT_NODE )
      {
        fprintf ( stderr, "Error at line %d:\n\tOnly root member of a Tree object may be referenced\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: %s was declared as an TREE on line %d\n\n", entry -> data . var_data . name,
                                                                               entry -> data . var_data . decl_line );
        erroroccured = 1;
        return;
      }
    }
    else
    {
      fprintf ( stderr, "Invalid tree produced! Assignable can't have more than 3 children\n" );
      erroroccured = 1;
      return;
    }
  }
  else if ( currnode -> node_type == AST_IDENTIFIER_NODE )
  {
    // The type of an identifier node is that of the variable in the symbol table, of the return
    // type of the function in the symbol table

    unsigned int index = currnode -> extra_data . symboltable_index;

    STBENTRY *entry = getEntryByIndex ( symboltable, index );

    if ( entry -> entry_type == ENTRY_VAR_TYPE )
      currnode -> result_type = entry -> data . var_data . data_type;
    else if ( entry -> entry_type == ENTRY_FUNC_TYPE )
      currnode -> result_type = entry -> data . func_data . ret_type;
  }
  else if ( currnode -> node_type == AST_READ_NODE )
  {
    // The type of the assignable child of a read node should be an integer or a float

    ANODE *child = getFirstChild ( currnode );

    if ( child -> result_type != D_INT_TYPE && child -> result_type != D_FLOAT_TYPE )
    {
      fprintf ( stderr, "Error at line %d:\n\tCannot read non-primitive value using the read statement\n\n", currnode -> line_no );
      erroroccured = 1;
      return;
    }
  }
  else if ( currnode -> node_type == AST_PRINT_NODE )
  {
    // The type of the assignable or literal child of a print node should be an integer, a float
    // or a string

    ANODE *child = getFirstChild ( currnode );

    if ( child -> result_type != D_INT_TYPE && child -> result_type != D_FLOAT_TYPE
         && child -> result_type != D_STRING_TYPE )
    {
      fprintf ( stderr, "Error at line %d:\n\tCannot print a value that is not a string, an int or a float\n", currnode -> line_no );
      erroroccured = 1;
      return;
    }
  }
  else if ( currnode -> node_type == AST_COMPARE_NODE )
  {
    // The two children of a compare node MUST be of the exact same type
    // Otherwise throw an error

    if ( getFirstChild ( currnode ) -> result_type != getSecondChild ( currnode ) -> result_type )
    {
      fprintf ( stderr, "Error at line %d:\n\tValues on the two sides of the compare expression are not the same\n", currnode -> line_no );
      fprintf ( stderr, "\tNote: LHS has type %s and RHS has type %s\n\n", getDataTypeName ( getFirstChild ( currnode ) -> result_type ),
                                                                           getDataTypeName ( getSecondChild ( currnode ) -> result_type ) );
      erroroccured = 1;
      return;
    }
  }
  else if ( currnode -> node_type == AST_EXP_NODE || currnode -> node_type == AST_AROP_NODE )
  {
    if ( currnode -> num_of_children == 1 )
      currnode -> result_type = getFirstChild ( currnode ) -> result_type;
    else if ( currnode -> num_of_children == 2 )
    {
      DATATYPE firsttype, secondtype, result;

      firsttype = getFirstChild ( currnode ) -> result_type;
      secondtype = getSecondChild ( currnode ) -> result_type;

      if ( firsttype == D_INT_TYPE || firsttype == D_FLOAT_TYPE )
      {
        if ( getFirstChild ( currnode ) -> result_type != getSecondChild ( currnode ) -> result_type )
        {
          fprintf ( stderr, "Error at line %d:\n\tFloats and Ints can only be added to other Floats and Ints respectively\n\n", currnode -> line_no );
          erroroccured = 1;
          return;
        }

        if ( firsttype == D_FLOAT_TYPE && currnode -> extra_data . arop_type == A_MODULO_TYPE )
        {
          // The right child gives us the operation type
          fprintf ( stderr, "Error at line %d:\n\tModulo operator cannot be applied to Floats\n\n", currnode -> line_no );
          erroroccured = 1;
          return;
        }

        result = firsttype;
      }
      else if ( firsttype == D_STRING_TYPE || secondtype == D_STRING_TYPE )
      {
        fprintf ( stderr, "Error at line %d:\n\tOperations on String are not allowed\n\n", currnode -> line_no );
        erroroccured = 1;
        return;
      }
      else if ( firsttype == D_GRAPH_TYPE || firsttype == D_TREE_TYPE )
      {
        if ( currnode -> extra_data . arop_type != A_PLUS_TYPE
             && currnode -> extra_data . arop_type != A_MINUS_TYPE )
        {
          fprintf ( stderr, "Error at line %d:\n\tOnly addition or removal operations allowed on Graphs and Trees\n\n", currnode -> line_no );
          erroroccured = 1;
        }
        else if ( secondtype != D_VERTEX_TYPE && secondtype != D_EDGE_TYPE )
          fprintf ( stderr, "Error at line %d:\n\tOnly Vertices and Edges may be added and removed from Graphs/Trees\n\n", currnode -> line_no );
        erroroccured = 1;

        result = firsttype;
      }
      else if ( secondtype == D_GRAPH_TYPE || secondtype == D_TREE_TYPE )
      {
        if ( currnode -> extra_data . arop_type != A_PLUS_TYPE
             && currnode -> extra_data . arop_type != A_MINUS_TYPE )
        {
          fprintf ( stderr, "Error at line %d:\n\tOnly addition or removal operations allowed on Graphs and Trees\n\n", currnode -> line_no );
          erroroccured = 1;
        }
        else if ( firsttype != D_VERTEX_TYPE && firsttype != D_EDGE_TYPE )
        {
          fprintf ( stderr, "Error at line %d:\n\tOnly Vertices and Edges may be added and removed from Graphs/Trees\n\n", currnode -> line_no );
          erroroccured = 1;
        }

        result = secondtype;
      }
      else
      {
        fprintf ( stderr, "Error at line %d:\n\tInvalid operands provided in expression\n\n", currnode -> line_no );
        erroroccured = 1;
      }

      currnode -> result_type = result;
    }
    else
    {
      fprintf ( stderr, "Invalid tree produced! EXP node should not have more than 2 children\n" );
      erroroccured = 1;
    }
  }
  else if ( currnode -> node_type == AST_PASSEDPARAMS_NODE )
  {
    // At a passed params node, we need to check that all the parameters are of the
    // same type as the called function

    // The first child of the parent will help us get the function identifier name from the STB
    unsigned int index = getFirstChild ( currnode -> parent ) -> extra_data . symboltable_index;

    FUNCTION *funcentry = & ( getEntryByIndex ( symboltable, index ) -> data . func_data );

    // Firstly, the number of parameters must match with the number of children of currnode
    if ( funcentry -> num_params != currnode -> num_of_children )
    {
      fprintf ( stderr, "Error at line %d:\n\tNumber of passed parameters don't match with the function definition\n", currnode -> line_no );
      fprintf ( stderr, "\tNote: %s accepts %d parameters, but %d parameters are being passed\n\n", funcentry -> name, funcentry -> num_params,
                                                                                                    currnode -> num_of_children );
      erroroccured = 1;
      return;
    }

    // If the number of parameters match, their types must also match
    LNODE funciterator, paramiterator;

    getIterator ( funcentry -> paramtypes, & funciterator );
    getIterator ( currnode -> children, & paramiterator );
    int paramnumber = 0;

    while ( hasNext ( & funciterator ) )
    {
      paramnumber ++;
      getNext ( funcentry -> parameters, & funciterator );
      getNext ( currnode -> children, & paramiterator );

      DATATYPE paramtype = funciterator . data . int_val;
      DATATYPE passedtype = ( * ( ANODE ** ) ( paramiterator . data . generic_val ) ) -> result_type;

      if ( paramtype != passedtype )
      {
        fprintf ( stderr, "Error at line %d:\n\tThe type of the passed parameter to the function doesn't match the definition\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: Parameter %d of %s should be of type %s, given type is %s\n\n", paramnumber, funcentry -> name,
                                                                                                   getDataTypeName ( paramtype ),
                                                                                                   getDataTypeName ( passedtype ) );
        erroroccured = 1;
        return;
      }
    }
  }
  else if ( currnode -> node_type == AST_RETURNSTMT_NODE )
  {
    // The type being returned should match the type in the function definition
    // Get the function data in the symbol table by getting the identifier node
    // which is the first child of the grandparent of currnode

    // Scenario 2: The return type is nothing, in which case the data_type extra data is set
    // and RETURNSTMT node has no children

    unsigned int index = getFirstChild ( currnode -> parent -> parent ) -> extra_data . symboltable_index;

    if ( currnode -> num_of_children == 0 )
    {
      // The return type is nothing

      if ( getEntryByIndex ( symboltable, index ) -> data . func_data . ret_type != D_NOTHING_TYPE )
      {
        fprintf ( stderr, "Error at line %d:\n\tType of value being returned does not match the return type in the function definition\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: Expected return type of %s is %s, but code returns Nothing\n\n", getEntryByIndex ( symboltable, index ) -> data . func_data . name,
                  getDataTypeName ( getEntryByIndex ( symboltable, index ) -> data . func_data . ret_type ) );
        erroroccured = 1;
      }

      return;
    }

    if ( getFirstChild ( currnode ) -> result_type
         != getEntryByIndex ( symboltable, index ) -> data . func_data . ret_type )
    {
      fprintf ( stderr, "Error at line %d:\n\tType of value being returned does not match the return type in the function definition\n", currnode -> line_no );
      fprintf ( stderr, "\tNote: Expected return type of %s is %s, but code returns a/an %s\n\n", getEntryByIndex ( symboltable, index ) -> data . func_data . name,
                getDataTypeName ( getEntryByIndex ( symboltable, index ) -> data . func_data . ret_type ), getDataTypeName ( getFirstChild ( currnode ) -> result_type ) );
      erroroccured = 1;
      return;
    }
  }
  else if ( currnode -> node_type == AST_FUNCTIONCALL_NODE )
  {
    unsigned int index = getFirstChild ( currnode ) -> extra_data . symboltable_index;

    if ( getEntryByIndex ( symboltable, index ) -> data . func_data . ret_type != D_NOTHING_TYPE )
    {
      fprintf ( stderr, "Warning at line %d:\n\tReturn value of function is not being used\n", currnode -> line_no );
      fprintf ( stderr, "\tNote: %s returns a/an %s\n\n", getEntryByIndex ( symboltable, index ) -> data . func_data . name,
                getDataTypeName ( getEntryByIndex ( symboltable, index ) -> data . func_data . ret_type ) );

      return;
    }
  }
  else if ( currnode -> node_type == AST_FOR_NODE )
  {
    // A for node could have 2, 4 or 6 children. We need to deal with each of the cases

    if ( currnode -> num_of_children == 2 )
    {
      ANODE *child = getFirstChild ( currnode );

      if ( child -> node_type == AST_LITERAL_NODE && child -> result_type != D_INT_TYPE )
      {
        fprintf ( stderr, "Error at line %d:\n\tThe number of iterations of a for loop must be an Integer\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: Provided type is a %s\n\n", getDataTypeName ( child -> result_type ) );
        erroroccured = 1;
      }
      else if ( child -> node_type == AST_IDENTIFIER_NODE )
      {
        unsigned int index = child -> extra_data . symboltable_index;

        STBENTRY *entry = getEntryByIndex ( symboltable, index );
        char *name = NULL;
        if ( entry -> entry_type == ENTRY_FUNC_TYPE )
          name = entry -> data . func_data . name;
        else
          name = entry -> data . var_data . name;

        if ( entry -> entry_type == ENTRY_FUNC_TYPE || entry -> data . var_data .data_type != D_INT_TYPE )
        {
          fprintf ( stderr, "Error at line %d:\n\tThe identifier in a for loop must be an integer variable\n", currnode -> line_no );
          fprintf ( stderr, "\tNote: %s is not an integer\n\n", name );
          erroroccured = 1;
          return;
        }
      }
    }
    else if ( currnode -> num_of_children == 3 )
    {
      fprintf ( stderr, "Invalid tree produced! For node cannot have 3 children\n" );
      erroroccured = 1;
    }
    else if ( currnode -> num_of_children == 4 )
    {
      // The first child must be a Vertex or an Edge and the third child must be a Graph or Tree

      if ( getFirstChild ( currnode ) -> result_type != D_VERTEX_TYPE
           && getFirstChild ( currnode ) -> result_type != D_EDGE_TYPE )
      {
        fprintf ( stderr, "Error at line %d:\n\tThe iteration variable must be a vertex or an edge\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: Provided variable has type %s\n\n", getDataTypeName ( getFirstChild ( currnode ) -> result_type ) );
        erroroccured = 1;
        return;
      }

      if ( getThirdChild ( currnode ) -> result_type != D_GRAPH_TYPE
           && getThirdChild ( currnode ) -> result_type != D_TREE_TYPE )
      {
        fprintf ( stderr, "Error at line %d:\n\tThe object to iterate over must be a Graph or Tree\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: Provided type is %s\n\n", getDataTypeName ( getThirdChild ( currnode ) -> result_type ) );
        erroroccured = 1;
        return;
      }
    }
    else if ( currnode -> num_of_children == 5 )
    {
      fprintf ( stderr, "Invalid tree produced! For node cannot have 5 children\n" );
      erroroccured = 1;
    }
    else if ( currnode -> num_of_children == 6 )
    {
      // We can have two cases here. Either the construct can be:
      // For u in G adjacent to v:
      // or it can be:
      // For u in BFT ( G, v )
      // We use the third node (which might be a BDFT node) to differentiate b/w the two

      if ( getFirstChild ( currnode ) -> result_type != D_VERTEX_TYPE
           && getFirstChild ( currnode ) -> result_type != D_EDGE_TYPE )
      {
        fprintf ( stderr, "Error at line %d:\n\tThe iteration variable must be a vertex or an edge\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: Provided variable has type %s\n\n", getDataTypeName ( getFirstChild ( currnode ) -> result_type ) );
        erroroccured = 1;
        return;
      }

      if ( getThirdChild ( currnode ) -> node_type == AST_BDFT_NODE )
      {
        if ( getFourthChild ( currnode ) -> result_type != D_GRAPH_TYPE
             && getFourthChild ( currnode ) -> result_type != D_TREE_TYPE )
        {
          fprintf ( stderr, "Error at line %d:\n\tThe first parameter to BFT or DFT must be a Graph or Tree\n", currnode -> line_no );
          fprintf ( stderr, "\tNote: Provided type is %s\n\n", getDataTypeName ( getFourthChild ( currnode ) -> result_type ) );
          erroroccured = 1;
          return;
        }

        if ( getFifthChild ( currnode ) -> result_type != D_VERTEX_TYPE )
        {
          fprintf ( stderr, "Error at line %d:\n\tThe second parameter to BFT/DFT must be a Vertex\n\n", currnode -> line_no );
          erroroccured = 1;
          return;
        }
      }
      else
      {
        // The third child is not BDFT, thus adjacent to must be there

        if ( getThirdChild ( currnode ) -> result_type != D_GRAPH_TYPE
             && getThirdChild ( currnode ) -> result_type != D_TREE_TYPE )
        {
          fprintf ( stderr, "Error at line %d:\n\tThe object to iterate over must be a Graph or Tree\n", currnode -> line_no );
          fprintf ( stderr, "\tNote: Type provided is %s\n\n", getDataTypeName ( getThirdChild ( currnode ) -> result_type ) );
          erroroccured = 1;
          return;
        }

        if ( getFifthChild ( currnode ) -> result_type != D_VERTEX_TYPE )
        {
          fprintf ( stderr, "Error at line %d:\n\tThe adjacent to parameter must be a Vertex\n", currnode -> line_no );
          fprintf ( stderr, "\tNote: Provided type is %s\n\n", getDataTypeName ( getFifthChild ( currnode ) -> result_type ) );
          erroroccured = 1;
          return;
        }
      }
    }
  }
  else if ( currnode -> node_type == AST_EDGECREATE_NODE )
  {
    if ( getFirstChild ( currnode ) -> result_type != D_VERTEX_TYPE
         || getSecondChild ( currnode ) -> result_type != D_VERTEX_TYPE )
    {
      fprintf ( stderr, "Error at line %d:\n\tThe components of an edge must be vertices\n", currnode -> line_no );
      fprintf ( stderr, "\tNote: Provided types are %s and %s\n\n", getDataTypeName ( getFirstChild ( currnode ) -> result_type ),
                                                                    getDataTypeName ( getSecondChild ( currnode ) -> result_type ) );
      erroroccured = 1;
      return;
    }

    currnode -> result_type = D_EDGE_TYPE;
  }
  else if ( currnode -> node_type == AST_LITERAL_NODE )
  {
    // The type of a literal is the type of its entry in the symbol table
    currnode -> result_type = getEntryByIndex (
        symboltable, currnode -> extra_data . symboltable_index ) -> data . lit_data . lit_type;
  }
  else if ( currnode -> node_type == AST_DEPTH_NODE )
    currnode -> result_type = D_INT_TYPE;
  else if ( currnode -> node_type == AST_DEST_NODE || currnode -> node_type == AST_SOURCE_NODE
            || currnode -> node_type == AST_ROOT_NODE )
    currnode -> result_type = D_VERTEX_TYPE;
  else if ( currnode -> node_type == AST_WEIGHT_NODE )
    currnode -> result_type = D_FLOAT_TYPE;
  else if ( currnode -> node_type == AST_ASSIGNFUNC_NODE )
  {
    // ASSIGNFUNC node could have 1, 2 or 3 children

    // If it has one child or two children, then the result type is the type of the
    // first child (i.e. the identifier)
    if ( currnode -> num_of_children == 1 )
      currnode -> result_type = getFirstChild ( currnode ) -> result_type;
    else if ( currnode -> num_of_children == 2 )
    {
      currnode -> result_type = getFirstChild ( currnode ) -> result_type;
      STBENTRY *entry = getEntryByIndex ( symboltable, getFirstChild ( currnode ) -> extra_data . symboltable_index );
      if ( entry -> entry_type != ENTRY_FUNC_TYPE )
      {
        fprintf ( stderr, "Error at line %d:\n\tAttempting to call a variable like a function\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: %s declared as a/an %s at line number %d\n\n", entry -> data . var_data . name,
                  getDataTypeName ( entry -> data . var_data . data_type ), entry -> data . var_data . decl_line );
        erroroccured = 1;
        return;
      }
    }
    else if ( currnode -> num_of_children == 3 )
    {
      currnode -> result_type = getThirdChild ( currnode ) -> result_type;
      STBENTRY *entry = getEntryByIndex ( symboltable, getFirstChild ( currnode ) -> extra_data . symboltable_index );
      VARIABLE *vardata = & ( entry -> data . var_data );

      if ( getFirstChild ( currnode ) -> result_type == D_STRING_TYPE
           || getFirstChild ( currnode ) -> result_type == D_INT_TYPE
           || getFirstChild ( currnode ) -> result_type == D_FLOAT_TYPE )
      {
        fprintf ( stderr, "Error at line %d:\n\tCannot get members of primitive type\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: %s has type %s\n\n", vardata -> name, getDataTypeName ( vardata -> data_type ) );
        erroroccured = 1;
        return;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_VERTEX_TYPE && getThirdChild ( currnode ) -> num_of_children > 0 )
      {
        fprintf ( stderr, "Error at line %d:\n\tCannot get non-primitive type member of a Vertex object\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: %s declared as a VERTEX on line %d\n\n", vardata -> name, vardata -> decl_line );
        erroroccured = 1;
        return;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_EDGE_TYPE )
      {
        if ( getThirdChild ( currnode ) -> num_of_children > 0
             && getFirstChild ( getThirdChild ( currnode ) ) -> node_type == AST_ROOT_NODE )
        {
          fprintf ( stderr, "Error at line %d:\n\tEdge object has no root member\n", currnode -> line_no );
          fprintf ( stderr, "\tNote: %s declared as an EDGE on line %d\n\n", vardata -> name, vardata -> decl_line );
          erroroccured = 1;
          return;
        }
      }

      if ( getFirstChild ( currnode ) -> result_type == D_GRAPH_TYPE )
      {
        fprintf ( stderr, "Error at line %d:\n\tGraph object has no members that can be referenced\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: %s declared as a GRAPH on line %d\n\n", vardata -> name, vardata -> decl_line );
        erroroccured = 1;
        return;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_TREE_TYPE
           && getThirdChild ( currnode ) -> num_of_children > 0
           && getFirstChild ( getThirdChild ( currnode ) ) -> node_type != AST_ROOT_NODE )
      {
        fprintf ( stderr, "Error at line %d:\n\tOnly the root member of a Tree object may be referenced\n", currnode -> line_no );
        fprintf ( stderr, "\tNote: %s declared as a TREE on line %d\n\n", vardata -> name, vardata -> decl_line );
        erroroccured = 1;
        return;
      }
    }
  }
  else if ( currnode -> node_type == AST_ENDASSIGN_NODE )
  {
    // It could have one or two children. If it has one child then it is the type of that child
    // else it is the type of the current node


    if ( currnode -> num_of_children == 0 )
      currnode -> result_type = currnode -> extra_data . data_type;
    else if ( currnode -> num_of_children == 1 )
      currnode -> result_type = getFirstChild ( currnode ) -> result_type;
    else
    {
      currnode -> result_type = currnode -> extra_data . data_type;

      if ( getFirstChild ( currnode ) -> node_type == AST_WEIGHT_NODE )
      {
        fprintf ( stderr, "Error at line %d:\n\tPrimitive type objects have no members\n\n", currnode -> line_no );
        erroroccured = 1;
        return;
      }
    }
  }

  // End of semantic analysis for different nodes

}

void layoutTemplate ( FILE *assemblyfile, FILE *codefile, FILE *datafile )
{
  fprintf ( datafile, "extern printf\n\n" );
  fprintf ( datafile, "section .data\n" );
  //fprintf ( datafile, "\t_int_string:\t\t\tdb\t'0000000000',10,0\n" );
  fprintf ( datafile, "\t_int_format:\t\t\tdb\t\"%%d\",10,0\n" );
  fprintf ( datafile, "\t_float_format:\t\tdb\t\"%%f\",10,0\n" );
  fprintf ( datafile, "\t_float_temp:\t\t\tdq\t0\n" );
  fprintf ( datafile, "\t_string_format:\t\tdb\t\"%%s\",0\n" );
  fprintf ( datafile, "\t_int_to_float:\t\tdw\t0,0\n" );

  fprintf ( assemblyfile, "\nsection .bss\n" );

  fprintf ( codefile, "_start:\n\n" );

  return;
}

void writeIntPrintFunction ( FILE *assemblyfile )
{
  fprintf ( assemblyfile, "\nprintInt:\n" );
  fprintf ( assemblyfile, "\tmov ebx, _int_string+9\n" );
  fprintf ( assemblyfile, "\tmov ecx, 10\n" );
  fprintf ( assemblyfile, "\tmov edi, 10\n\n" );
  fprintf ( assemblyfile, "l:\n" );
  fprintf ( assemblyfile, "\tmov edx, 0\n" );
  fprintf ( assemblyfile, "\tdiv edi\n" );
  fprintf ( assemblyfile, "\tadd edx, 48\n" );
  fprintf ( assemblyfile, "\tmov [ebx], dl\n" );
  fprintf ( assemblyfile, "\tdec ebx\n" );
  fprintf ( assemblyfile, "\tloop l\n\n" );
  fprintf ( assemblyfile, "\tmov edi, _int_string\n" );
  fprintf ( assemblyfile, "\tmov ecx, 10\n\n" );
  fprintf ( assemblyfile, "\tmov ebx, 0\n" );
  fprintf ( assemblyfile, "nz:\n" );
  fprintf ( assemblyfile, "\tmov bl, [edi]\n" );
  fprintf ( assemblyfile, "\tcmp bl, 48\n" );
  fprintf ( assemblyfile, "\tjne fnz\n" );
  fprintf ( assemblyfile, "\tinc edi\n\tloop nz\n\n" );
  fprintf ( assemblyfile, "\tmov edx, 2\n" );
  fprintf ( assemblyfile, "\tmov ecx, _int_string+9\n" );
  fprintf ( assemblyfile, "\tjmp print\n\n" );
  fprintf ( assemblyfile, "fnz:\n\tmov edx, ecx\n" );
  fprintf ( assemblyfile, "\tinc edx\n" );
  fprintf ( assemblyfile, "\tmov ecx, edi\n\n" );
  fprintf ( assemblyfile, "print:\n" );
  fprintf ( assemblyfile, "\tmov eax, 4\n" );
  fprintf ( assemblyfile, "\tmov ebx, 1\n" );
  fprintf ( assemblyfile, "\tint 80h\n\n" );
  fprintf ( assemblyfile, "\tret\n" );
}

int getSize ( DATATYPE type )
{
  switch ( type )
  {
    case D_INT_TYPE    :
    case D_FLOAT_TYPE  :
    case D_STRING_TYPE : return 1;
    case D_VERTEX_TYPE :
    case D_TREE_TYPE   : return 3;
    case D_GRAPH_TYPE  : return 2;
    case D_EDGE_TYPE   : return 6;
    default            : fprintf ( stderr, "Querying for size of unrecognized type\n" );
                         return 0;
  }
}

int getSubtreeActivationSize ( ANODE *root )
{
  if ( root -> node_type == AST_DATATYPE_NODE )
    return ( getParent ( root ) -> num_of_children - 1 ) * getSize ( root -> extra_data . data_type );

  int value = 0;
  LNODE iterator;
  getIterator ( root -> children, & iterator );

  while ( hasNext ( & iterator ) )
  {
    getNext ( root -> children, & iterator );
    ANODE *child = * ( ANODE ** ) ( iterator . data . generic_val );

    value += getSubtreeActivationSize ( child );
  }

  return value;
}

int getProgramSize ( ANODE *programNode )
{
  if ( programNode -> node_type != AST_PROGRAM_NODE )
  {
    fprintf ( stderr, "Call getProgramSize on a program node next time\n" );
    return -1;
  }

  int value = 0;
  LNODE iterator;
  getIterator ( programNode -> children, & iterator );

  while ( hasNext ( & iterator ) )
  {
    getNext ( programNode -> children, & iterator );
    ANODE *child = * ( ANODE ** ) ( iterator . data . generic_val );

    if ( child -> node_type == AST_GLOBALDEFINE_NODE
         || child -> node_type == AST_FUNCTION_NODE )
      continue;

    value += getSubtreeActivationSize ( child );
  }

  return value;
}

int getOffsetInReg ( ANODE *assignable, FILE *codefile, SYMBOLTABLE *symboltable )
{
  int o1 = assignable -> offset1;
  int o2 = (assignable -> offsetcount > 1) ? assignable -> offset2 : OFFSET_ANY;
  int o3 = (assignable -> offsetcount > 2) ? assignable -> offset3 : OFFSET_ANY;

  int target = getRegister ( codefile, symboltable, getFirstChild ( assignable ) -> extra_data . symboltable_index,
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

int getLiteralInRegister ( ANODE *literalnode, FILE *codefile, SYMBOLTABLE *symboltable,
                           TRIE *literaltrie, LITDATA *literals )
{
  LITERAL *litdata = & ( getEntryByIndex ( symboltable, literalnode -> extra_data . symboltable_index ) -> data . lit_data );

  TNODE *foundlit = findString ( literaltrie, litdata -> value );

  int targetreg = getRegister ( codefile, symboltable, literalnode -> extra_data . symboltable_index,
                  OFFSET_ANY, OFFSET_ANY, OFFSET_ANY, NO_SPECIFIC_REG, IS_LITERAL, NO_REGISTER,
                  NO_REGISTER );

  if ( targetreg == -1 )
  {
    fprintf ( stderr, "Error: Out of temporary registers to get literal\n" );
    exit ( -1 );
  }

  fprintf ( codefile, "\tmov\t%s, [%s]\n", getRegisterName ( targetreg ), literals [ foundlit -> data . int_val ] . name );

  setRegisterProperties ( targetreg, 0, -1, IS_LITERAL, 0, literalnode -> extra_data . symboltable_index,
                          OFFSET_ANY, OFFSET_ANY, OFFSET_ANY );

  return targetreg;
}

void generateCode ( ANODE *currnode, SYMBOLTABLE *symboltable, FILE *assemblyfile, FILE *codefile,
                    FILE *functionfile, TRIE* literaltrie, LITDATA *literals, FILE *datafile,
                    int infunction )
{
  FILE *outputfile = codefile;

  if ( infunction == 1 )
    outputfile = functionfile;

  if ( currnode -> node_type == AST_GLOBALDEFINE_NODE )
  {
    LNODE iterator;
    getIterator ( currnode -> children , &iterator );

    getNext ( currnode -> children, &iterator );

    while ( hasNext ( &iterator ) )
    {
      getNext ( currnode -> children, & iterator );

      ANODE *node = * ( ANODE ** ) ( iterator . data . generic_val );
      VARIABLE *vardata = & ( getEntryByIndex ( symboltable,
            node -> extra_data . symboltable_index ) -> data . var_data );

      fprintf ( assemblyfile, "\t%s:\t", vardata -> name );

      if ( vardata -> data_type == D_INT_TYPE || vardata -> data_type == D_FLOAT_TYPE )
        fprintf ( assemblyfile, "resb\t4\n" );
      else if ( vardata -> data_type == D_VERTEX_TYPE )
        fprintf ( assemblyfile, "resb\t12\n" );
      else if ( vardata -> data_type == D_STRING_TYPE )
        fprintf ( assemblyfile, "resb\t4\n" );
      else if ( vardata -> data_type == D_EDGE_TYPE )
        fprintf ( assemblyfile, "resb\t24\n" );
      else if ( vardata -> data_type == D_TREE_TYPE )
        fprintf ( assemblyfile, "resb\t12\n" );
      else if ( vardata -> data_type == D_GRAPH_TYPE )
        fprintf ( assemblyfile, "resb\t8\n" );
    }
  }
  else if ( currnode -> node_type == AST_LITERAL_NODE )
  {
    LITERAL *litdata = & ( getEntryByIndex ( symboltable,
        currnode -> extra_data . symboltable_index ) -> data . lit_data );

    TNODE *foundlit = findString ( literaltrie, litdata -> value );

    if ( foundlit == NULL )
    {
      foundlit = insertString ( literaltrie, litdata -> value );
      foundlit -> data . int_val = curlitindex;

      int len = 11;
      literals [ curlitindex ] . name = malloc ( len * sizeof ( char ) );
      snprintf ( literals [ curlitindex ] . name, 11, "l%d", curlitindex );

      fprintf ( datafile, "\t%s:\t", literals [ curlitindex ] . name );

      if ( litdata -> lit_type == D_STRING_TYPE )
        fprintf ( datafile, "db\t%s,10,0\n", litdata -> value );
      else if ( litdata -> lit_type == D_INT_TYPE )
      {
        int value = atoi ( litdata -> value );
        int lower = ( value & LOWER16MASK ), upper = ( ( value & UPPER16MASK ) >> 16 );
        fprintf ( datafile, "dw\t%d,%d\n", lower, upper );
      }
      else if ( litdata -> lit_type == D_FLOAT_TYPE )
        fprintf ( datafile, "dd\t%s\n", litdata -> value );

      curlitindex ++;
    }
  }
  else if ( currnode -> node_type == AST_LET_NODE )
  {
    ANODE *assignable = getFirstChild ( currnode );

    int targetreg = getOffsetInReg ( assignable, outputfile, symboltable );

    while ( ! registers [ targetreg ] . hasoffset )
    {
      registers [ targetreg ] . flushed = 1;
      targetreg = getOffsetInReg ( assignable, outputfile, symboltable );
    }

    int resultreg = getSecondChild ( currnode ) -> offsetreg;

    if ( getSecondChild ( currnode ) -> result_type == D_INT_TYPE && assignable -> result_type == D_FLOAT_TYPE )
    {
      // Perform type conversion
      fprintf ( codefile, "\n\t; Performing type conversion\n" );
      fprintf ( codefile, "\tmov\t[_int_to_float], %s\n", getRegisterName ( resultreg ) );
      fprintf ( codefile, "\tfild\tdword [_int_to_float]\n" );
      fprintf ( codefile, "\tfstp\tdword [_int_to_float]\n" );
      fprintf ( codefile, "\t; Move converted value back\n" );
      fprintf ( codefile, "\tmov\t%s, [_int_to_float]\n\n", getRegisterName ( resultreg ) );
    }

    fprintf ( codefile, "\tmov\t[%s], %s\n\n", getRegisterName ( targetreg ), getRegisterName ( resultreg ) );

    registers [ resultreg ] . flushed = 1;
  }
  else if ( currnode -> node_type == AST_PRINT_NODE )
  {
    ANODE *child = getFirstChild ( currnode );

    if ( child -> num_of_children > 0 )
      child = getFirstChild ( child );

    STBENTRY *entry = getEntryByIndex ( symboltable, child -> extra_data . symboltable_index );

    if ( entry -> entry_type == ENTRY_LIT_TYPE )
    {
      fprintf ( outputfile, "\n\tpusha\n" );
      TNODE *foundlit = findString ( literaltrie, entry -> data . lit_data . value );
      // Check and print the int or float literal
      if ( entry -> data . lit_data . lit_type == D_INT_TYPE )
      {
        fprintf ( outputfile, "\t; Printing integer literal\n" );
        fprintf ( outputfile, "\tpush\t[ %s ]\n", literals [ foundlit -> data . int_val ] . name );
        fprintf ( outputfile, "\tpush\t_int_format\n" );
        fprintf ( outputfile, "\tcall printf\n" );
        fprintf ( outputfile, "\tadd\tesp, 8\n" );
      }
      else if ( entry -> data . lit_data . lit_type == D_STRING_TYPE )
      {
        fprintf ( outputfile, "\t; Printing string literal\n" );
        fprintf ( outputfile, "\tmov\teax, 4\n" );
        fprintf ( outputfile, "\tmov\tebx, 1\n" );
        fprintf ( outputfile, "\tmov\tecx, %s\n", literals [ foundlit -> data . int_val ] . name );
        fprintf ( outputfile, "\tmov\tedx, %d\n", ( int ) strlen ( entry -> data . lit_data . value ) - 1 );
        fprintf ( outputfile, "\tint\t80h\n" );
      }
      else if ( entry -> data . lit_data . lit_type == D_FLOAT_TYPE )
      {
        fprintf ( outputfile, "\t; Printing float literal\n" );
        fprintf ( outputfile, "\tfld\tdword\t[%s]\n", literals [ foundlit -> data . int_val ] . name );
        fprintf ( outputfile, "\tfstp\tqword\t[_float_temp]\n" );
        fprintf ( outputfile, "\tpush\tdword\t[_float_temp+4]\n" );
        fprintf ( outputfile, "\tpush\tdword\t[_float_temp]\n" );
        fprintf ( outputfile, "\tpush\tdword\t_float_format\n" );
        fprintf ( outputfile, "\tcall\tprintf\n" );
        fprintf ( outputfile, "\tadd\tesp, 12\n" );
      }
      fprintf ( outputfile, "\tpopa\n\n" );
    }
    else
    {
      // The entry MUST be a variable type
      // Get its offset, which is inherited and stored in the assignable node child
      // Use result_type to decide the type and print accordingly

      ANODE *assignable = getFirstChild ( currnode );
      int datareg = getOffsetInReg ( assignable, outputfile, symboltable );

      while ( ! registers [ datareg ] . hasoffset )
      {
        registers [ datareg ] . flushed = 1;
        datareg = getOffsetInReg ( assignable, outputfile, symboltable );
      }

      // Dereference offset and get the data to print
      fprintf ( outputfile, "\tmov\t%s, [%s]\n", getRegisterName ( datareg ), getRegisterName ( datareg ) );
      registers [ datareg ] . hasoffset = 0;

      if ( assignable -> result_type == D_INT_TYPE )
      {
        fprintf ( outputfile, "\n\tpusha\t\t; Printf modifies registers so pushall\n" );
        fprintf ( outputfile, "\t; Printing integer variable\n" );
        fprintf ( outputfile, "\tpush\t%s\n", getRegisterName ( datareg ) );
        fprintf ( outputfile, "\tpush\t_int_format\n" );
        fprintf ( outputfile, "\tcall printf\n" );
        fprintf ( outputfile, "\tadd\tesp, 8\n" );
        fprintf ( outputfile, "\tpopa\n\n" );
      }
      else if ( assignable -> result_type == D_STRING_TYPE )
      {
        fprintf ( outputfile, "\n\tpusha\n" );
        fprintf ( outputfile, "\t; Printing string variable\n" );
        fprintf ( outputfile, "\tpush\t%s\n", getRegisterName ( datareg ) );
        fprintf ( outputfile, "\tpush\tdword\t_string_format\n" );
        fprintf ( outputfile, "\tcall printf\n" );
        fprintf ( outputfile, "\tadd\tesp, 8\n\n" );
        fprintf ( outputfile, "\tpopa\n\n" );
      }
      else if ( assignable -> result_type == D_FLOAT_TYPE )
      {
        fprintf ( outputfile, "\n\tpusha\n" );
        fprintf ( outputfile, "\t; Printing float variable\n" );
        fprintf ( outputfile, "\tmov\t[_float_temp], %s\n", getRegisterName ( datareg ) );
        fprintf ( outputfile, "\tfld\tdword\t[_float_temp]\n" );
        fprintf ( outputfile, "\tfstp\tqword\t[_float_temp]\n" );
        fprintf ( outputfile, "\tpush\tdword\t[_float_temp+4]\n" );
        fprintf ( outputfile, "\tpush\tdword\t[_float_temp]\n" );
        fprintf ( outputfile, "\tpush\tdword\t_float_format\n" );
        fprintf ( outputfile, "\tcall\tprintf\n" );
        fprintf ( outputfile, "\tadd\tesp, 12\n\n" );
        fprintf ( outputfile, "\tpopa\n\n" );
      }
    }
  }
  else if ( currnode -> node_type == AST_IDENTIFIER_NODE )
  {
    STBENTRY *entry = getEntryByIndex ( symboltable, currnode -> extra_data . symboltable_index );

    if ( getParent ( currnode ) -> node_type == AST_DEFINE_NODE )
    {
      entry -> offset = curroffset;
      curroffset += getSize ( entry -> data . var_data . data_type ) * 4;
    }
    else if ( entry -> entry_type == ENTRY_VAR_TYPE )
    {
      if ( entry -> data . var_data . var_type == V_GLOBAL_TYPE )
      {
        currnode -> global_or_local = IS_GLOBAL;
        currnode -> offsetcount = ONE_OFFSET;
        currnode -> offset1 = entry -> index;
      }
      else
      {
        currnode -> global_or_local = IS_LOCAL;
        currnode -> offsetcount = ONE_OFFSET;
        currnode -> offset1 = entry -> offset;
      }
    }
  }
  else if ( currnode -> node_type == AST_ASSIGNABLE_NODE )
  {
    if ( currnode -> num_of_children == 1 )
    {
      currnode -> offsetcount = getFirstChild ( currnode ) -> offsetcount;
      currnode -> offset1 = getFirstChild ( currnode ) -> offset1;
      currnode -> global_or_local = getFirstChild ( currnode ) -> global_or_local;
    }
    else if ( currnode -> num_of_children == 3 )
    {
      currnode -> global_or_local = getFirstChild ( currnode ) -> global_or_local;
      currnode -> offset1 = getFirstChild ( currnode ) -> offset1;

      currnode -> offsetcount = 1 + getThirdChild ( currnode ) -> offsetcount;

      currnode -> offset2 = getThirdChild ( currnode ) -> offset1;
      currnode -> offset3 = getThirdChild ( currnode ) -> offset2;
    }
  }
  else if ( currnode -> node_type == AST_ENDASSIGN_NODE )
  {
    if ( currnode -> num_of_children == 0 )
    {
      currnode -> offsetcount = ONE_OFFSET;
      if ( currnode -> extra_data . data_type == D_INT_TYPE )
        currnode -> offset1 = INTOFFSET;
      else if ( currnode -> extra_data . data_type == D_FLOAT_TYPE )
        currnode -> offset1 = FLOATOFFSET;
      else if ( currnode -> extra_data . data_type == D_STRING_TYPE )
        currnode -> offset1 = STRINGOFFSET;
    }
    else if ( currnode -> num_of_children == 1 || currnode -> num_of_children == 2 )
    {
      if ( currnode -> num_of_children == 1 )
        currnode -> offsetcount = ONE_OFFSET;
      else
        currnode -> offsetcount = TWO_OFFSETS;

      if ( getFirstChild ( currnode ) -> node_type == AST_ROOT_NODE )
        currnode -> offset1 = ROOTOFFSET;
      if ( getFirstChild ( currnode ) -> node_type == AST_SOURCE_NODE )
        currnode -> offset1 = SOURCEOFFSET;
      if ( getFirstChild ( currnode ) -> node_type == AST_DEST_NODE )
        currnode -> offset1 = DESTOFFSET;
      if ( getFirstChild ( currnode ) -> node_type == AST_WEIGHT_NODE )
        currnode -> offset1 = WEIGHTOFFSET;

      if ( currnode -> num_of_children == 2 )
      {
        if ( currnode -> extra_data . data_type == D_INT_TYPE )
          currnode -> offset2 = INTOFFSET;
        else if ( currnode -> extra_data . data_type == D_FLOAT_TYPE )
          currnode -> offset2 = FLOATOFFSET;
        else if ( currnode -> extra_data . data_type == D_STRING_TYPE )
          currnode -> offset2 = STRINGOFFSET;
      }
    }
  }
  else if ( currnode -> node_type == AST_EXP_NODE )
  {
    if ( currnode -> num_of_children == 1 )
    {
      if ( getFirstChild ( currnode ) -> node_type == AST_LITERAL_NODE )
      {
        LITERAL *litdata = & ( getEntryByIndex ( symboltable,
          getFirstChild ( currnode ) -> extra_data . symboltable_index ) -> data . lit_data );

        TNODE *foundlit = findString ( literaltrie, litdata -> value );

        currnode -> offsetcount = DATA_IN_REG;
        currnode -> offsetreg = getRegister ( codefile, symboltable,
            getFirstChild ( currnode ) -> extra_data . symboltable_index,
            OFFSET_ANY, OFFSET_ANY, OFFSET_ANY, NO_SPECIFIC_REG, 1, NO_REGISTER, NO_REGISTER );

        if ( currnode -> offsetreg == -1 )
        {
          fprintf ( stderr, "Error: Out of temporary registers at EXP node\n" );
          exit ( -1 );
        }

        int gotreg = currnode -> offsetreg;
        setRegisterProperties ( gotreg, 0, -1, 1, 0, -1, OFFSET_ANY, OFFSET_ANY, OFFSET_ANY );

        if ( DEBUG_REGISTER_ALLOC )
          fprintf ( stderr, "%s's result got %s register\n", getNodeTypeName ( currnode -> node_type ),
                                                             getRegisterName ( currnode -> offsetreg ) );

        if ( litdata -> lit_type == D_STRING_TYPE )
          fprintf ( codefile, "\tmov\t%s, %s\n", getRegisterName ( currnode -> offsetreg ),
                                                   literals [ foundlit -> data . int_val ] . name );
        else
          fprintf ( codefile, "\tmov\t%s, [%s]\n", getRegisterName ( currnode -> offsetreg ),
                                                   literals [ foundlit -> data . int_val ] . name );

      }
      else
      {
        // is assign or func node
        currnode -> offsetcount = DATA_IN_REG;
        currnode -> offsetreg = getFirstChild ( currnode ) -> offsetreg;
      }
    }
    else if ( currnode -> num_of_children == 2 )
    {
      int leftreg = -1, rightreg = -1;
      int resultreg = -1;
      int islit1 = 0;
      int islitleft = 0;

      ANODE *firstchild = getFirstChild ( currnode );
      ANODE *secondchild = getSecondChild ( currnode );
      AROPTYPE op = currnode -> extra_data . arop_type;

      if ( firstchild -> node_type == AST_LITERAL_NODE || secondchild -> node_type == AST_LITERAL_NODE )
      {
        ANODE *litchild = (firstchild -> node_type == AST_LITERAL_NODE) ? firstchild : secondchild;
        islitleft = (firstchild -> node_type == AST_LITERAL_NODE) ? 1 : 0;
        char *litvalue = getEntryByIndex ( symboltable, litchild -> extra_data . symboltable_index ) -> data . lit_data . value;

        if ( strcmp ( litvalue, "1" ) == 0 && ( op == A_PLUS_TYPE || ( op == A_MINUS_TYPE && ! islitleft ) ) )
          islit1 = 1;
        else if ( islitleft )
          leftreg = getLiteralInRegister ( litchild, codefile, symboltable, literaltrie, literals );
        else
          rightreg = getLiteralInRegister ( litchild, codefile, symboltable, literaltrie, literals );

        if ( DEBUG_REGISTER_ALLOC )
          fprintf ( stderr, "%s's result got %s register\n", getNodeTypeName ( currnode -> node_type ),
                                                             getRegisterName ( islitleft ? leftreg : rightreg ) );
      }

      if ( firstchild -> node_type == AST_ASSIGNFUNC_NODE || secondchild -> node_type == AST_ASSIGNFUNC_NODE )
      {
        int isleft = (firstchild -> node_type == AST_ASSIGNFUNC_NODE) ? 1 : 0;

        if ( isleft )
          leftreg = firstchild -> offsetreg;
        else
          rightreg = secondchild -> offsetreg;
      }

      if ( firstchild -> node_type == AST_EXP_NODE || firstchild -> node_type == AST_AROP_NODE )
      {
        leftreg = getRegister ( outputfile, symboltable, -1, OFFSET_ANY, OFFSET_ANY, OFFSET_ANY,
                                NO_SPECIFIC_REG, 1, leftreg, rightreg );

        // Get the left data from the stack
        fprintf ( outputfile, "\tpop\t%s\n", getRegisterName ( leftreg ) );

        setRegisterProperties ( leftreg, 0, -1, 1, 0, -1, OFFSET_ANY, OFFSET_ANY, OFFSET_ANY );
      }

      if ( secondchild -> node_type == AST_AROP_NODE || secondchild -> node_type == AST_EXP_NODE )
        rightreg = secondchild -> offsetreg;

      int leftdone = 0;
      int gottemp = 0;

      if ( op == A_MINUS_TYPE )
      {
        gottemp = 1;
        resultreg = getRegister ( outputfile, symboltable, -1, OFFSET_ANY, OFFSET_ANY, OFFSET_ANY,
                                  NO_SPECIFIC_REG, 1, leftreg, rightreg );

        if ( resultreg == -1 )
        {
          fprintf ( stderr, "Error: Out of temporary registers while executing MINUS op\n" );
          exit ( -1 );
        }

        if ( islit1 )
        {
          if ( islitleft )
            fprintf ( outputfile, "\tmov\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( rightreg ) );
          else
            fprintf ( outputfile, "\tmov\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( leftreg ) );
        }
        else
          fprintf ( outputfile, "\tmov\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( leftreg ) );

        setRegisterProperties ( resultreg, 0, -1, 1, 0, -1, OFFSET_ANY, OFFSET_ANY, OFFSET_ANY );
      }



      if ( registers [ leftreg ] . istemp && ! gottemp && ! islit1 )
      {
        resultreg = leftreg;
        leftdone = 1;
      }
      else if ( registers [ rightreg ] . istemp && ! gottemp && ! islit1 )
      {
        resultreg = rightreg;
      }
      else if ( ! gottemp )
      {
        resultreg = getRegister ( outputfile, symboltable, -1, OFFSET_ANY, OFFSET_ANY, OFFSET_ANY,
                                  NO_SPECIFIC_REG, 1, leftreg, rightreg );

        if ( resultreg == -1 )
        {
          fprintf ( stderr, "Error: Out of registers while getting temp register\n" );
          exit ( -1 );
        }

        setRegisterProperties ( resultreg, 0, -1, 1, 0, -1, OFFSET_ANY, OFFSET_ANY, OFFSET_ANY );

        if ( islit1 )
        {
          if ( islitleft )
            fprintf ( outputfile, "\tmov\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( rightreg ) );
          else
            fprintf ( outputfile, "\tmov\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( leftreg ) );
        }
        else
          fprintf ( outputfile, "\tmov\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( leftreg ) );
        leftdone = 1;
      }

      if ( ! islit1 && islitleft != 1 )
        assert ( leftreg != -1 );
      else if ( ! islit1 )
        assert ( rightreg != -1 );

      // TODO: Check for the different data types
      if ( op == A_PLUS_TYPE )
      {
        if ( islit1 )
          fprintf ( outputfile, "\tinc\t%s\n", getRegisterName ( resultreg ) );
        else
          fprintf ( outputfile, "\tadd\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( leftdone ? rightreg : leftreg ) );
      }
      else if ( op == A_MINUS_TYPE )
      {
        if ( islit1 )
          fprintf ( outputfile, "\tdec\t%s\n", getRegisterName ( resultreg ) );
        else
          fprintf ( outputfile, "\tsub\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( rightreg ) );
      }
      else if ( op == A_MUL_TYPE )
      {
        if ( ! islit1 )
        {
          int regtomul = (leftdone ? rightreg : leftreg);

          // If the regs do not belong to these, then push and restore data later
          // Push only if necessary
          if ( resultreg != EAX_REG && regtomul != EAX_REG && ! registers [ EAX_REG ] . flushed )
            fprintf ( outputfile, "\tpush\teax\n" );
          if ( resultreg != EBX_REG && regtomul != EBX_REG && ! registers [ EBX_REG ] . flushed )
            fprintf ( outputfile, "\tpush\tebx\n" );
          if ( resultreg != EDX_REG && regtomul != EDX_REG && ! registers [ EDX_REG ] . flushed )
            fprintf ( outputfile, "\tpush\tedx\n" );

          fprintf ( outputfile, "\n\t; Begin multiply\n" );

          // Move to appropriate registers only if necessary
          if ( ( regtomul != EBX_REG || resultreg != EAX_REG ) && ( regtomul != EAX_REG || resultreg != EBX_REG ) )
          {
            fprintf ( outputfile, "\tpush\t%s\n", getRegisterName ( regtomul ) );
            fprintf ( outputfile, "\tpush\t%s\n", getRegisterName ( resultreg ) );
            fprintf ( outputfile, "\tpop\teax\n" );
            fprintf ( outputfile, "\tpop\tebx\n" );
          }
          fprintf ( outputfile, "\timul\tebx\n" );

          if ( resultreg != EAX_REG )
            fprintf ( outputfile, "\tmov\t%s, eax\n", getRegisterName ( resultreg ) );

          fprintf ( outputfile, "\t; End multiply\n\n" );

          // Restore the registers if they belonged to some other code
          if ( resultreg != EDX_REG && regtomul != EDX_REG && ! registers [ EDX_REG ] . flushed )
            fprintf ( outputfile, "\tpop\tedx\n" );
          if ( resultreg != EBX_REG && regtomul != EBX_REG && ! registers [ EBX_REG ] . flushed )
            fprintf ( outputfile, "\tpop\tebx\n" );
          if ( resultreg != EAX_REG && regtomul != EAX_REG && ! registers [ EAX_REG ] . flushed )
            fprintf ( outputfile, "\tpop\teax\n" );
        }
      }
      else if ( op == A_DIV_TYPE || op == A_MODULO_TYPE )
      {
        int regtodiv = (leftdone ? rightreg : leftreg);

        // If the regs do not belong to these, then push and restore data later
        if ( resultreg != EAX_REG && regtodiv != EAX_REG && ! registers [ EAX_REG ] . flushed )
          fprintf ( outputfile, "\tpush\teax\n" );
        if ( resultreg != EBX_REG && regtodiv != EBX_REG && ! registers [ EBX_REG ] . flushed )
          fprintf ( outputfile, "\tpush\tebx\n" );
        if ( resultreg != EDX_REG && regtodiv != EDX_REG && ! registers [ EDX_REG ] . flushed )
          fprintf ( outputfile, "\tpush\tedx\n" );

        fprintf ( outputfile, "\n\t; Begin division/modulo\n" );

        // Move to appropriate registers only if necessary
        if ( regtodiv != EBX_REG || resultreg != EAX_REG )
        {
          fprintf ( outputfile, "\tpush\t%s\n", getRegisterName ( regtodiv ) );
          fprintf ( outputfile, "\tpush\t%s\n", getRegisterName ( resultreg ) );
          fprintf ( outputfile, "\tpop\teax\n" );
          fprintf ( outputfile, "\tpop\tebx\n" );
        }
        fprintf ( outputfile, "\tmov\tedx, 0\n" );
        fprintf ( outputfile, "\tidiv\tebx\n" );

        if ( op == A_DIV_TYPE && resultreg != EAX_REG )
          fprintf ( outputfile, "\tmov\t%s, eax\n", getRegisterName ( resultreg ) );
        else if ( resultreg != EDX_REG )
          fprintf ( outputfile, "\tmov\t%s, edx\n", getRegisterName ( resultreg ) );

        fprintf ( outputfile, "\t; End division/modulo\n\n" );

        // Restore the registers if they belonged to some other code
        if ( resultreg != EDX_REG && regtodiv != EDX_REG && ! registers [ EDX_REG ] . flushed )
          fprintf ( outputfile, "\tpop\tedx\n" );
        if ( resultreg != EBX_REG && regtodiv != EBX_REG && ! registers [ EBX_REG ] . flushed )
          fprintf ( outputfile, "\tpop\tebx\n" );
        if ( resultreg != EAX_REG && regtodiv != EAX_REG && ! registers [ EAX_REG ] . flushed )
          fprintf ( outputfile, "\tpop\teax\n" );
      }

      currnode -> offsetcount = DATA_IN_REG;
      currnode -> offsetreg = resultreg;
      registers [ resultreg ] . istemp = 1;

      if ( leftreg != resultreg && registers [ leftreg ] . istemp )
        registers [ leftreg ] . flushed = 1;
      if ( rightreg != resultreg && registers [ rightreg ] . istemp )
        registers [ rightreg ] . flushed = 1;

      // Ugly hack to determine if currnode is the first child
      if ( getFirstChild ( getParent ( currnode ) ) == currnode
           && ( getParent ( currnode ) -> node_type == AST_EXP_NODE || getParent ( currnode ) -> node_type == AST_AROP_NODE )
           && getParent ( currnode ) -> num_of_children > 1 )
      {
        fprintf ( outputfile, "\tpush\t%s\n", getRegisterName ( resultreg ) );
        registers [ resultreg ] . flushed = 1;
      }
    }
  }
  else if ( currnode -> node_type == AST_AROP_NODE )
  {
    if ( currnode -> num_of_children == 1 )
    {
      ANODE *child = getFirstChild ( currnode );
      if ( child -> node_type == AST_LITERAL_NODE )
      {
        currnode -> offsetcount = DATA_IN_REG;
        currnode -> offsetreg = getLiteralInRegister ( child, outputfile, symboltable, literaltrie, literals );
      }
      else if ( child -> node_type == AST_ASSIGNFUNC_NODE || child -> node_type == AST_EXP_NODE )
      {
        currnode -> offsetcount = DATA_IN_REG;
        currnode -> offsetreg = child -> offsetreg;
      }
    }
    else if ( currnode -> num_of_children == 2 )
    {
      int leftreg = -1, rightreg = -1;
      int resultreg = -1;
      int islitleft = 0;
      int islit1 = 0;

      ANODE *firstchild = getFirstChild ( currnode );
      ANODE *secondchild = getSecondChild ( currnode );
      AROPTYPE op = currnode -> extra_data . arop_type;

      if ( firstchild -> node_type == AST_LITERAL_NODE || secondchild -> node_type == AST_LITERAL_NODE )
      {
        ANODE *litchild = (firstchild -> node_type == AST_LITERAL_NODE) ? firstchild : secondchild;
        islitleft = (firstchild -> node_type == AST_LITERAL_NODE) ? 1 : 0;
        char *litvalue = getEntryByIndex ( symboltable, litchild -> extra_data . symboltable_index ) -> data . lit_data . value;

        if ( strcmp ( litvalue, "1" ) == 0 && ( op == A_PLUS_TYPE || ( op == A_MINUS_TYPE && ! islitleft ) ) )
          islit1 = 1;
        if ( islitleft )
          leftreg = getLiteralInRegister ( litchild, codefile, symboltable, literaltrie, literals );
        else
          rightreg = getLiteralInRegister ( litchild, codefile, symboltable, literaltrie, literals );

        if ( DEBUG_REGISTER_ALLOC )
          fprintf ( stderr, "%s's result got %s register\n", getNodeTypeName ( currnode -> node_type ),
                                                             getRegisterName ( islitleft ? leftreg : rightreg ) );
      }

      if ( firstchild -> node_type == AST_ASSIGNFUNC_NODE || secondchild -> node_type == AST_ASSIGNFUNC_NODE )
      {
        int isleft = (firstchild -> node_type == AST_ASSIGNFUNC_NODE) ? 1 : 0;

        if ( isleft )
          leftreg = firstchild -> offsetreg;
        else
          rightreg = secondchild -> offsetreg;
      }

      if ( firstchild -> node_type == AST_EXP_NODE || firstchild -> node_type == AST_AROP_NODE )
      {
        leftreg = getRegister ( outputfile, symboltable, -1, OFFSET_ANY, OFFSET_ANY, OFFSET_ANY,
                                NO_SPECIFIC_REG, 1, leftreg, rightreg );

        // Get the left data from the stack
        fprintf ( outputfile, "\tpop\t%s\n", getRegisterName ( leftreg ) );

        setRegisterProperties ( leftreg, 0, -1, 1, 0, -1, OFFSET_ANY, OFFSET_ANY, OFFSET_ANY );
      }

      if ( secondchild -> node_type == AST_AROP_NODE || secondchild -> node_type == AST_EXP_NODE )
        rightreg = secondchild -> offsetreg;

      int leftdone = 0;
      int gottemp = 0;

      if ( ! islit1 && ! islitleft )
        assert ( leftreg != -1 );
      else if ( ! islit1 )
        assert ( rightreg != -1 );

      if ( op == A_MINUS_TYPE )
      {
        gottemp = 1;
        resultreg = getRegister ( outputfile, symboltable, -1, OFFSET_ANY, OFFSET_ANY, OFFSET_ANY,
                                  NO_SPECIFIC_REG, 1, leftreg, rightreg );

        if ( resultreg == -1 )
        {
          fprintf ( stderr, "Error: Out of temporary registers while executing MINUS op\n" );
          exit ( -1 );
        }

        if ( islit1 )
        {
          if ( islitleft )
            fprintf ( outputfile, "\tmov\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( rightreg ) );
          else
            fprintf ( outputfile, "\tmov\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( leftreg ) );
        }
        else
          fprintf ( outputfile, "\tmov\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( leftreg ) );

        setRegisterProperties ( resultreg, 0, -1, 1, 0, -1, OFFSET_ANY, OFFSET_ANY, OFFSET_ANY );
      }

      if ( registers [ leftreg ] . istemp && ! gottemp  && ! islit1 )
      {
        resultreg = leftreg;
        leftdone = 1;
      }
      else if ( registers [ rightreg ] . istemp && ! gottemp && ! islit1 )
      {
        resultreg = rightreg;
      }
      else
      {
        resultreg = getRegister ( outputfile, symboltable, -1, OFFSET_ANY, OFFSET_ANY, OFFSET_ANY,
                                  NO_SPECIFIC_REG, 1, leftreg, rightreg );

        if ( resultreg == -1 )
        {
          fprintf ( stderr, "Error: Out of registers while getting temporary register\n" );
          exit ( -1 );
        }

        setRegisterProperties ( resultreg, 0, -1, 1, 0, -1, OFFSET_ANY, OFFSET_ANY, OFFSET_ANY );

        if ( islit1 )
        {
          if ( islitleft )
            fprintf ( outputfile, "\tmov\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( rightreg ) );
          else
            fprintf ( outputfile, "\tmov\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( leftreg ) );
        }
        else
          fprintf ( outputfile, "\tmov\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( leftreg ) );
        leftdone = 1;
      }

      // TODO: Check for the different data types
      if ( op == A_PLUS_TYPE )
      {
        if ( islit1 )
          fprintf ( outputfile, "\tinc\t%s\n", getRegisterName ( resultreg ) );
        else
          fprintf ( outputfile, "\tadd\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( leftdone ? rightreg : leftreg ) );
      }
      else if ( op == A_MINUS_TYPE )
      {
        if ( islit1 )
          fprintf ( outputfile, "\tdec\t%s\n", getRegisterName ( resultreg ) );
        else
          fprintf ( outputfile, "\tsub\t%s, %s\n", getRegisterName ( resultreg ), getRegisterName ( rightreg ) );
      }
      else if ( op == A_MUL_TYPE )
      {
        if ( ! islit1 )
        {
          int regtomul = (leftdone ? rightreg : leftreg);

          // If the regs do not belong to these, then push and restore data later
          if ( resultreg != EAX_REG && regtomul != EAX_REG && ! registers [ EAX_REG ] . flushed )
            fprintf ( outputfile, "\tpush\teax\n" );
          if ( resultreg != EBX_REG && regtomul != EBX_REG && ! registers [ EBX_REG ] . flushed )
            fprintf ( outputfile, "\tpush\tebx\n" );
          if ( resultreg != EDX_REG && regtomul != EDX_REG && ! registers [ EDX_REG ] . flushed )
            fprintf ( outputfile, "\tpush\tedx\n" );

          fprintf ( outputfile, "\n\t; Begin Multiply\n" );

          if ( ( regtomul != EBX_REG || resultreg != EAX_REG ) && ( regtomul != EAX_REG || resultreg != EBX_REG ) )
          {
            fprintf ( outputfile, "\tpush\t%s\n", getRegisterName ( regtomul ) );
            fprintf ( outputfile, "\tpush\t%s\n", getRegisterName ( resultreg ) );
            fprintf ( outputfile, "\tpop\teax\n" );
            fprintf ( outputfile, "\tpop\tebx\n" );
          }
          fprintf ( outputfile, "\timul\tebx\n" );

          if ( resultreg != EAX_REG )
            fprintf ( outputfile, "\tmov\t%s, eax\n", getRegisterName ( resultreg ) );

          fprintf ( outputfile, "\t; End Multiply\n\n" );

          // Restore the registers if they belonged to some other code
          if ( resultreg != EDX_REG && regtomul != EDX_REG && ! registers [ EDX_REG ] . flushed )
            fprintf ( outputfile, "\tpop\tedx\n" );
          if ( resultreg != EBX_REG && regtomul != EBX_REG && ! registers [ EBX_REG ] . flushed )
            fprintf ( outputfile, "\tpop\tebx\n" );
          if ( resultreg != EAX_REG && regtomul != EAX_REG && ! registers [ EAX_REG ] . flushed )
            fprintf ( outputfile, "\tpop\teax\n" );
        }
      }
      else if ( op == A_DIV_TYPE || op == A_MODULO_TYPE )
      {
        int regtodiv = (leftdone ? rightreg : leftreg);

        // If the regs do not belong to these, then push and restore data later
        if ( resultreg != EAX_REG && regtodiv != EAX_REG && ! registers [ EAX_REG ] . flushed )
          fprintf ( outputfile, "\tpush\teax\n" );
        if ( resultreg != EBX_REG && regtodiv != EBX_REG && ! registers [ EBX_REG ] . flushed )
          fprintf ( outputfile, "\tpush\tebx\n" );
        if ( resultreg != EDX_REG && regtodiv != EDX_REG && ! registers [ EDX_REG ] . flushed )
          fprintf ( outputfile, "\tpush\tedx\n" );

        fprintf ( outputfile, "\n\t; Begin division/modulo\n" );

        if ( regtodiv != EBX_REG || resultreg != EAX_REG )
        {
          fprintf ( outputfile, "\tpush\t%s\n", getRegisterName ( regtodiv ) );
          fprintf ( outputfile, "\tpush\t%s\n", getRegisterName ( resultreg ) );
          fprintf ( outputfile, "\tpop\teax\n" );
          fprintf ( outputfile, "\tpop\tebx\n" );
        }
        fprintf ( outputfile, "\tmov\tedx, 0\n" );
        fprintf ( outputfile, "\tidiv\tebx\n" );

        if ( op == A_DIV_TYPE && resultreg != EAX_REG )
          fprintf ( outputfile, "\tmov\t%s, eax\n", getRegisterName ( resultreg ) );
        else if ( resultreg != EDX_REG )
          fprintf ( outputfile, "\tmov\t%s, edx\n", getRegisterName ( resultreg ) );

        fprintf ( outputfile, "\t; End division/modulo\n\n" );

        // Restore the registers if they belonged to some other code
        if ( resultreg != EDX_REG && regtodiv != EDX_REG && ! registers [ EDX_REG ] . flushed )
          fprintf ( outputfile, "\tpop\tedx\n" );
        if ( resultreg != EBX_REG && regtodiv != EBX_REG && ! registers [ EBX_REG ] . flushed )
          fprintf ( outputfile, "\tpop\tebx\n" );
        if ( resultreg != EAX_REG && regtodiv != EAX_REG && ! registers [ EAX_REG ] . flushed )
          fprintf ( outputfile, "\tpop\teax\n" );
      }

      currnode -> offsetcount = DATA_IN_REG;
      currnode -> offsetreg = resultreg;
      registers [ resultreg ] . istemp = 1;

      if ( leftreg != resultreg && registers [ leftreg ] . istemp )
        registers [ leftreg ] . flushed = 1;
      if ( rightreg != resultreg && registers [ rightreg ] . istemp )
        registers [ rightreg ] . flushed = 1;

      if ( getFirstChild ( getParent ( currnode ) ) == currnode
           && ( getParent ( currnode ) -> node_type == AST_EXP_NODE || getParent ( currnode ) -> node_type == AST_AROP_NODE )
           && getParent ( currnode ) -> num_of_children > 1 )
      {
        fprintf ( outputfile, "\tpush\t%s\n", getRegisterName ( resultreg ) );
        registers [ resultreg ] . flushed = 1;
      }
    }
  }
  else if ( currnode -> node_type == AST_ASSIGNFUNC_NODE )
  {
    // TODO: Deal with the function call case
    if ( currnode -> num_of_children == 1 )
    {
      currnode -> offsetcount = getFirstChild ( currnode ) -> offsetcount;
      currnode -> offset1 = getFirstChild ( currnode ) -> offset1;
      currnode -> global_or_local = getFirstChild ( currnode ) -> global_or_local;
    }
    else if ( currnode -> num_of_children == 3 )
    {
      currnode -> global_or_local = getFirstChild ( currnode ) -> global_or_local;
      currnode -> offset1 = getFirstChild ( currnode ) -> offset1;

      currnode -> offsetcount = 1 + getThirdChild ( currnode ) -> offsetcount;

      currnode -> offset2 = getThirdChild ( currnode ) -> offset1;
      currnode -> offset3 = getThirdChild ( currnode ) -> offset2;
    }

    // TODO: Deal with function calls separately
    int gotreg = getOffsetInReg ( currnode, outputfile, symboltable );

    // Get the data from the offset if the result is an INT or FLOAT only
    if ( registers [ gotreg ] . hasoffset && ( currnode -> result_type == D_INT_TYPE || currnode -> result_type == D_FLOAT_TYPE ) )
    {
      fprintf ( outputfile, "\tmov\t%s, [%s]\n", getRegisterName ( gotreg ), getRegisterName ( gotreg ) );
      registers [ gotreg ] . hasoffset = 0;
    }

    currnode -> offsetcount = DATA_IN_REG;
    currnode -> offsetreg = gotreg;
  }
  else if ( currnode -> node_type == AST_IF_NODE )
  {
    // Write a label to go to if if has only one child

    if ( currnode -> num_of_children == 2 )
      fprintf ( outputfile, "\niflabel%d_1:\n", currnode -> extra_data . iflabel );
    else if ( currnode -> num_of_children > 2 )
      fprintf ( outputfile, "\niflabel%d_2:\n", currnode -> extra_data . iflabel );
  }
  else if ( currnode -> node_type == AST_COMPARE_NODE )
  {
    // TODO: Handle boolean expressions
    int reg1 = getFirstChild ( currnode ) -> offsetreg;
    int reg2 = getSecondChild ( currnode ) -> offsetreg;
    int label = getParent ( getParent ( currnode ) ) -> extra_data . iflabel;

    fprintf ( codefile, "\tcmp\t%s, %s\n", getRegisterName ( reg1 ), getRegisterName ( reg2 ) );
    if ( currnode -> extra_data . compop_type == C_EQ_TYPE )
      fprintf ( codefile, "\tjne\tiflabel%d_1\n\n", label );
    else if ( currnode -> extra_data . compop_type == C_GT_TYPE )
      fprintf ( codefile, "\tjle\tiflabel%d_1\n\n", label );
    else if ( currnode -> extra_data . compop_type == C_GTE_TYPE )
      fprintf ( codefile, "\tjl\tiflabel%d_1\n\n", label );
    else if ( currnode -> extra_data . compop_type == C_LTE_TYPE )
      fprintf ( codefile, "\tjg\tiflabel%d_1\n\n", label );
    else if ( currnode -> extra_data . compop_type == C_LT_TYPE )
      fprintf ( codefile, "\tjge\tiflabel%d_1\n\n", label );

    registers [ reg1 ] . flushed = 1;
    registers [ reg2 ] . flushed = 1;
  }
  else if ( currnode -> node_type == AST_FOR_NODE )
  {
    fprintf ( outputfile, "\tdec\tecx\n" );
    fprintf ( outputfile, "\tcmp\tecx, 0\n" );
    fprintf ( outputfile, "\tjle\tlooplabel%d_2\n\n", currnode -> extra_data . forlabel );
    fprintf ( outputfile, "\tjmp\tlooplabel%d_1\n\n", currnode -> extra_data . forlabel );
    fprintf ( outputfile, "looplabel%d_2:\n", currnode -> extra_data . forlabel );
    fprintf ( outputfile, "\tpop\tecx\n" );
    registers [ ECX_REG ] . flushed = 0;
    registers [ ECX_REG ] . istemp = 1;
  }
}

void topDownCodeGeneration ( ANODE *currnode, FILE *codefile, SYMBOLTABLE *symboltable,
                             TRIE *literaltrie, LITDATA *literals )
{
  // Function used primarily to reserve stack space
  if ( currnode -> node_type == AST_PROGRAM_NODE )
  {
    int toReserve = getProgramSize ( currnode ) * 4;
    fprintf ( codefile, "\tpush\tebp\n" );
    fprintf ( codefile, "\tmov\tebp, esp\n" );
    fprintf ( codefile, "\n\tsub\tesp, %d\t\t;Reserve %d stack space\n\n", toReserve, toReserve );
  }
  else if ( currnode -> node_type == AST_IF_NODE )
  {
    if ( currnode -> extra_data . compop_type > 1 )
      fprintf ( codefile, "\niflabel%d_2:\n", getParent ( currnode ) -> extra_data . iflabel );
    // TODO: Deal with cascaded if case
    currnode -> extra_data . iflabel = iflabel++;
    getSecondChild ( currnode ) -> extra_data . compop_type = 1;
    if ( currnode -> num_of_children == 3 )
      getThirdChild ( currnode ) -> extra_data . compop_type = 2;
  }
  else if ( currnode -> node_type == AST_BLOCK_NODE )
  {
    if ( currnode -> extra_data . compop_type > 1 )
    {
      int label = getParent ( currnode ) -> extra_data . iflabel;
      fprintf ( codefile, "\tjmp\tiflabel%d_2\n\n", label );
      fprintf ( codefile, "\niflabel%d_1:\n", label );
    }
    else if ( getParent ( currnode ) -> node_type == AST_FOR_NODE )
    {
      int i;
      for ( i = 0; i < NUMREG; i++ )
      {
        registers [i] . flushed = 1;
        registers [i] . istemp = 0;
      }

      registers [ ECX_REG ] . flushed = 1;
      ANODE *idennode = getFirstChild ( getParent ( currnode ) );
      STBENTRY *entry = getEntryByIndex ( symboltable, idennode -> extra_data . symboltable_index );
      int reg = getRegister ( codefile, symboltable, entry -> index, idennode -> offset1, OFFSET_ANY, OFFSET_ANY,
                              ECX_REG, 1, 0, 0 );

      setRegisterProperties ( reg, 0, -1, 1, 0, -1, OFFSET_ANY, OFFSET_ANY, OFFSET_ANY );

      fprintf ( codefile, "\tpush\tecx\n" );
      if ( entry -> entry_type == ENTRY_LIT_TYPE )
      {
        TNODE *foundlit = findString ( literaltrie, entry -> data . lit_data . value );
        fprintf ( codefile, "\tmov\tecx, [%s]\n", literals [ foundlit -> data . int_val ] . name );
      }
      else if ( idennode -> global_or_local == IS_GLOBAL )
        fprintf ( codefile, "\tmov\tecx, [%s]\n", entry -> data . var_data . name );
      else
        fprintf ( codefile, "\tmov\tecx, [ebp-%d]\n", idennode -> offset1 );

      fprintf ( codefile, "\nlooplabel%d_1:\n", getParent ( currnode ) -> extra_data . forlabel );
    }
  }
  else if ( currnode -> node_type == AST_FOR_NODE )
  {
    currnode -> extra_data . forlabel = forlabel++;
  }
}

void checkAndGenerateCode ( AST *ast, SYMBOLTABLE *symboltable, FILE *stbdumpfile,
                            FILE *assemblyfile, FILE *codefile, FILE *functionfile,
                            TRIE *literaltrie, LITDATA *literals, FILE *datafile )
{
  // The function traverses the AST first top down then bottom with the aid of the
  // STACKENTRY structure

  symboltable = openEnv ( symboltable );

  ANODE *programNode = * ( ANODE ** ) ( ast -> root -> children -> head -> data . generic_val );

  STACK *stack = getStack ( STACK_GENERIC_TYPE );

  stack = setStackGenericSize ( stack, sizeof ( STACKENTRY ) );

  STACKENTRY temp;
  temp . node = programNode;
  temp . upordown = DOWN;

  int loopcount = 0;
  int infunction = 0;
  int bdftcount = 0;

  stack = push ( stack, & temp );

  while ( ! isEmpty ( stack ) )
  {
    ANODE *currnode = ( ( STACKENTRY * ) top ( stack ) ) -> node;
    int updown = ( ( STACKENTRY * ) top ( stack ) ) -> upordown;

    pop ( stack );

    if ( updown == DOWN )
    {
      if ( currnode -> node_type == AST_FUNCTION_NODE )
        infunction = 1;

      if ( currnode -> node_type == AST_FOR_NODE )
        loopcount ++;

      if ( currnode -> node_type == AST_BDFT_NODE )
        bdftcount ++;

      if ( currnode -> node_type == AST_QUALIFIEDPARAMETERS_NODE )
        symboltable = openEnv ( symboltable );
      else if ( currnode -> node_type == AST_BLOCK_NODE )
        symboltable = openEnv ( symboltable );

      temp . node = currnode;
      temp . upordown = UP;

      stack = push ( stack, & temp );

      handleTypeSpecificActions ( currnode, symboltable, stbdumpfile );

      LNODE iterator;

      getReverseIterator ( currnode -> children, & iterator );

      while ( hasPrevious ( & iterator ) )
      {
        getPrevious ( currnode -> children, & iterator );

        ANODE *child = * ( ANODE ** ) ( iterator . data . generic_val );

        temp . node = child;
        temp . upordown = DOWN;

        stack = push ( stack, & temp );
      }

      topDownCodeGeneration ( currnode, codefile, symboltable, literaltrie, literals );

      if ( DEBUG_AST_CONSTRUCTION )
      {
        fprintf ( stderr, "Analyzing node %s on the way down\n", getNodeTypeName ( currnode -> node_type ) );
        if ( currnode -> node_type == AST_EXP_NODE || currnode -> node_type == AST_AROP_NODE )
          fprintf ( stderr, "With operation %s\n", getAropName ( currnode -> extra_data . arop_type ) );
        if ( currnode -> node_type == AST_LITERAL_NODE )
          fprintf ( stderr, "With value %s\n", getEntryByIndex ( symboltable, currnode -> extra_data . symboltable_index )
                                               -> data . lit_data . value );
      }
    }
    else
    {
      // Now in the bottom-up traversal portion
      if ( currnode -> node_type == AST_FUNCTION_NODE )
        infunction = 0;

      if ( currnode -> node_type == AST_FOR_NODE )
      {
        loopcount --;
        if ( currnode -> num_of_children >= 3 && getThirdChild ( currnode ) -> node_type == AST_BDFT_NODE )
          bdftcount --;
      }

      if ( currnode -> node_type == AST_BLOCK_NODE || currnode -> node_type == AST_FUNCBODY_NODE )
        symboltable = closeEnv ( symboltable );

      performSemanticChecks ( currnode, symboltable, & infunction, & loopcount, & bdftcount );

      generateCode ( currnode, symboltable, assemblyfile, codefile, functionfile,
                     literaltrie, literals, datafile, infunction );

      if ( DEBUG_AST_CONSTRUCTION )
        fprintf ( stderr, "Analyzing node %s on the way up\n", getNodeTypeName ( currnode -> node_type ) );
    }
  }
}

void writeReturnZero ( FILE *codefile )
{
  fprintf ( codefile, "\n\tmov esp, ebp\n" );
  fprintf ( codefile, "\tpop ebp\n" );
  fprintf ( codefile, "\n\tmov eax, 1\n" );
  fprintf ( codefile, "\tmov ebx, 0\n" );
  fprintf ( codefile, "\tint 80h\n\n" );
}

void joinCodeFiles ( FILE *assemblyfile )
{
  FILE *codefile = NULL;
  codefile = fopen ( ASSEMBLY_CODE_FILE, "r" );

  if ( codefile == NULL )
  {
    fprintf ( stderr, "Failed to open assembly code file\n" );
    erroroccured = 1;
    return;
  }

  FILE *functionfile = NULL;
  functionfile = fopen ( ASSEMBLY_FUNCTIONS_FILE, "r" );

  if ( functionfile == NULL )
  {
    fprintf ( stderr, "Failed to open assembly functions file\n" );
    erroroccured = 1;
    return;
  }

  FILE *varfile = NULL;
  varfile = fopen ( ASSEMBLY_FILE, "r" );

  if ( varfile == NULL )
  {
    fprintf ( stderr, "Failed to open variables file\n" );
    erroroccured = 1;
    return;
  }

  char c;
  while ( fscanf ( varfile, "%c", &c ) > 0 )
    fprintf ( assemblyfile, "%c", c );

  fprintf ( assemblyfile, "\n" );

  while ( fscanf ( functionfile, "%c", &c ) > 0 )
    fprintf ( assemblyfile, "%c", c );

  fprintf ( assemblyfile, "\n" );

  while ( fscanf ( codefile, "%c", &c ) > 0 )
    fprintf ( assemblyfile, "%c", c );

  fprintf ( assemblyfile, "\n" );

  if ( fclose ( codefile ) != 0 )
    fprintf ( stderr, "Failed to close code file\n" );

  if ( fclose ( functionfile ) != 0 )
    fprintf ( stderr, "Failed to close functions file\n" );

  if ( fclose ( varfile ) != 0 )
    fprintf ( stderr, "Failed to close functions file\n" );
}

int main ( )
{
  // Get the system block size
  struct stat fi;
  stat ( "/", &fi );
  int blocksize = fi.st_blksize;

  /*********************************************************
    *                                                      *
    *              PHASE 1 : Populate Tries                *
    *                                                      *
    ********************************************************
   */

  AST* ast = NULL;
  ast = getNewAst ();

  TRIE *terminals = NULL, *nonterminals = NULL;
  int terminalscount = 0, nonterminalscount = 0;

  terminals = getNewTrie ( TRIE_INT_TYPE );
  nonterminals = getNewTrie ( TRIE_INT_TYPE );

  FILE *tmapfile = NULL, *ntmapfile = NULL;

  tmapfile = fopen ( T_INDEX_FILE, "rb" );
  ntmapfile = fopen ( NT_INDEX_FILE, "rb" );

  if ( tmapfile == NULL || ntmapfile == NULL )
  {
    fprintf ( stderr, "Failed to open (non) terminals index file\n" );
    return -1;
  }

  populateTrie ( tmapfile, blocksize, terminals, &terminalscount );
  populateTrie ( ntmapfile, blocksize, nonterminals, &nonterminalscount );

  if ( fclose ( tmapfile ) != 0 )
    fprintf ( stderr, "Failed to close terminals index file\n" );
  if ( fclose ( ntmapfile ) != 0 )
    fprintf ( stderr, "Failed to close non terminals index file\n" );



  /*********************************************************
    *                                                      *
    *          PHASE 2 : Read AST from dump file           *
    *                                                      *
    ********************************************************
   */

  setAstNodeName ( ast -> root, ROOTNODENAME );

  FILE *astdumpfile = NULL;
  astdumpfile = fopen ( AST_DUMP_FILE, "r" );

  if ( astdumpfile == NULL )
  {
    fprintf ( stderr, "Failed to open AST dump file to read\n" );
    return -1;
  }

  readAstDumpFile ( ast -> root, astdumpfile );

  if ( fclose ( astdumpfile ) != 0 )
    fprintf ( stderr, "Failed to close AST dump file after reading\n" );




  /*********************************************************
    *                                                      *
    *    PHASE 3 : Parse AST and populate Symbol Table     *
    *                                                      *
    ********************************************************
   */

  int i;
  for ( i = 0; i < NUMREG; i++ )
    registers [i] . flushed = 1;

  FILE *stbdumpfile = NULL;
  stbdumpfile = fopen ( STB_DUMP_FILE, "r" );

  if ( stbdumpfile == NULL )
  {
    fprintf ( stderr, "Failed to open Symbol Table dump file\n" );
    return -1;
  }

  FILE *assemblyfile = NULL;
  assemblyfile = fopen ( ASSEMBLY_FILE, "w+" );

  if ( assemblyfile == NULL )
  {
    fprintf ( stderr, "Failed to open assembly file to write\n" );
    return -1;
  }

  FILE *codefile = NULL;
  codefile = fopen ( ASSEMBLY_CODE_FILE, "w+" );

  if ( codefile == NULL )
  {
    fprintf ( stderr, "Failed to open assemly code file to write\n" );
    return -1;
  }

  FILE *functionfile = NULL;
  functionfile = fopen ( ASSEMBLY_FUNCTIONS_FILE, "w+" );

  if ( functionfile == NULL )
  {
    fprintf ( stderr, "Failed to open assembly functions file\n" );
    return -1;
  }

  FILE *datafile = NULL;
  datafile = fopen ( ASSEMBLY_DATA_FILE, "w+" );

  if ( datafile == NULL )
  {
    fprintf ( stderr, "Failed to open assembly data file\n" );
    return -1;
  }

  unsigned int num_entries = 0;

  fscanf ( stbdumpfile, "%u", & num_entries );

  SYMBOLTABLE *symboltable = getSymbolTable ();

  symboltable = setNumEntries ( symboltable, num_entries );

  LITDATA literals [ num_entries ];
  TRIE *literaltrie = NULL;
  literaltrie = getNewTrie ( TRIE_INT_TYPE );

  layoutTemplate ( assemblyfile, codefile, datafile );

  checkAndGenerateCode ( ast, symboltable, stbdumpfile, assemblyfile, codefile, functionfile,
                         literaltrie, literals, datafile );

  if ( startwritten == 0 )
    fprintf ( assemblyfile, "\nsection .text\n\tglobal _start\n\n" );

  writeReturnZero ( codefile );

  if ( fclose ( codefile ) != 0 )
    fprintf ( stderr, "Failed to close code file\n" );

  if ( fclose ( functionfile ) != 0 )
    fprintf ( stderr, "Failed to close function file\n" );

  if ( fclose ( assemblyfile ) != 0 )
    fprintf ( stderr, "Failed to close assembly file\n" );

  joinCodeFiles ( datafile );

  if ( fclose ( datafile ) != 0 )
    fprintf ( stderr, "Failed to close assembly file\n" );

  if ( fclose ( stbdumpfile ) != 0 )
    fprintf ( stderr, "Failed to close Symbol Table dump file\n" );

  if ( erroroccured == 1 )
    return -1;

  return 0;
}

