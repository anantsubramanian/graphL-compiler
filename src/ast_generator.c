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

#define BUFFERLEN 400
#define INSTRLEN 5
#define DIGSTART 48
#define DIGEND 57
#define NEWLINE '\n'
#define COMMENT_START '#'
#define PROPERTIES_START '@'
#define PARSE_OUTPUT_FILE "PARSEOUTPUT"
#define ATTRIBUTES_FILE "TOKENMAP"
#define AST_OUTPUT_FILE "ASTOUTPUT"
#define STB_DUMP_FILE "STBDUMP"
#define AST_DUMP_FILE "ASTDUMP"
#define AST_NODETYPES_FILE "config/ast_nodetypes"
#define AST_INSTRUCTIONS_FILE "config/ast_instructions"
#define T_INDEX_FILE "config/terminals_index"
#define NT_INDEX_FILE "config/nonterminals_index"
#define ROOTNODE_NAME "AST_ROOT_NODE"
#define AST_STACK_NAME "AST Generation Stack"

#define DEBUG_ALL 0
#define DEBUG_ONCREATE 0
#define DEBUG_AUXOPS 0

#define PROPERTY_PARENT 1
#define PROPERTY_READ 2
#define PROPERTY_ADD 4
#define PROPERTY_CREATE 8

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

int erroroccured;

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

typedef struct property_data
{
  int jumps;
  int instruction;
  int node_type;
} PROPERTY;

typedef enum coditionalval_type
{
  CONDITIONAL_TYPE_FIRST = 0,
  CONDITIONAL_TERMINAL,
  CONDITIONAL_NONTERMINAL,
  CONDITIONAL_NODETYPE,
  CONDITIONAL_TYPE_LAST
} CONDTYPE;

typedef struct instruction_auxdata
{
  int conditional_value;
  CONDTYPE conditional_type;
} AUXDATA;

int makeTrieProperty ( char *instr )
{
  if ( instr == NULL )
  {
    fprintf ( stderr, "Cannot make property from non-existent instruction\n" );
    erroroccured = 1;
    return -1;
  }

  if ( strlen ( instr ) < 2 )
  {
    fprintf ( stderr, "Instruction should be 2 characters in length\n" );
    erroroccured = 1;
    return -1;
  }

  int result = 0;
  if ( instr [0] == 'P' )
    result |= PROPERTY_PARENT;
  else if ( instr [0] == 'C' )
    result |= PROPERTY_CREATE;

  if ( instr [1] == 'R' )
    result |= PROPERTY_READ;
  else if ( instr [1] == 'A' )
    result |= PROPERTY_ADD;

  return result;
}

