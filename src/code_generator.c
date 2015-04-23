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
#include <sys/stat.h>
#include "headers/ast.h"
#include "headers/symboltable.h"

#ifndef TRIE_DEFINED
  #include "headers/trie.h"
#endif

#define DEBUG_AST_CONSTRUCTION 0
#define DEBUG_STB_AUXOPS 1

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

// Required language-specific tokens for semantic analysis
#define TK_BEGIN "TK_BEGIN"
#define TK_END "TK_END"
#define TK_IDEN "TK_IDEN"
#define TK_INTLIT "TK_INTLIT"
#define TK_STRINGLIT "TK_STRINGLIT"
#define TK_FLOATLIT "TK_FLOATLIT"
#define TK_INT "TK_INT"
#define TK_STRING "TK_STRING"
#define TK_FLOAT "TK_FLOAT"
#define TK_VERTEX "TK_VERTEX"
#define TK_EDGE "TK_EDGE"
#define TK_GRAPH "TK_GRAPH"
#define TK_TREE "TK_TREE"
#define TK_NOTHING "TK_NOTHING"
#define TK_AND "TK_AND"
#define TK_OR "TK_OR"
#define TK_NOT "TK_NOT"
#define TK_PLUS "TK_PLUS"
#define TK_MINUS "TK_MINUS"
#define TK_MUL "TK_MUL"
#define TK_DIV "TK_DIV"
#define TK_MODULO "TK_MODULO"
#define TK_GT "TK_GT"
#define TK_LT "TK_LT"
#define TK_GTE "TK_GTE"
#define TK_LTE "TK_LTE"
#define TK_EQ "TK_EQ"
#define TK_BFT "TK_BFT"
#define TK_DFT "TK_DFT"
#define TK_FUNCTION "TK_FUNCTION"
// End language-specific tokens

// A consistent mapping between these #defines and the AST_NODETYPES_FILE
// must be maintained.

#define AST_PROGRAM_NODE 0
#define AST_GLOBALDEFINES_NODE 1
#define AST_GLOBALDEFINE_NODE 2
#define AST_DEFINE_NODE 3
#define AST_LET_NODE 4
#define AST_ASSIGNABLE_NODE 5
#define AST_DATATYPE_NODE 6
#define AST_IDENTIFIER_NODE 7
#define AST_FUNCTION_NODE 8
#define AST_FUNCBODY_NODE 9
#define AST_QUALIFIEDPARAMETERS_NODE 10
#define AST_QUALIFIEDPARAMETER_NODE 11
#define AST_RETURNTYPE_NODE 12
#define AST_READ_NODE 13
#define AST_PRINT_NODE 14
#define AST_COMPARE_NODE 15
#define AST_BOOLEXP_NODE 16
#define AST_EXP_NODE 17
#define AST_PASSEDPARAMS_NODE 18
#define AST_RETURNSTMT_NODE 19
#define AST_FUNCTIONCALL_NODE 20
#define AST_IF_NODE 21
#define AST_BLOCK_NODE 22
#define AST_FOR_NODE 23
#define AST_BDFT_NODE 24
#define AST_EDGECREATE_NODE 25
#define AST_BOOLOP_NODE 26
#define AST_AROP_NODE 27
#define AST_FORIN_NODE 28
#define AST_ADJTO_NODE 29
#define AST_LITERAL_NODE 30
#define AST_DEPTH_NODE 31
#define AST_DEST_NODE 32
#define AST_SOURCE_NODE 33
#define AST_WEIGHT_NODE 34
#define AST_ASSIGNFUNC_NODE 35
#define AST_MEMBEROP_NODE 36
#define AST_ROOT_NODE 37
#define AST_ENDASSIGN_NODE 38
#define AST_BREAK_NODE 39

// End AST node types #defines

#define DOWN 0
#define UP 1

// The structure that is pushed on the stack to check whether this node is being
// poppsed on the way down or the way up, i.e. top-down traversal or bottom-up traversal
typedef struct stack_entry
{
  ANODE *node;
  int upordown;
} STACKENTRY;

