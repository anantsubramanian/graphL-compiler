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

#define BUFFERLEN 400
#define NEWLINE '\n'
#define STB_DUMP_FILE "STBDUMP"
#define AST_DUMP_FILE "ASTDUMP"
#define T_INDEX_FILE "config/terminals_index"
#define NT_INDEX_FILE "config/nonterminals_index"
#define ROOTNODENAME "AST_ROOT_NODE"

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
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

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


void analyzeAst ( AST *ast, SYMBOLTABLE *symboltable, FILE *stbdumpfile )
{

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

  unsigned int num_entries = 0;

  fscanf ( stbdumpfile, "%u", & num_entries );

  SYMBOLTABLE *symboltable = getSymbolTable ();

  symboltable = setNumEntries ( symboltable, num_entries );

  analyzeAst ( ast, symboltable, stbdumpfile );

  if ( fclose ( stbdumpfile ) != 0 )
    fprintf ( stderr, "Failed to close Symbol Table dump file\n" );

  return 0;
}