DATATYPE getDataType ( ANODE *currnode )
{
  if ( currnode == NULL )
  {
    fprintf ( stderr, "Cannot get data type of child of non-existent node\n" );
    erroroccured = 1;
    return -1;
  }

  if ( currnode -> num_of_children <= 0 )
  {
    fprintf ( stderr, "Trying to get data type of child of node with no children\n" );
    erroroccured = 1;
    return -1;
  }

  ANODE *firstchild = *( ( ANODE ** ) ( currnode -> children -> head -> data . generic_val ) );

  return firstchild -> extra_data . data_type;
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

int extractLineNum ( char *input )
{
  int n = strlen ( input );
  char *p = input + n - 1;

  while ( *p != ' ' ) p--;
  p++;

  int lno = atoi ( p );
  *p = '\0';

  return lno;
}

void extractTokenData ( char *inputtoken, char **token, char **name, int *linenumber )
{
  if ( inputtoken == NULL )
  {
    fprintf ( stderr, "Cannot extract non-existent token data\n" );
    erroroccured = 1;
    return;
  }

  int len = strlen ( inputtoken + 1 );

  *token = malloc ( len * sizeof ( char ) );
  *name = malloc ( len * sizeof ( char ) );
  int i = 0;

  // Get token type

  while ( i < len && inputtoken [i] != ',' ) i++;

  if ( i == len )
  {
    fprintf ( stderr, "No auxiliary data seems to be present?\n" );
    erroroccured = 1;
    return;
  }

  inputtoken [i] = '\0';
  strcpy ( *token, inputtoken + 1 );
  i++;

  int namestart = i;

  // Get token name / value

  while ( i < len && inputtoken [i] != ',' ) i++;

  if ( i == len )
  {
    fprintf ( stderr, "Insufficient amount of auxiliary data\n" );
    erroroccured = 1;
    return;
  }

  inputtoken [i] = '\0';
  strcpy ( *name, inputtoken + namestart );
  i++;

  // Get token line number
  *linenumber = 0;

  while ( i < len && inputtoken [i] >= DIGSTART && inputtoken [i] <= DIGEND )
  {
    *linenumber = ( (*linenumber) * 10 ) + ( inputtoken [i++] - DIGSTART );
  }

}

int getLineCount ( FILE *inputfile, int blocksize )
{
  char c;

  int curbuff = -1;
  int charindx = -1;
  int lines = 0;
  int charsread = 0;
  char buffers [2] [blocksize];

  while ( TRUE )
  {
    // Get char from appropriate buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, inputfile ) ) == 0 )
        break;
    }
    c = buffers [ curbuff ] [ charindx ];

    if ( charsread < blocksize && charindx >= charsread )
      break;

    if ( c == NEWLINE )
      lines++;
  }

  return lines;
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

void getNodeInstructions ( FILE *instructionsfile, int blocksize, TRIE *instructions,
                           TRIE *auxdata, TRIE *nodetypemap, TRIE *nonterminals,
                           TRIE *terminals, TRIE *properties )
{
  char c;

  char buffers [2] [ blocksize ];
  char token [ BUFFERLEN ];
  char instr [ INSTRLEN ];
  char extradata [ BUFFERLEN ];
  char nodetype [ BUFFERLEN ];
  int num_jumps = 0;

  int charindx = -1;
  int curbuff = -1;
  int charsread = 0;
  int toriore = 0;

  int tokencounter = 0;
  int instrcounter = 0;
  int extracounter = 0;
  int nodetcounter = 0;

  int isfirst = 1;
  int incomment = 0;
  int started_properties = 0;
  int prevspace = 0;

  while ( TRUE )
  {
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, instructionsfile ) ) == 0 )
        break;
    }

    c = buffers [ curbuff ] [ charindx ];

    if ( charsread < blocksize && charindx >= charsread )
      break;

    if ( isfirst == 1 && c == COMMENT_START )
      incomment = 1;

    if ( c == NEWLINE )
    {
      if ( incomment != 1 )
      {
        // If the token is the special properties start marker
        if ( token [0] == PROPERTIES_START )
        {
          token [ tokencounter ] = '\0';
          if ( DEBUG_ALL ) printf ( "Started properties on %s\n", token );
          started_properties = 1;
          incomment = 0;
          isfirst = 1;
          toriore = 0;
          tokencounter = 0;
          continue;
        }

        // Parsing instructions and not properties
        if ( started_properties == 0 )
        {
          // Is a valid line to parse, and not a comment
          if ( toriore == 1 )
          {
            // No auxiliary data
            instr [ instrcounter ] = '\0';

            TNODE *temp = insertString ( instructions, token );
            temp -> data . int_val = createProperty ( instr );

            if ( DEBUG_ALL ) printf ( "Instruction %s %s %d\n", token, instr, createProperty ( instr ) );
          }
          else
          {
            // Auxiliary data is there
            extradata [ extracounter ] = '\0';
            nodetype [ nodetcounter ] = '\0';

            TNODE *temp = insertString ( instructions, token );
            temp -> data . int_val = createProperty ( instr );

            temp = insertString ( auxdata, token );

            // Is the aux data a node type, a terminal or a non terminal?
            TNODE *istype = findString ( nodetypemap, extradata );
            TNODE *isterm = findString ( terminals, extradata );
            TNODE *isntrm = findString ( nonterminals, extradata );

            AUXDATA tempdata;

            if ( istype != NULL )
            {
              // Was a node type
              tempdata . conditional_value = istype -> data . int_val;
              tempdata . conditional_type = CONDITIONAL_NODETYPE;
              setValue ( auxdata, temp, & tempdata );
            }
            else if ( isterm != NULL )
            {
              // Was a terminal
              tempdata . conditional_value = isterm -> data . int_val;
              tempdata . conditional_type = CONDITIONAL_TERMINAL;
              setValue ( auxdata, temp, & tempdata );
            }
            else if ( isntrm != NULL )
            {
              // Was a non terminal
              tempdata . conditional_value = isntrm -> data . int_val;
              tempdata . conditional_type = CONDITIONAL_NONTERMINAL;
              setValue ( auxdata, temp, & tempdata );
            }
            else
            {
              fprintf ( stderr, "Invalid auxiliary data in the instructions file\n" );
              exit (-1);
            }
            if ( DEBUG_ALL ) printf ( "Instruction %s %s %d %s\n", token, instr, createProperty ( instr ), extradata );
          }
        }
        else
        {
          extradata [ extracounter ] = '\0';
          nodetype [ nodetcounter ] = '\0';
          if ( DEBUG_ALL ) printf ( "Property %s %d %s %s %s\n", token, num_jumps, instr, extradata, nodetype );
          // Parsing properties now
          TNODE* nodeexists = findString ( properties, token );
          TRIE* level2trie = NULL;
          if ( nodeexists == NULL )
          {
            nodeexists = insertString ( properties, token );
            nodeexists -> data . generic_val = getNewTrie ( TRIE_GENERIC_TYPE );
            nodeexists -> data . generic_val = setTrieGenericSize ( (TRIE *) nodeexists -> data . generic_val, sizeof ( PROPERTY ) );
          }

          level2trie = (TRIE*) ( nodeexists -> data . generic_val );

          TNODE* propertyexists = findString ( level2trie, extradata );
          if ( propertyexists == NULL )
            propertyexists = insertString ( level2trie, extradata );

          PROPERTY temp;
          temp . jumps = num_jumps;
          temp . instruction = makeTrieProperty ( instr );
          if ( ( temp . instruction & PROPERTY_CREATE ) == PROPERTY_CREATE )
          {
            TNODE *findnodetype = findString ( nodetypemap, nodetype );
            if ( findnodetype == NULL )
            {
              fprintf ( stderr, "Invalid node type for create property in the instructions file\n" );
              exit (-1);
            }

            temp . node_type = findnodetype -> data . int_val;
          }

          propertyexists = setValue ( level2trie, propertyexists, & temp );
        }
      }

      incomment = 0;
      isfirst = 1;
      toriore = 0;
      tokencounter = 0;
    }
    else if ( incomment == 1 )
    {
      isfirst = 0;
      continue;
    }
    else if ( c == ' ' )
    {
      isfirst = 0;
      if ( prevspace == 1 ) continue;
      prevspace = 1;
      if ( toriore == 0 )
      {
        token [ tokencounter ] = '\0';
        instrcounter = 0;
        num_jumps = 0;
        toriore = 1;
      }
      else if ( toriore == 1 && started_properties == 0 )
      {
        instr [ instrcounter ] = '\0';
        extracounter = 0;
        toriore = 2;
      }
      else if ( toriore == 1 && started_properties == 1 )
      {
        instrcounter = 0;
        toriore = 2;
      }
      else if ( toriore == 2 && started_properties == 1 )
      {
        instr [ instrcounter ] = '\0';
        extracounter = 0;
        toriore = 3;
      }
      else if ( toriore == 3 && started_properties == 1 )
      {
        extradata [ extracounter ] = '\0';
        nodetcounter = 0;
        toriore = 4;
      }
    }
    else if ( toriore == 0 )
      token [ tokencounter++ ] = c, prevspace = 0, isfirst = 0;
    else if ( toriore == 1 && started_properties == 0 )
      instr [ instrcounter++ ] = c, prevspace = 0, isfirst = 0;
    else if ( toriore == 1 && started_properties == 1 )
    {
      num_jumps = num_jumps * 10 + c - 48;
      prevspace = 0, isfirst = 0;
    }
    else if ( toriore == 2 && started_properties == 0 )
      extradata [ extracounter++ ] = c, prevspace = 0, isfirst = 0;
    else if ( toriore == 2 && started_properties == 1 )
      instr [ instrcounter++ ] = c, prevspace = 0, isfirst = 0;
    else if ( toriore == 3 )
      extradata [ extracounter++ ] = c, prevspace = 0, isfirst = 0;
    else if ( toriore == 4 )
      nodetype [ nodetcounter++ ] = c, prevspace = 0, isfirst = 0;
  }

}