int shouldintprint = 0;
int hasglobalvars = 0;
int hasfunctions = 0;
int startwritten = 0;
char *intoffset = "intoffset";
char *floatoffset = "floatoffset";
char *stringoffset = "stringoffset";
char *sourceoffset = "sourceoffset";
char *destoffset = "destoffset";
char *weightoffset = "weightoffset";
char *rootoffset = "rootoffset";
char *vlistoffset = "vlistoffset";
char *elistoffset = "elistoffset";

char nodeTypes[][30] = {

  "AST_PROGRAM_NODE",
  "AST_GLOBALDEFINES_NODE",
  "AST_GLOBALDEFINE_NODE",
  "AST_DEFINE_NODE",
  "AST_LET_NODE",
  "AST_ASSIGNABLE_NODE",
  "AST_DATATYPE_NODE",
  "AST_IDENTIFIER_NODE",
  "AST_FUNCTION_NODE",
  "AST_FUNCBODY_NODE",
  "AST_QUALIFIEDPARAMETERS_NODE",
  "AST_QUALIFIEDPARAMETER_NODE",
  "AST_RETURNTYPE_NODE",
  "AST_READ_NODE",
  "AST_PRINT_NODE",
  "AST_COMPARE_NODE",
  "AST_BOOLEXP_NODE",
  "AST_EXP_NODE",
  "AST_PASSEDPARAMS_NODE",
  "AST_RETURNSTMT_NODE",
  "AST_FUNCTIONCALL_NODE",
  "AST_IF_NODE",
  "AST_BLOCK_NODE",
  "AST_FOR_NODE",
  "AST_BDFT_NODE",
  "AST_EDGECREATE_NODE",
  "AST_BOOLOP_NODE",
  "AST_AROP_NODE",
  "AST_FORIN_NODE",
  "AST_ADJTO_NODE",
  "AST_LITERAL_NODE",
  "AST_DEPTH_NODE",
  "AST_DEST_NODE",
  "AST_SOURCE_NODE",
  "AST_WEIGHT_NODE",
  "AST_ASSIGNFUNC_NODE",
  "AST_MEMBEROP_NODE",
  "AST_ROOT_NODE",
  "AST_ENDASSIGN_NODE",
  "AST_BREAK_NODE",
  ""
};

char dataTypes[][10] = {

  "",
  "INT",
  "FLOAT",
  "STRING",
  "VERTEX",
  "EDGE",
  "TREE",
  "GRAPH",
  "NOTHING",
  ""
};

int curlitindex;

typedef struct literal_data
{
  char *name;
} LITDATA;

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

ANODE* getFirstChild ( ANODE *node )
{
  if ( node -> num_of_children == 0 )
  {
    fprintf ( stderr, "Cannot get first child of node with no children\n" );
    fprintf ( stderr, "At node %s\n", getNodeTypeName ( node -> node_type ) );
    return NULL;
  }

  return * ( ANODE ** ) ( node -> children -> head -> data . generic_val );
}

ANODE* getSecondChild ( ANODE *node )
{
  if ( node -> num_of_children < 2 )
  {
    fprintf ( stderr, "Cannot get non existent second child of node\n" );
    return NULL;
  }

  return * ( ANODE ** ) ( node -> children -> head -> next -> data . generic_val );
}

ANODE* getThirdChild ( ANODE *node )
{
  if ( node -> num_of_children < 3 )
  {
    fprintf ( stderr, "Cannot get non existent 3rd child\n" );
    return NULL;
  }

  return * ( ANODE ** ) ( node -> children -> head -> next -> next -> data . generic_val );
}

ANODE* getFourthChild ( ANODE *node )
{
  if ( node -> num_of_children < 4 )
  {
    fprintf ( stderr, "Cannot get non existent 4th child\n" );
    return NULL;
  }

  return * ( ANODE ** ) ( node -> children -> head -> next -> next -> next -> data . generic_val );
}