void getNodeIntegers ( int *beginint, int *endint, int *idenint, int *intlitint,
                       int *stringlitint, int *floatlitint, int *intint,
                       int *stringint, int *floatint, int *vertexint, int *edgeint,
                       int *graphint, int *treeint, int *nothingint, int *andint,
                       int *orint, int *notint, int *plusint, int *minusint,
                       int *mulint, int *divint, int *moduloint, int *gtint,
                       int *ltint, int *gteint, int *lteint, int *eqint,
                       int *bftint, int *dftint, int *functionint, TRIE *terminals )
{

  TNODE *beginnode = findString ( terminals, TK_BEGIN );
  TNODE *endnode = findString ( terminals, TK_END );
  TNODE *idennode = findString ( terminals, TK_IDEN );
  TNODE *intlitnode = findString ( terminals, TK_INTLIT );
  TNODE *stringlitnode = findString ( terminals, TK_STRINGLIT );
  TNODE *floatlitnode = findString ( terminals, TK_FLOATLIT );
  TNODE *intnode = findString ( terminals, TK_INT );
  TNODE *stringnode = findString ( terminals, TK_STRING );
  TNODE *floatnode = findString ( terminals, TK_FLOAT );
  TNODE *vertexnode = findString ( terminals, TK_VERTEX );
  TNODE *edgenode = findString ( terminals, TK_EDGE );
  TNODE *graphnode = findString ( terminals, TK_GRAPH );
  TNODE *treenode = findString ( terminals, TK_TREE );
  TNODE *nothingnode = findString ( terminals, TK_NOTHING );
  TNODE *andnode = findString ( terminals, TK_AND );
  TNODE *ornode = findString ( terminals, TK_OR );
  TNODE *notnode = findString ( terminals, TK_NOT );
  TNODE *plusnode = findString ( terminals, TK_PLUS );
  TNODE *minusnode = findString ( terminals, TK_MINUS );
  TNODE *mulnode = findString ( terminals, TK_MUL );
  TNODE *divnode = findString ( terminals, TK_DIV );
  TNODE *modulonode = findString ( terminals, TK_MODULO );
  TNODE *gtnode = findString ( terminals, TK_GT );
  TNODE *ltnode = findString ( terminals, TK_LT );
  TNODE *gtenode = findString ( terminals, TK_GTE );
  TNODE *ltenode = findString ( terminals, TK_LTE );
  TNODE *eqnode = findString ( terminals, TK_EQ );
  TNODE *bftnode = findString ( terminals, TK_BFT );
  TNODE *dftnode = findString ( terminals, TK_DFT );
  TNODE *functionnode = findString ( terminals, TK_FUNCTION );

  if ( beginnode == NULL
       || endnode == NULL
       || idennode == NULL
       || intlitnode == NULL
       || stringlitnode == NULL
       || floatlitnode == NULL
       || intnode == NULL
       || stringnode == NULL
       || floatnode == NULL
       || vertexnode == NULL
       || edgenode == NULL
       || graphnode == NULL
       || treenode == NULL
       || nothingnode == NULL
       || andnode == NULL
       || ornode == NULL
       || notnode == NULL
       || plusnode == NULL
       || minusnode == NULL
       || mulnode == NULL
       || divnode == NULL
       || modulonode == NULL
       || gtnode == NULL
       || ltnode == NULL
       || gtenode == NULL
       || ltenode == NULL
       || eqnode == NULL
       || bftnode == NULL
       || functionnode == NULL
       || dftnode == NULL ) fprintf ( stderr, "Failed to find required terminal for semantic analysis\n" ), exit (-1);

  *beginint = beginnode -> data . int_val;
  *endint = endnode -> data . int_val;
  *idenint = idennode -> data . int_val;
  *intlitint = intlitnode -> data . int_val;
  *stringlitint = stringlitnode -> data . int_val;
  *floatlitint = floatlitnode -> data . int_val;
  *intint = intnode -> data . int_val;
  *stringint = stringnode -> data . int_val;
  *floatint = floatnode -> data . int_val;
  *vertexint = vertexnode -> data . int_val;
  *edgeint = edgenode -> data . int_val;
  *graphint = graphnode -> data . int_val;
  *treeint = treenode -> data . int_val;
  *nothingint = nothingnode -> data . int_val;
  *andint = andnode -> data . int_val;
  *orint = ornode -> data . int_val;
  *notint = notnode -> data . int_val;
  *plusint = plusnode -> data . int_val;
  *minusint = minusnode -> data . int_val;
  *mulint = mulnode -> data . int_val;
  *divint = divnode -> data . int_val;
  *moduloint = modulonode -> data . int_val;
  *gtint = gtnode -> data . int_val;
  *ltint = ltnode -> data . int_val;
  *gteint = gtenode -> data . int_val;
  *lteint = ltenode -> data . int_val;
  *eqint = eqnode -> data . int_val;
  *bftint = bftnode -> data . int_val;
  *dftint = dftnode -> data . int_val;
  *functionint = functionnode -> data . int_val;

}

void handleAuxiliaryTerminalOperations (
    int beginint, int endint, int idenint, int intlitint,
    int stringlitint, int floatlitint, int intint,
    int stringint, int floatint, int vertexint, int edgeint,
    int graphint, int treeint, int nothingint, int andint,
    int orint, int notint, int plusint, int minusint,
    int mulint, int divint, int moduloint, int gtint,
    int ltint, int gteint, int lteint, int eqint,
    int bftint, int dftint, int functionint, int terminalvalue,
    ANODE *currnode, SYMBOLTABLE *symboltable, int *should_start_function,
    int *function_scope_started, char *tokenname, int linenumber,
    FILE *stbdumpfile )
{

  // Here  'topvalue' = the string that was popped from the stack
  //       'token' = the next line that was read from the input
  //       'tokenname' = name of the variable / value of the literal
  //       linenumber = the linenumber for variables / literals

  if ( terminalvalue == beginint )
  {
    if ( *function_scope_started == 0 )
    {
      // If it is TK_BEGIN
      symboltable = openEnv ( symboltable );
      if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Opening environment\n\n" );
    }
    else
      *function_scope_started = 0;
  }
  else if ( terminalvalue == endint )
  {
    // If it is TK_END
    symboltable = closeEnv ( symboltable );
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Closing environment\n\n" );
  }
  else if ( terminalvalue == ltint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning LT type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . compop_type = C_LT_TYPE;
  }
  else if ( terminalvalue == lteint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning LTE type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . compop_type = C_LTE_TYPE;
  }
  else if ( terminalvalue == gtint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning GT type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . compop_type = C_GT_TYPE;
  }
  else if ( terminalvalue == gteint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning GTE type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . compop_type = C_GTE_TYPE;
  }
  else if ( terminalvalue == eqint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning EQ type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . compop_type = C_EQ_TYPE;
  }
  else if ( terminalvalue == andint )
  {
    if ( currnode -> node_type == AST_BOOLEXP_NODE || currnode -> node_type == AST_BOOLOP_NODE )
    {
      if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning AND type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
      currnode -> extra_data . boolop_type = B_AND_TYPE;
    }
    else if ( DEBUG_ALL )
      printf ( "Got AND at non-boolop/boolexp node so ignoring...\n" );
  }
  else if ( terminalvalue == orint )
  {
    if ( currnode -> node_type == AST_BOOLEXP_NODE || currnode -> node_type == AST_BOOLOP_NODE )
    {
      if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning OR type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
      currnode -> extra_data . boolop_type = B_OR_TYPE;
    }
    else if ( DEBUG_ALL )
      printf ( "Got OR at non-boolop/boolexp node so ignoring...\n" );
  }
  else if ( terminalvalue == notint )
  {
    if ( currnode -> node_type == AST_BOOLEXP_NODE || currnode -> node_type == AST_BOOLOP_NODE )
    {
      if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning NOT type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
      currnode -> extra_data . boolop_type = B_NOT_TYPE;
    }
    else if ( DEBUG_ALL )
      printf ( "Got NOT at non-boolop/boolexp node so ignoring...\n" );
  }
  else if ( terminalvalue == bftint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning BFT type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . bdft_type = BDFT_BFT_TYPE;
  }
  else if ( terminalvalue == dftint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning DFT type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . bdft_type = BDFT_DFT_TYPE;
  }
  else if ( terminalvalue == plusint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning PLUS type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . arop_type = A_PLUS_TYPE;
  }
  else if ( terminalvalue == minusint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning MINUS type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . arop_type = A_MINUS_TYPE;
  }
  else if ( terminalvalue == mulint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning MUL type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . arop_type = A_MUL_TYPE;
  }
  else if ( terminalvalue == divint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning DIV type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . arop_type = A_DIV_TYPE;
  }
  else if ( terminalvalue == moduloint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning MODULO type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . arop_type = A_MODULO_TYPE;
  }
  else if ( terminalvalue == intint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning INT type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_INT_TYPE;
  }
  else if ( terminalvalue == floatint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning FLOAT type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_FLOAT_TYPE;
  }
  else if ( terminalvalue == stringint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning STRING type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_STRING_TYPE;
  }
  else if ( terminalvalue == vertexint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning VERTEX type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_VERTEX_TYPE;
  }
  else if ( terminalvalue == edgeint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning EDGE type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_EDGE_TYPE;
  }
  else if ( terminalvalue == graphint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning GRAPH type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_GRAPH_TYPE;
  }
  else if ( terminalvalue == treeint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning TREE type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_TREE_TYPE;
  }
  else if ( terminalvalue == nothingint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL ) printf ( "Assigning NOTHING type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_NOTHING_TYPE;
  }
  else if ( terminalvalue == functionint )
  {
    // If function keyword has been seen, then start the function scope after the next identifier
    // and ignore the next begin
    *should_start_function = 1;
  }
  else if ( terminalvalue == idenint )
  {
    ANODE *parentnode = currnode -> parent;
    if ( parentnode -> node_type == AST_DEFINE_NODE
         || parentnode -> node_type == AST_FUNCTION_NODE
         || parentnode -> node_type == AST_GLOBALDEFINE_NODE
         || parentnode -> node_type == AST_QUALIFIEDPARAMETER_NODE )
    {
      // At a define node, so need to add an entry to the symbol table.
      // the data type for the node should be identifiable from the first child
      // of the current node.

      if ( getEntryByName ( symboltable, tokenname ) != NULL )
      {
        // Report an error if the current identifier is a variable and had been declared in the same scope before
        if ( parentnode -> node_type != AST_FUNCTION_NODE )
        {
          STBENTRY *previousentry = getEntryByName ( symboltable, tokenname );

          if ( previousentry -> entry_type == ENTRY_FUNC_TYPE )
          {
            // A variable is being declared now, but the previous entry by this name is a function
            // Throw an error and ignore this variable definition
            fprintf ( stderr, "Redeclaration of variable %s at line %d\n", tokenname, linenumber );
            fprintf ( stderr, "Note: Previous declaration of %s as a function on line %d.\n",
                      tokenname, previousentry -> data . func_data . decl_line );
            erroroccured = 1;

            // Do not count this variable declaration
            // Return from the function to avoid further processing
            return;
          }

          if ( previousentry -> data . var_data . scope_level == symboltable -> cur_scope )
          {
            fprintf ( stderr, "Redeclaration of variable %s at line number %d\n", tokenname, linenumber );
            fprintf ( stderr, "Note: Previous declaration of %s as type %s on line %d.\n",
                      tokenname, getDataTypeName ( previousentry -> data . var_data . data_type ),
                      previousentry -> data . var_data . decl_line );
            erroroccured = 1;

            // Do not count the redeclaration, process the rest of the input as
            // though this declaration didn't occur
            return;
          }
        }
        else
        {
          // Is a duplicate function definition
          STBENTRY *previousentry = getEntryByName ( symboltable, tokenname );
          fprintf ( stderr, "Redeclaration of function %s at line number %d\n", tokenname, linenumber );
          fprintf ( stderr, "Note: Previous declaration of %s at line number %d.\n", tokenname,
                    previousentry -> data . func_data . decl_line );
          erroroccured = 1;

          // Update the function to the latest declaration, and let the
          // parameters be updated automatically.
          // Don't return at this point, this way the start of the func
          // scope is handled correctly after the else below
        }
      }
      else
      {
        unsigned insertedIndex = -1;

        if ( parentnode -> node_type == AST_FUNCTION_NODE )
        {
          insertedIndex = ( unsigned int ) addEntry ( symboltable, tokenname, ENTRY_FUNC_TYPE );
          if ( DEBUG_AUXOPS ) printf ( "Added function entry %s at scope %d\n", tokenname, symboltable -> cur_scope );
          STBENTRY *insertedEntry = getEntryByIndex ( symboltable, insertedIndex );

          FUNCTION *funcdata = & ( insertedEntry -> data . func_data );

          funcdata -> decl_line = linenumber;

          int len = strlen ( tokenname );
          funcdata -> name = malloc ( (len+1) * sizeof ( char ) );

          if ( funcdata -> name == NULL )
          {
            fprintf ( stderr, "Failed to allocate memory for function name\n" );
            exit (-1);
          }

          strcpy ( funcdata -> name, tokenname );

          currnode -> extra_data . symboltable_index = insertedIndex;

          dumpEntry ( symboltable, stbdumpfile, insertedIndex, 'd' );
        }
        else
        {
          // Not a function node, so it must be a variable declaration.
          // Add the corresponding entry in the symbol table.
          insertedIndex = addEntry ( symboltable, tokenname, ENTRY_VAR_TYPE );
          if ( DEBUG_AUXOPS ) printf ( "Added variable entry %s at scope %d\n", tokenname, symboltable -> cur_scope );

          STBENTRY *insertedEntry = getEntryByIndex ( symboltable, insertedIndex );

          VARIABLE *vardata = & ( insertedEntry -> data . var_data );

          if ( parentnode -> node_type == AST_GLOBALDEFINE_NODE )
            vardata -> var_type = V_GLOBAL_TYPE;
          else if ( parentnode -> node_type == AST_QUALIFIEDPARAMETER_NODE )
            vardata -> var_type = V_PARAM_TYPE;
          else if ( parentnode -> node_type == AST_DEFINE_NODE )
            vardata -> var_type = V_LOCAL_TYPE;
          else
          {
            fprintf ( stderr, "At node %s did not recognize variable type\n", getNodeTypeName ( currnode -> node_type ) );
            erroroccured = 1;
          }

          // Set the data type using the data type of the first child
          vardata -> data_type = getDataType ( parentnode );
          vardata -> decl_line = linenumber;

          if ( DEBUG_AUXOPS ) printf ( "Set data type of %s as %s in Symbol Table\n", tokenname, getDataTypeName ( vardata -> data_type ) );

          currnode -> extra_data . symboltable_index = insertedIndex;

          dumpEntry ( symboltable, stbdumpfile, insertedIndex, 'd' );
        }
      }
    }
    else
    {
      // Variable is being referenced, not declared, here so push it into the linkedlist of references
      STBENTRY *foundEntry = getEntryByName ( symboltable, tokenname );

      // If the variable is not found in the symbol table, it hasn't been declared yet in this scope.
      // so report an error
      if ( foundEntry == NULL )
      {
        fprintf ( stderr, "Variable %s used but has not been declared in this scope\n", tokenname );
        fprintf ( stderr, "Note: Error at line %d.\n", linenumber );

        erroroccured = 1;

        // Let all the rest of the errors associated with the undeclared variable
        // show up as well, for ease of debugging
        return;
      }
      else
      {
        if ( foundEntry -> entry_type == ENTRY_VAR_TYPE )
          foundEntry -> data . var_data . refr_lines =
            insertAtBack ( foundEntry -> data . var_data . refr_lines, &linenumber );
        else if ( foundEntry -> entry_type == ENTRY_FUNC_TYPE )
          foundEntry -> data . func_data . refr_lines =
            insertAtBack ( foundEntry -> data . func_data . refr_lines, &linenumber );

      }

      currnode -> extra_data . symboltable_index = foundEntry -> index;

      dumpEntry ( symboltable, stbdumpfile, foundEntry -> index, 'r' );
    }

    // Finished processing the identifier, if this identifier was a function name,
    // then we need to start a new scop here itself, so that parameters belong to
    // the inner scope.
    // We set the function_scope_started parameter so that the TK_BEGIN for the
    // function body doesn't start another scope, again.

    if ( *should_start_function == 1 )
    {
      *should_start_function = 0;
      symboltable = openEnv ( symboltable );
      *function_scope_started = 1;
    }
  }
  else if ( terminalvalue == intlitint || terminalvalue == floatlitint
            || terminalvalue == stringlitint )
  {
    // Add entry for the literal in the symbol table if it doesn't already exist

    STBENTRY *entry = getEntryByName ( symboltable, tokenname );

    // If entry doesn't exist, the entry needs to be added for the literal
    // If the entry already exists, nothing further needs to be done.
    if ( entry == NULL )
    {
      unsigned int entryIndex = addEntry ( symboltable, tokenname, ENTRY_LIT_TYPE );
      entry = getEntryByIndex ( symboltable, entryIndex );

      // Entry now has the entry index. Set the value appropriately
      if ( terminalvalue == intlitint )
        entry -> data . lit_data . lit_type = D_INT_TYPE;
      else if ( terminalvalue == floatlitint )
        entry -> data . lit_data . lit_type = D_FLOAT_TYPE;
      else if ( terminalvalue == stringlitint )
        entry -> data . lit_data . lit_type = D_STRING_TYPE;

      int len = strlen ( tokenname );
      entry -> data . lit_data . value = malloc ( (len+1) * sizeof ( char ) );

      if ( entry -> data . lit_data . value == NULL )
      {
        fprintf ( stderr, "Failed to allocate memory for literal\n" );
        exit (-1);
      }

      strcpy ( entry -> data . lit_data . value, tokenname );
    }

    currnode -> extra_data . symboltable_index = entry -> index;

    // Dump literal entries every time, no difference b/w definition and reference
    dumpEntry ( symboltable, stbdumpfile, entry -> index, 'd' );
  }

  // End handling aux ops for terminals
  // Nothing more needs to be done

}