ANODE* getFifthChild ( ANODE *node )
{
  if ( node -> num_of_children < 5 )
  {
    fprintf ( stderr, "Cannot get non existent 5th child\n" );
    return NULL;
  }

  return * ( ANODE ** ) ( node -> children -> head -> next -> next -> next -> next -> data . generic_val );
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
    fprintf ( stderr, "Error: Break statements can only occur inside loops.\n" );

    // Continue processing after ignoring the node
    return;
  }
  else if ( currnode -> node_type == AST_RETURNSTMT_NODE && *infunction == 0 )
  {
    fprintf ( stderr, "Error: Return statements can only occur inside function definitions.\n" );
    return;
  }
  else if ( currnode -> node_type == AST_DEPTH_NODE && *loopcount <= 0 && *bdftcount <= 0 )
  {
    fprintf ( stderr, "Error: Depth can only be used inside a BFT / DFT based loop.\n" );
    return;
  }
  else if ( currnode -> node_type == AST_LET_NODE )
  {
    // The left and right children of Let node should have the same type
    if ( getFirstChild ( currnode ) -> result_type != getSecondChild ( currnode ) -> result_type )
    {
      if ( getFirstChild ( currnode ) -> result_type == D_FLOAT_TYPE
           && getSecondChild ( currnode ) -> result_type == D_INT_TYPE )
        fprintf ( stderr, "Warning: Implicit conversion from Int to Float in Let statement\n" );
      else
        fprintf ( stderr, "Error: Assigning incompatible types in let statement\n" );
      return;
    }

    STBENTRY *entry = getEntryByIndex (
        symboltable, getFirstChild ( getFirstChild ( currnode ) ) -> extra_data . symboltable_index );

    if ( entry -> entry_type == ENTRY_FUNC_TYPE )
    {
      fprintf ( stderr, "Error: Attempting to assign to a function identifier\n" );
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
      return;
    }
    else if ( currnode -> num_of_children == 3 )
    {
      // If Assignable has three children, then its first child should be a complex type
      currnode -> result_type = getThirdChild ( currnode ) -> result_type;

      if ( getFirstChild ( currnode ) -> result_type == D_STRING_TYPE
           || getFirstChild ( currnode ) -> result_type == D_INT_TYPE
           || getFirstChild ( currnode ) -> result_type == D_FLOAT_TYPE )
      {
        fprintf ( stderr, "Error: Cannot get members of primitive type\n" );
        return;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_VERTEX_TYPE && getThirdChild ( currnode ) -> num_of_children > 0 )
      {
        fprintf ( stderr, "Error: Cannot get non-primitive type member of a Vertex object\n" );
        return;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_EDGE_TYPE )
      {
        if ( getThirdChild ( currnode ) -> num_of_children > 0
             && getFirstChild ( getThirdChild ( currnode ) ) -> node_type == AST_ROOT_NODE )
        {
          fprintf ( stderr, "Error: Edge object has no root member\n" );
          return;
        }
      }

      if ( getFirstChild ( currnode ) -> result_type == D_GRAPH_TYPE )
      {
        fprintf ( stderr, "Error: Graph object has no members that can be referenced\n" );
        return;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_TREE_TYPE
           && getThirdChild ( currnode ) -> num_of_children > 0
           && getFirstChild ( getThirdChild ( currnode ) ) -> node_type != AST_ROOT_NODE )
      {
        fprintf ( stderr, "Error: Only root member of a Tree object may be referenced\n" );
        return;
      }
    }
    else
    {
      fprintf ( stderr, "Invalid tree produced! Assignable can't have more than 3 children\n" );
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
      fprintf ( stderr, "Error: Cannot read non-primitive value using the read statement\n" );
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
      fprintf ( stderr, "Error: Cannot print a value that is not a string, an int or a float\n" );
      return;
    }
  }
  else if ( currnode -> node_type == AST_COMPARE_NODE )
  {
    // The two children of a compare node MUST be of the exact same type
    // Otherwise throw an error

    if ( getFirstChild ( currnode ) -> result_type != getSecondChild ( currnode ) -> result_type )
    {
      fprintf ( stderr, "Error: Values on the two sides of the compare expression are not the same\n" );
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
          fprintf ( stderr, "Error: Floats and Ints can only be added to other Floats and Ints respectively\n" );
          return;
        }

        if ( firsttype == D_FLOAT_TYPE && currnode -> extra_data . arop_type == A_MODULO_TYPE )
        {
          // The right child gives us the operation type
          fprintf ( stderr, "Error: Modulo operator cannot be applied to Floats\n" );
          return;
        }

        result = firsttype;
      }
      else if ( firsttype == D_STRING_TYPE || secondtype == D_STRING_TYPE )
      {
        fprintf ( stderr, "Error: Operations on String are not allowed\n" );
        return;
      }
      else if ( firsttype == D_GRAPH_TYPE || firsttype == D_TREE_TYPE )
      {
        if ( currnode -> extra_data . arop_type != A_PLUS_TYPE
             && currnode -> extra_data . arop_type != A_MINUS_TYPE )
        {
          fprintf ( stderr, "Operator type: %d\n", getSecondChild ( currnode ) -> extra_data . arop_type );
          fprintf ( stderr, "Error: Only addition or removal operations allowed on Graphs and Trees\n" );
        }
        else if ( secondtype != D_VERTEX_TYPE && secondtype != D_EDGE_TYPE )
          fprintf ( stderr, "Error: Only Vertices and Edges may be added and removed from Graphs/Trees\n" );

        result = firsttype;
      }
      else if ( secondtype == D_GRAPH_TYPE || secondtype == D_TREE_TYPE )
      {
        if ( currnode -> extra_data . arop_type != A_PLUS_TYPE
             && currnode -> extra_data . arop_type != A_MINUS_TYPE )
          fprintf ( stderr, "Error: Only addition or removal operations allowed on Graphs and Trees\n" );
        else if ( firsttype != D_VERTEX_TYPE && firsttype != D_EDGE_TYPE )
          fprintf ( stderr, "Error: Only Vertices and Edges may be added and removed from Graphs/Trees\n" );

        result = secondtype;
      }
      else
        fprintf ( stderr, "Error: Invalid operands provided in expression\n" );

      currnode -> result_type = result;
    }
    else
      fprintf ( stderr, "Invalid tree produced! EXP node should not have more than 2 children\n" );
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
      fprintf ( stderr, "Error: Number of passed parameters don't match with the function definition\n" );
      return;
    }

    // If the number of parameters match, their types must also match
    LNODE funciterator, paramiterator;

    getIterator ( funcentry -> paramtypes, & funciterator );
    getIterator ( currnode -> children, & paramiterator );

    while ( hasNext ( & funciterator ) )
    {
      getNext ( funcentry -> parameters, & funciterator );
      getNext ( currnode -> children, & paramiterator );

      DATATYPE paramtype = funciterator . data . int_val;
      DATATYPE passedtype = ( * ( ANODE ** ) ( paramiterator . data . generic_val ) ) -> result_type;

      if ( paramtype != passedtype )
      {
        fprintf ( stderr, "Error: The type of the passed parameter to the function doesn't match the definition\n" );
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
        fprintf ( stderr, "Error: Type of value being returned does not match the return type in fn definition\n" );

      return;
    }

    if ( getFirstChild ( currnode ) -> result_type
         != getEntryByIndex ( symboltable, index ) -> data . func_data . ret_type )
    {
      fprintf ( stderr, "Error: Type of value being returned does not match the return type in fn definition\n" );
      return;
    }
  }
  else if ( currnode -> node_type == AST_FUNCTIONCALL_NODE )
  {
    unsigned int index = getFirstChild ( currnode ) -> extra_data . symboltable_index;

    if ( getEntryByIndex ( symboltable, index ) -> data . func_data . ret_type != D_NOTHING_TYPE )
    {
      fprintf ( stderr, "Warning: Return value of function is not being used\n" );
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
        fprintf ( stderr, "Error: The number of iterations of a for loop must be an Integer\n" );
      else if ( child -> node_type == AST_IDENTIFIER_NODE )
      {
        unsigned int index = child -> extra_data . symboltable_index;

        STBENTRY *entry = getEntryByIndex ( symboltable, index );

        if ( entry -> entry_type == ENTRY_FUNC_TYPE || entry -> data . var_data .data_type != D_INT_TYPE )
        {
          fprintf ( stderr, "Error: The identifier in a for loop must be an integer variable\n" );
          return;
        }
      }
    }
    else if ( currnode -> num_of_children == 3 )
      fprintf ( stderr, "Invalid tree produced! For node cannot have 3 children\n" );
    else if ( currnode -> num_of_children == 4 )
    {
      // The first child must be a Vertex or an Edge and the third child must be a Graph or Tree

      if ( getFirstChild ( currnode ) -> result_type != D_VERTEX_TYPE
           && getFirstChild ( currnode ) -> result_type != D_EDGE_TYPE )
      {
        fprintf ( stderr, "Error: The iteration variable must be a vertex or an edge\n" );
        return;
      }

      if ( getThirdChild ( currnode ) -> result_type != D_GRAPH_TYPE
           && getThirdChild ( currnode ) -> result_type != D_TREE_TYPE )
      {
        fprintf ( stderr, "Error: The object to iterate over must be a Graph or Tree\n" );
        return;
      }
    }
    else if ( currnode -> num_of_children == 5 )
      fprintf ( stderr, "Invalid tree produced! For node cannot have 5 children\n" );
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
        fprintf ( stderr, "Error: The iteration variable must be a vertex or an edge\n" );
        return;
      }

      if ( getThirdChild ( currnode ) -> node_type == AST_BDFT_NODE )
      {
        if ( getFourthChild ( currnode ) -> result_type != D_GRAPH_TYPE
             && getFourthChild ( currnode ) -> result_type != D_TREE_TYPE )
        {
          fprintf ( stderr, "Error: The first parameter to BFT or DFT must be a Graph or Tree\n" );
          return;
        }

        if ( getFifthChild ( currnode ) -> result_type != D_VERTEX_TYPE )
        {
          fprintf ( stderr, "Error: The second parameter to BFT/DFT must be a Vertex\n" );
          return;
        }
      }
      else
      {
        // The third child is not BDFT, thus adjacent to must be there

        if ( getThirdChild ( currnode ) -> result_type != D_GRAPH_TYPE
             && getThirdChild ( currnode ) -> result_type != D_TREE_TYPE )
        {
          fprintf ( stderr, "Error: The object to iterate over must be a Graph or Tree\n" );
          return;
        }

        if ( getFifthChild ( currnode ) -> result_type != D_VERTEX_TYPE )
        {
          fprintf ( stderr, "Error: The adjacent to parameter must be a Vertex\n" );
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
      fprintf ( stderr, "Error: The components of an edge must be vertices\n" );
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
      if ( getEntryByIndex ( symboltable,
            getFirstChild ( currnode ) -> extra_data . symboltable_index ) -> entry_type != ENTRY_FUNC_TYPE )
      {
        fprintf ( stderr, "Error: Attempting to call a variable like a function\n" );
        return;
      }
    }
    else if ( currnode -> num_of_children == 3 )
    {
      currnode -> result_type = getThirdChild ( currnode ) -> result_type;

      if ( getFirstChild ( currnode ) -> result_type == D_STRING_TYPE
           || getFirstChild ( currnode ) -> result_type == D_INT_TYPE
           || getFirstChild ( currnode ) -> result_type == D_FLOAT_TYPE )
      {
        fprintf ( stderr, "Error: Cannot get members of primitive type\n" );
        return;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_VERTEX_TYPE && getThirdChild ( currnode ) -> num_of_children > 0 )
      {
        fprintf ( stderr, "Error: Cannot get non-primitive type member of a Vertex object\n" );
        return;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_EDGE_TYPE )
      {
        if ( getThirdChild ( currnode ) -> num_of_children > 0
             && getFirstChild ( getThirdChild ( currnode ) ) -> node_type == AST_ROOT_NODE )
        {
          fprintf ( stderr, "Error: Edge object has no root member\n" );
          return;
        }
      }

      if ( getFirstChild ( currnode ) -> result_type == D_GRAPH_TYPE )
      {
        fprintf ( stderr, "Error: Graph object has no members that can be referenced\n" );
        return;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_TREE_TYPE
           && getThirdChild ( currnode ) -> num_of_children > 0
           && getFirstChild ( getThirdChild ( currnode ) ) -> node_type != AST_ROOT_NODE )
      {
        fprintf ( stderr, "Error: Only root member of a Tree object may be referenced\n" );
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
        fprintf ( stderr, "Error: Primitive type objects have no members\n" );
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
  fprintf ( datafile, "\t_int_string:\t\tdb\t'0000000000',10,0\n" );
  fprintf ( datafile, "\t_float_format:\t\tdb\t\"%%f\",10,0\n" );
  fprintf ( datafile, "\t_float_temp:\t\tdq\t0\n" );
  fprintf ( datafile, "\tintoffset:\t\tequ\t0\n" );
  fprintf ( datafile, "\tfloatoffset:\t\tequ\t1\n" );
  fprintf ( datafile, "\tstringoffset:\t\tequ\t2\n" );
  fprintf ( datafile, "\tsourceoffset:\t\tequ\t3\n" );
  fprintf ( datafile, "\tdestoffset:\t\tequ\t4\n" );
  fprintf ( datafile, "\tweightoffset:\t\tequ\t5\n" );
  fprintf ( datafile, "\trootoffset:\t\tequ\t2\n" );
  fprintf ( datafile, "\tvlistoffset:\t\tequ\t0\n" );
  fprintf ( datafile, "\telistoffset:\t\tequ\t1\n" );

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

void generateCode ( ANODE *currnode, SYMBOLTABLE *symboltable, FILE *assemblyfile, FILE *codefile,
                    FILE *functionfile, TRIE* literaltrie, LITDATA *literals, FILE *datafile )
{
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
        fprintf ( assemblyfile, "resw\t1\n" );
      else if ( vardata -> data_type == D_VERTEX_TYPE )
        fprintf ( assemblyfile, "resw\t3\n" );
      else if ( vardata -> data_type == D_STRING_TYPE )
        fprintf ( assemblyfile, "resw\t1\n" );
      else if ( vardata -> data_type == D_EDGE_TYPE )
        fprintf ( assemblyfile, "resw\t6\n" );
      else if ( vardata -> data_type == D_TREE_TYPE )
        fprintf ( assemblyfile, "resw\t3\n" );
      else if ( vardata -> data_type == D_GRAPH_TYPE )
        fprintf ( assemblyfile, "resw\t2\n" );
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
        fprintf ( datafile, "db\t%s,10\n", litdata -> value );
      else if ( litdata -> lit_type == D_INT_TYPE )
      {
        int value = atoi ( litdata -> value );
        int lower = ( value & LOWER16MASK ), upper = ( ( value & UPPER16MASK ) >> 16 );
        fprintf ( datafile, "dw\t%d,%d\n", lower, upper );
      }
      else if ( litdata -> lit_type == D_FLOAT_TYPE )
        fprintf ( datafile, "dd\t%s\n", litdata -> value );
    }

    curlitindex ++;
  }
  else if ( currnode -> node_type == AST_GLOBALDEFINES_NODE )
  {
    hasglobalvars = 1;
    startwritten = 1;
    fprintf ( assemblyfile, "\nsection .text\n\tglobal _start\n\n" );
  }
  else if ( currnode -> node_type == AST_LET_NODE )
  {
    STBENTRY *entry = getEntryByIndex ( symboltable,
        getFirstChild ( getFirstChild ( currnode ) ) -> extra_data . symboltable_index );

    if ( entry -> data . var_data . var_type == V_GLOBAL_TYPE )
    {
      // Entry is a global type, so we can copy to it using its name

    }
  }
  else if ( currnode -> node_type == AST_PRINT_NODE )
  {
    ANODE *child = getFirstChild ( currnode );

    if ( child -> num_of_children > 0 )
      child = getFirstChild ( child );

    STBENTRY *entry = getEntryByIndex ( symboltable, child -> extra_data . symboltable_index );

    if ( entry -> entry_type == ENTRY_LIT_TYPE )
    {
      TNODE *foundlit = findString ( literaltrie, entry -> data . lit_data . value );
      // Check and print the int or float literal
      if ( entry -> data . lit_data . lit_type == D_INT_TYPE )
      {
        shouldintprint = 1;
        fprintf ( codefile, "\tpusha\n" );
        fprintf ( codefile, "\tmov\teax, [ %s ]\n", literals [ foundlit -> data . int_val ] . name );
        fprintf ( codefile, "\tcall printInt\n" );
        fprintf ( codefile, "\tpopa\n" );
      }
      else if ( entry -> data . lit_data . lit_type == D_STRING_TYPE )
      {
        fprintf ( codefile, "\tmov\teax, 4\n" );
        fprintf ( codefile, "\tmov\tebx, 1\n" );
        fprintf ( codefile, "\tmov\tecx, %s\n", literals [ foundlit -> data . int_val ] . name );
        fprintf ( codefile, "\tmov\tedx, %d\n", ( int ) strlen ( entry -> data . lit_data . value ) - 1 );
        fprintf ( codefile, "\tint\t80h\n\n" );
      }
      else if ( entry -> data . lit_data . lit_type == D_FLOAT_TYPE )
      {
        fprintf ( codefile, "\tfld\tdword\t[%s]\n", literals [ foundlit -> data . int_val ] . name );
        fprintf ( codefile, "\tfstp\tqword\t[_float_temp]\n" );
        fprintf ( codefile, "\tpush\tdword\t[_float_temp+4]\n" );
        fprintf ( codefile, "\tpush\tdword\t[_float_temp]\n" );
        fprintf ( codefile, "\tpush\tdword\t_float_format\n" );
        fprintf ( codefile, "\tcall\tprintf\n" );
        fprintf ( codefile, "\tadd\tesp, 12\n" );
      }
    }
  }

  if ( functionfile == NULL )
    fprintf ( stderr, "Function file should not be NULL\n" );
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

      printf ( "Analyzing node %s on the way down\n", getNodeTypeName ( currnode -> node_type ) );

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
                     literaltrie, literals, datafile );

      printf ( "Analyzing node %s on the way up\n", getNodeTypeName ( currnode -> node_type ) );
    }
  }
}

void writeReturnZero ( FILE *codefile )
{
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
    return;
  }

  FILE *functionfile = NULL;
  functionfile = fopen ( ASSEMBLY_FUNCTIONS_FILE, "r" );

  if ( functionfile == NULL )
  {
    fprintf ( stderr, "Failed to open assembly functions file\n" );
    return;
  }

  FILE *varfile = NULL;
  varfile = fopen ( ASSEMBLY_FILE, "r" );

  if ( varfile == NULL )
  {
    fprintf ( stderr, "Failed to open variables file\n" );
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
  curlitindex = 0;
  TRIE *literaltrie = NULL;
  literaltrie = getNewTrie ( TRIE_INT_TYPE );

  layoutTemplate ( assemblyfile, codefile, datafile );

  checkAndGenerateCode ( ast, symboltable, stbdumpfile, assemblyfile, codefile, functionfile,
                         literaltrie, literals, datafile );

  if ( startwritten == 0 )
    fprintf ( assemblyfile, "\nsection .text\n\tglobal _start\n\n" );

  if ( shouldintprint == 1 )
    writeIntPrintFunction ( assemblyfile );

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

  return 0;
}