void handleNodeProperty ( ANODE **currnode, const char *topvalue, TNODE *jumps,
                          STACK **stack, AST *ast, int *shouldAdd, int *shouldRead )
{
  int numberOfJumps = ( (PROPERTY *) jumps -> data . generic_val ) -> jumps;
  int shouldCreate = ( ( ( ( (PROPERTY *) jumps -> data . generic_val ) -> instruction ) & PROPERTY_CREATE ) == PROPERTY_CREATE );
  int nodetype_tocreate = ( (PROPERTY *) jumps -> data . generic_val ) -> node_type;
  *shouldRead = ( ( ( ( (PROPERTY *) jumps -> data . generic_val ) -> instruction ) & PROPERTY_READ ) == PROPERTY_READ );
  *shouldAdd = ( ( ( ( (PROPERTY *) jumps -> data . generic_val ) -> instruction ) & PROPERTY_ADD ) == PROPERTY_ADD );

  if ( DEBUG_ALL ) printf ( "At node %s got %s so jumping %d times\n", getNodeTypeName ( (*currnode) -> node_type ),
                                                                       topvalue, numberOfJumps );
  if ( shouldCreate == 0 )
  {
    // Should jump
    while ( numberOfJumps > 0 )
    {
      if ( getParent ( *currnode ) == NULL || getParent ( *currnode ) == ast -> root )
        break;
      *currnode = getParent ( *currnode );
      numberOfJumps --;
    }
  }
  else if ( shouldCreate == 1 )
  {
    if ( DEBUG_ALL || DEBUG_ONCREATE )
      printf ( "At node %s got %s so creating and going to %s\n\n",
               getNodeTypeName ( (*currnode) -> node_type ), topvalue,
               getNodeTypeName ( nodetype_tocreate ) );

    *currnode = addChild ( *currnode, nodetype_tocreate, GOTOCH );

    // numberOfJumps is the number of elements to pop from the stack
    while ( numberOfJumps > 0 )
    {
      if ( isEmpty ( *stack ) )
      {
        fprintf ( stderr, "Instructions say pop elements, but stack is empty!\n" );
        exit (-1);
      }
      *stack = pop ( *stack );
      numberOfJumps --;
    }
  }

  return;
}

int handleNodeInstruction ( FILE *astoutput, ANODE **currnode, TNODE *currentval, AST *ast,
                            const char *topvalue, TRIE *auxdata, int *conditional_read,
                            int *conditional_pop, int *conditional_value, CONDTYPE *conditional_type )
{
  // instruction stores the encoded instruction, the individual bits need to be
  // examined to decode the instruction
  int instruction = currentval -> data . int_val;

  if ( (instruction & CREATE) == CREATE )
  {
    // If we have to create a node, examine the type of node to be created
    TNODE *typeofnode = findString ( auxdata, topvalue );

    if ( typeofnode == NULL )
    {
      fprintf ( stderr, "Node instruction is create, but type to create not found\n" );
      exit (-1);
    }

    if ( ( (AUXDATA *) ( typeofnode -> data . generic_val ) ) -> conditional_type != CONDITIONAL_NODETYPE )
    {
      fprintf ( stderr, "Node instruction is create, but the auxiliary data provided is not a node type\n" );
      exit (-1);
    }

    int type_to_create = ( (AUXDATA *) ( typeofnode -> data . generic_val ) ) -> conditional_value;

    if ( DEBUG_ALL || DEBUG_ONCREATE ) printf ( "At node %s\n", getNodeTypeName ( (*currnode) -> node_type ) );

    fprintf ( astoutput, "At node %s\n", getNodeTypeName ( (*currnode) -> node_type ) );

    *currnode = addChild ( *currnode, type_to_create, instruction );

    if ( DEBUG_ALL ) printf ( "Got %s so ", topvalue );

    if ( DEBUG_ALL || DEBUG_ONCREATE ) printf ( "Creating: %s\n\n", getNodeTypeName ( type_to_create ) );

    fprintf ( astoutput, "Creating: %s\n\n", getNodeTypeName ( type_to_create ) );

    if ( DEBUG_ALL ) printf ( "\n\n" );

    // currnode is now either the same node, child, or parent depending on
    // the instruction.

    // If the instruction says read, then return the read status 1.
    if ( (instruction & READ) == READ )
      return 1;
  }
  else if ( (instruction & GOTOCH) == GOTOCH )
  {
    fprintf ( stderr, "Instruction says goto child, but no child was created. So which child?\n" );
    return -1;
  }
  else if ( (instruction & PARENT) == PARENT )
  {
    if ( DEBUG_ALL ) printf ( "At node %s\n", getNodeTypeName ( (*currnode) -> node_type ) );
    if ( DEBUG_ALL ) printf ( "Got %s so ", topvalue );
    if ( DEBUG_ALL ) printf ( "Going to parent\n" );

    if ( getParent ( *currnode ) == NULL || getParent ( *currnode ) == ast -> root )
      fprintf ( stderr, "Parent is null, so remaining at same node\n" );
    else
      *currnode = getParent ( *currnode );

    // If the instruction says read, we should return the read status 1
    if ( (instruction & READ) == READ )
      return 1;
  }
  else if ( (instruction & CONDRD) == CONDRD )
  {
    if ( DEBUG_ALL ) printf ( "At node %s\n", getNodeTypeName ( (*currnode) -> node_type ) );
    if ( DEBUG_ALL ) printf ( "Got %s so ", topvalue );
    if ( DEBUG_ALL ) printf ( "conditionally reading/popping next value %d\n", instruction );
    // Instruction is a conditional read
    // Set the conditional read flag and value so that
    // currnode will be changed to parent on that value

    if ( (instruction & READ) == READ )
      *conditional_read = 1;
    else
      *conditional_pop = 1;

    TNODE *cond_value = findString ( auxdata, topvalue );
    if ( cond_value == NULL )
    {
      fprintf ( stderr, "Conditional read & goto parent has no auxiliary data\n" );
      return -1;
    }

    *conditional_value = ( (AUXDATA *) ( cond_value -> data . generic_val ) ) -> conditional_value;
    *conditional_type = ( (AUXDATA *) ( cond_value -> data . generic_val ) ) -> conditional_type;

    if ( (instruction & READ) == READ )
      return 1;

    if ( DEBUG_ALL ) printf ( "Not reading as conditional read failed\n" );
  }
  else if ( (instruction & READ) == READ )
  {
    if ( DEBUG_ALL ) printf ( "At node %s\n", getNodeTypeName ( (*currnode) -> node_type ) );
    if ( DEBUG_ALL ) printf ( "Got %s so ", topvalue );
    if ( DEBUG_ALL ) printf ( "Reading next input...\n" );
    return 1;
  }

  // Should not read, so return 0
  return 0;
}

AST* createAST ( FILE * parseroutput, int blocksize, AST *ast, TRIE *instructions,
                 TRIE *auxdata, TRIE *nonterminals, TRIE *terminals, TRIE *properties,
                 SYMBOLTABLE *symboltable, FILE *astoutput )
{
  FILE *stbdumpfile = NULL;
  stbdumpfile = fopen ( STB_DUMP_FILE, "w+" );

  if ( stbdumpfile == NULL )
  {
    fprintf ( stderr, "Failed to open STB dump file\n" );
    erroroccured = 1;
    return NULL;
  }

  fprintf ( stbdumpfile, "%u\n", symboltable -> num_entries );

  // We start processing from the root node
  ANODE *currnode = ast -> root;

  setAstNodeName ( currnode, ROOTNODE_NAME );

  STACK *stack = NULL;
  stack = getStack ( STACK_STRING_TYPE );
  stack = setStackName ( stack, AST_STACK_NAME );

  // Open a global environment in the symbol table
  symboltable = openEnv ( symboltable );

  char c;

  char buffers [2] [blocksize];
  char token [ BUFFERLEN ];

  int charindx = -1;
  int curbuff = -1;
  int charsread = 0;
  int tokencounter = 0;
  int curline = 1;

  int function_scope_started = 0;     // Flag used to detect that the scope for a function has already started
  int should_start_function = 0;      // Flag used to indicate that a function scope must be started

  int conditional_read = 0;
  int conditional_pop = 0;
  int conditional_value = -1;
  CONDTYPE conditional_type = CONDITIONAL_TYPE_FIRST;

  // Get the integer index numbers for the required terminals in the language
  // for efficient comparison

  int beginint, endint, idenint, intlitint, stringlitint, floatlitint, intint,
      stringint, floatint, vertexint, edgeint, graphint, treeint, nothingint,
      andint, orint, notint, plusint, minusint, mulint, divint, moduloint,
      gtint, ltint, gteint, lteint, eqint, bftint, dftint, functionint;

  getNodeIntegers ( & beginint, & endint, & idenint, & intlitint, & stringlitint,
                    & floatlitint, & intint, & stringint, & floatint, & vertexint,
                    & edgeint, & graphint, & treeint, & nothingint, & andint, & orint,
                    & notint, & plusint, & minusint, & mulint, & divint, & moduloint,
                    & gtint, & ltint, & gteint, & lteint, & eqint, & bftint, & dftint,
                    & functionint, terminals );

  // Read the output produced by the parser, line-by-line and follow the instructions
  // from the ast_instructions file to construct the AST

  while ( TRUE )
  {
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, parseroutput ) ) == 0 )
        break;
    }

    c = buffers [ curbuff ] [ charindx ];


    if ( charsread < blocksize && charindx >= charsread )
      break;

    if ( c == NEWLINE )
    {
      token [ tokencounter ] = '\0';

      // Set the previous curline value to the current node, whatever it may be
      // and get the new curline value to be set in the next iteration
      currnode -> line_no = curline;
      curline = extractLineNum ( token );

      // A whole line of expansion has been read. Need to process
      // it so push the words onto the stack, from which they will
      // be popped and processed in the appropriate order
      stack = pushReverseSpaceSeparatedWords ( stack, token );

      while ( ! isEmpty ( stack ) )
      {
        // Examine the next value on top of the stack, and perform the corresponding operation
        char *topvalue = strdup ( ( char * ) top ( stack ) );
        stack = pop ( stack );

        char *temptoken = NULL, *tokenname = NULL;
        int linenumber = -1;

        // If the top of the stack is an identifier or a literal, it has
        // the prefix "<T", so use it to identify the fact that there is
        // also auxiliary data present with it.
        if ( topvalue [0] == '<' && topvalue [1] == 'T' )
        {
          extractTokenData ( topvalue, &temptoken, &tokenname, &linenumber );
          free ( topvalue );

          // temptoken was dynamically allocated memory by extractTokenData
          // since topvalue is now temptoken, temptoken will be freed with
          // when topvalue is freed below
          // We need to take care of freeing tokenname, which should be done
          // whenever topvalue is freed
          topvalue = temptoken;
        }


        TNODE *istopterminal = findString ( terminals, topvalue );

        // If the topvalue is a terminal some auxiliary operations should be performed
        // depending on which terminal it is
        if ( istopterminal != NULL )
        {
          int terminalvalue = istopterminal -> data . int_val;

          // Perform auxiliary operations depending on which terminal is
          // encountered. Call the appropriate function to do the same.

          handleAuxiliaryTerminalOperations (
              beginint, endint, idenint, intlitint, stringlitint,
              floatlitint, intint, stringint, floatint, vertexint,
              edgeint, graphint, treeint, nothingint, andint, orint,
              notint, plusint, minusint, mulint, divint, moduloint,
              gtint, ltint, gteint, lteint, eqint, bftint, dftint,
              functionint, terminalvalue, currnode, symboltable,
              & should_start_function, & function_scope_started,
              tokenname, linenumber, stbdumpfile );
        }

        // If the current node has any properties according to the instructions file, then
        // those properties override all other instructions, so they must be tested and
        // acted on first.

        TNODE *hasproperties = findString ( properties, getNodeTypeName ( currnode -> node_type ) );

        if ( hasproperties != NULL )
        {
          TRIE* trieToSearch = (TRIE *) hasproperties -> data . generic_val;
          TNODE* jumps = findString ( trieToSearch, topvalue );

          if ( jumps != NULL )
          {
            int shouldAdd = 0, shouldRead = 0;

            handleNodeProperty ( &currnode, topvalue, jumps, &stack, ast, &shouldAdd, &shouldRead );

            if ( shouldRead == 1 )
            {
              free ( topvalue );
              free ( tokenname );
              break;
            }
            else if ( shouldAdd == 1 )
            {
              stack = push ( stack, topvalue );
              if ( DEBUG_ALL ) printf ( "Pushing back %s\n", topvalue );
              free ( topvalue );
              free ( tokenname );
              continue;
            }
          }
        }

        // If the conditional flags were set in a previous iteration, process it now
        // as we have the required token to examine the condition
        if ( conditional_read == 1 || conditional_pop == 1 )
        {
          // If the previous value was a conditional read we may need to go to currnode's parent
          conditional_read = 0;
          conditional_pop = 0;

          // The conditional read may be on a terminal or on a non-terminal. Check both.

          TNODE *termval = findString ( terminals, topvalue );
          TNODE *nontermval = findString ( nonterminals, topvalue );

          if ( termval != NULL && termval -> data . int_val == conditional_value
               && conditional_type == CONDITIONAL_TERMINAL )
            currnode = getParent ( currnode );
          else if ( nontermval != NULL && nontermval -> data . int_val == conditional_value
                    && conditional_type == CONDITIONAL_NONTERMINAL )
            currnode = getParent ( currnode );
          else
            if ( DEBUG_ALL ) printf ( "Conditional read unsuccessful\n" );

          // Don't break as the topvalue should be processed even on conditional read
          // but do reset the conditional_value
          conditional_value = -1;
          conditional_type = CONDITIONAL_TYPE_FIRST;
        }

        // Conditional instructions completed (if any), continue regular processing..

        // Start regular instruction processing
        TNODE *currentval = findString ( instructions, topvalue );

        // If there are no instructions for the current value, ignore it if terminal
        // Read on non-terminals.
        if ( currentval == NULL )
        {
          if ( DEBUG_ALL ) printf ( "*No rule for %s*\n", topvalue );
          TNODE *isnonterm = findString ( nonterminals, topvalue );
          free ( topvalue );
          free ( tokenname );

          // Is a non-terminal...
          if ( isnonterm != NULL )
            break;

          continue;
        }

        // If on handling the instruction, a read status is returned
        // then we break so that the next token from the input file
        // is read
        if ( handleNodeInstruction ( astoutput, &currnode, currentval,
                                     ast, topvalue, auxdata, &conditional_read,
                                     &conditional_pop, &conditional_value,
                                     &conditional_type ) == 1 )
        {
          // We should read, as the function returned a read flag
          free ( topvalue );
          free ( tokenname );
          break;
        }
      }   // End of while for stack processing

      tokencounter = 0;
    }
    else
      token [ tokencounter++ ] = c;
  }

  if ( fclose ( stbdumpfile ) != 0 )
    fprintf ( stderr, "Failed to close STB dump file\n" );

  return ast;
}

int needsRotation ( ANODE *node )
{
  if ( node -> node_type != AST_EXP_NODE && node -> node_type != AST_AROP_NODE )
  {
    fprintf ( stderr, "Rotation checks should only be performed on EXP or AROP nodes\n" );
    return 0;
  }

  if ( node -> num_of_children != 2 || getSecondChild ( node ) -> num_of_children != 2 )
    return 0;

  ANODE *rightchild = getSecondChild ( node );

  if ( node -> node_type == AST_EXP_NODE && rightchild -> node_type == AST_EXP_NODE )
    return 0;

  // If the current node has higher precedence then the child, then rotation needs to be performed
  if ( ( node -> extra_data . arop_type == A_MUL_TYPE || node -> extra_data . arop_type == A_DIV_TYPE || node -> extra_data . arop_type == A_MODULO_TYPE )
       && ( rightchild -> extra_data . arop_type == A_PLUS_TYPE || rightchild -> extra_data . arop_type == A_MINUS_TYPE ) )
    return 1;

  return 0;
}

void insertAropBetween ( ANODE *node )
{
  LNODE *toedit = node -> children -> head -> next;
  ANODE *temp = * ( ANODE ** ) ( toedit -> data . generic_val );

  ANODE *toinsert = malloc ( sizeof ( ANODE ) );
  if ( toinsert == NULL )
  {
    fprintf ( stderr, "Failed to insert arop node in between while setting precedence\n" );
    return;
  }

  toinsert = initializeAstNode ( toinsert, node );
  toinsert = setNodeType ( toinsert, AST_AROP_NODE );

  toinsert -> line_no = node -> line_no;

  // Make the first child of the created node as temp, and the second child of node
  // as toinsert
  toinsert -> num_of_children ++;
  toinsert -> children = insertAtBack ( toinsert -> children, & temp );

  memcpy ( toedit -> data . generic_val, & toinsert, sizeof ( toinsert ) );

}

void setExpressionPrecedence ( ANODE *node )
{
  LNODE *curnode = node -> children -> head;
  LNODE iterator;
  getIterator ( node -> children, &iterator );

  while ( hasNext ( &iterator ) )
  {
    getNext ( node -> children, &iterator );
    ANODE *child = * ( ANODE ** ) ( iterator . data . generic_val );

    setExpressionPrecedence ( child );

    if ( child -> node_type == AST_EXP_NODE || child -> node_type == AST_AROP_NODE )
      if ( needsRotation ( child ) )
      {
        ANODE *newnode = rotateLeft ( child );
        memcpy ( curnode -> data . generic_val, &newnode, sizeof ( newnode ) );

        if ( newnode -> node_type == AST_AROP_NODE && getFirstChild ( newnode ) -> node_type == AST_EXP_NODE )
          newnode -> node_type = AST_EXP_NODE;

        ANODE *tocheck = getSecondChild ( getFirstChild ( newnode ) );

        if ( tocheck -> node_type == AST_LITERAL_NODE || tocheck -> node_type == AST_ASSIGNFUNC_NODE )
          insertAropBetween ( getFirstChild ( newnode ) );
      }

    curnode = curnode -> next;
  }
}

void preOrderDumpAst ( ANODE *node, FILE *astdumpfile )
{
  dumpNode ( node, astdumpfile );

  LNODE iterator;
  getIterator ( node -> children, &iterator );

  while ( hasNext ( &iterator ) )
  {
    getNext ( node -> children, &iterator );

    preOrderDumpAst ( * ( ANODE ** ) ( iterator . data . generic_val ), astdumpfile );
  }
}

int main ( )
{
  // Get the system block size
  struct stat fi;
  stat ( "/", &fi );
  int blocksize = fi.st_blksize;

  erroroccured = 0;

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
  int nodetypescount = 0;

  terminals = getNewTrie ( TRIE_INT_TYPE );
  nonterminals = getNewTrie ( TRIE_INT_TYPE );

  FILE *tmapfile = NULL, *ntmapfile = NULL;
  FILE *nodetypesfile = NULL;

  tmapfile = fopen ( T_INDEX_FILE, "rb" );
  ntmapfile = fopen ( NT_INDEX_FILE, "rb" );
  nodetypesfile = fopen ( AST_NODETYPES_FILE, "rb" );

  if ( tmapfile == NULL || ntmapfile == NULL || nodetypesfile == NULL )
  {
    fprintf ( stderr, "Failed to open (non) terminals index file / AST node types file\n" );
    return -1;
  }

  populateTrie ( tmapfile, blocksize, terminals, &terminalscount );
  populateTrie ( ntmapfile, blocksize, nonterminals, &nonterminalscount );
  populateTrie ( nodetypesfile, blocksize, ast -> node_typemap, &nodetypescount );

  if ( fclose ( tmapfile ) != 0 )
    fprintf ( stderr, "Failed to close terminals index file\n" );
  if ( fclose ( ntmapfile ) != 0 )
    fprintf ( stderr, "Failed to close non terminals index file\n" );
  if ( fclose ( nodetypesfile ) != 0 )
    fprintf ( stderr, "Failed to close AST node types file\n" );



  /*********************************************************
    *                                                      *
    *        PHASE 2 : Build Rule Tries and Mapping        *
    *                                                      *
    ********************************************************
   */

  TRIE *instructions = NULL, *auxdata = NULL;
  // This Trie of Tries stores the nodes with special properties
  // if findString ( properties, string ) exists then string has
  // a special property
  // if findString ( (TRIE*) findString ( properties, string ) -> data . generic_val, string2 )
  // exists, then then string node has a property for string2..
  TRIE *properties = NULL;

  instructions = getNewTrie ( TRIE_INT_TYPE );
  auxdata = getNewTrie ( TRIE_GENERIC_TYPE );
  auxdata = setTrieGenericSize ( auxdata, sizeof ( AUXDATA ) );
  properties = getNewTrie ( TRIE_GENERIC_TYPE );
  properties = setTrieGenericSize ( properties, sizeof ( TRIE* ) );

  FILE *instructionsfile = NULL;

  instructionsfile = fopen ( AST_INSTRUCTIONS_FILE, "rb" );

  if ( instructionsfile == NULL )
  {
    fprintf ( stderr, "Failed to open AST nodes' instructions file\n" );
    return -1;
  }

  getNodeInstructions ( instructionsfile, blocksize, instructions, auxdata, ast -> node_typemap,
                        nonterminals, terminals, properties );


  if ( fclose ( instructionsfile ) != 0 )
    fprintf ( stderr, "Failed to close instructions file\n" );

  /*********************************************************
    *                                                      *
    *    PHASE 3 : Build Final AST and use Symbol Table    *
    *                                                      *
    ********************************************************
   */

  FILE *attributesFile = NULL;
  attributesFile = fopen ( ATTRIBUTES_FILE, "rb" );

  if ( attributesFile == NULL )
  {
    fprintf ( stderr, "Failed to open attributes map file\n" );
    return -1;
  }

  int stbentries = getLineCount ( attributesFile, blocksize );

  if ( fclose ( attributesFile ) != 0 )
    fprintf ( stderr, "Failed to close attributes file\n" );

  SYMBOLTABLE *symboltable = getSymbolTable();

  symboltable = setNumEntries ( symboltable, stbentries );

  FILE *parseroutput = NULL;
  parseroutput = fopen ( PARSE_OUTPUT_FILE, "rb" );

  if ( parseroutput == NULL )
  {
    fprintf ( stderr, "Failed to open parser output file\n" );
    return -1;
  }

  FILE *astoutput = NULL;
  astoutput = fopen ( AST_OUTPUT_FILE, "w+" );

  if ( astoutput == NULL )
  {
    fprintf ( stderr, "Failed to open AST output file\n" );
    return -1;
  }

  ast = createAST ( parseroutput, blocksize, ast, instructions, auxdata,
                    nonterminals, terminals, properties, symboltable,
                    astoutput );

  if ( fclose ( parseroutput ) != 0 )
  {
    fprintf ( stderr, "Failed to close parser output file\n" );
    return -1;
  }

  if ( fclose ( astoutput ) != 0 )
  {
    fprintf ( stderr, "Failed to close AST output file\n" );
    return -1;
  }

  FILE *astdumpfile = NULL;
  astdumpfile = fopen ( AST_DUMP_FILE, "w+" );

  if ( astdumpfile == NULL )
  {
    fprintf ( stderr, "Failed to open AST dump file to write\n" );
    return -1;
  }

  ANODE *firstnode = * ( ANODE ** ) ( ast -> root -> children -> head -> data . generic_val );

  setExpressionPrecedence ( firstnode );

  preOrderDumpAst ( firstnode, astdumpfile );

  if ( fclose ( astdumpfile ) != 0 )
    fprintf ( stderr, "Failed to close AST dump file\n" );

  if ( DEBUG_ALL ) printf ( "AST successfully built\n" );

  if ( erroroccured == 1 )
    return -1;

  return 0;
}

