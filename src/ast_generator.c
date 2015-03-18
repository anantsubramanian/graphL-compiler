#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "headers/ast.h"

#define BUFFERLEN 400
#define INSTRLEN 5
#define NEWLINE '\n'
#define COMMENT_START '#'
#define PARSE_OUTPUT_FILE "PARSEOUTPUT"
#define AST_NODETYPES_FILE "config/ast_nodetypes"
#define AST_INSTRUCTIONS_FILE "config/ast_instructions"
#define T_INDEX_FILE "config/terminals_index"
#define NT_INDEX_FILE "config/nonterminals_index"

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
#define AST_FUNCTIONDEFINES_NODE 8
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

// End AST node types #defines

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

void getNodeInstructions ( FILE *instructionsfile, int blocksize, TRIE *instructions,
                           TRIE *auxdata, TRIE *nodetypemap, TRIE *nonterminals,
                           TRIE *terminals )
{
  char c;

  char buffers [2] [ blocksize ];
  char token [ BUFFERLEN ];
  char instr [ INSTRLEN ];
  char extradata [ BUFFERLEN ];

  int charindx = -1;
  int curbuff = -1;
  int charsread = 0;
  int toriore = 0;

  int tokencounter = 0;
  int instrcounter = 0;
  int extracounter = 0;

  int isfirst = 1;
  int incomment = 0;
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
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( isfirst == 1 && c == COMMENT_START )
      incomment = 1;

    if ( c == NEWLINE )
    {
      if ( incomment != 1 )
      {
        // Is a valid line to parse, and not a comment
        if ( toriore == 1 )
        {
          // No auxiliary data
          instr [ instrcounter ] = '\0';

          TNODE *temp = insertString ( instructions, token );
          temp -> data . int_val = createProperty ( instr );
        }
        else
        {
          // Auxiliary data is there
          extradata [ extracounter ] = '\0';

          TNODE *temp = insertString ( instructions, token );
          temp -> data . int_val = createProperty ( instr );

          temp = insertString ( auxdata, token );

          // Is the aux data a node type, a terminal or a non terminal?
          TNODE *istype = findString ( nodetypemap, extradata );
          TNODE *isterm = findString ( terminals, extradata );
          TNODE *isntrm = findString ( nonterminals, extradata );

          if ( istype != NULL )
          {
            // Was a node type
            temp -> data . int_val = istype -> data . int_val;
          }
          else if ( isterm != NULL )
          {
            // Was a terminal
            temp -> data . int_val = isterm -> data . int_val;
          }
          else if ( isntrm != NULL )
          {
            // Was a non terminal
            temp -> data . int_val = isntrm -> data . int_val;
          }
          else
          {
            fprintf ( stderr, "Invalid auxiliary data in the instructions file\n" );
            exit (-1);
          }
        }
        printf ( "%s %s %d %s\n", token, instr, createProperty ( instr ), extradata );
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
        toriore = 1;
      }
      else if ( toriore == 1 )
      {
        instr [ instrcounter ] = '\0';
        extracounter = 0;
        toriore = 2;
      }
    }
    else if ( toriore == 0 )
      token [ tokencounter++ ] = c, prevspace = 0, isfirst = 0;
    else if ( toriore == 1 )
      instr [ instrcounter++ ] = c, prevspace = 0, isfirst = 0;
    else if ( toriore == 2 )
      extradata [ extracounter++ ] = c, prevspace = 0, isfirst = 0;
  }

}

AST* createAST ( FILE * parseroutput, int blocksize, AST *ast, TRIE *auxdata,
                 TRIE *nonterminals, TRIE *terminals )
{
  ANODE *currnode = ast -> root;

  char c;

  char buffers [2] [blocksize];
  char token [ BUFFERLEN ];

  int charindx = -1;
  int curbuff = -1;
  int charsread = 0;
  int tokencounter = 0;

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
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == NEWLINE )
    {

    }
    else
      token [ tokencounter++ ] = c;
  }

  return ast;
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

  instructions = getNewTrie ( TRIE_INT_TYPE );
  auxdata = getNewTrie ( TRIE_INT_TYPE );

  FILE *instructionsfile = NULL;

  instructionsfile = fopen ( AST_INSTRUCTIONS_FILE, "rb" );

  if ( instructionsfile == NULL )
  {
    fprintf ( stderr, "Failed to open AST nodes' instructions file\n" );
    return -1;
  }

  getNodeInstructions ( instructionsfile, blocksize, instructions, auxdata, ast -> node_typemap,
                        nonterminals, terminals );

  FILE *parseroutput = NULL;
  parseroutput = fopen ( PARSE_OUTPUT_FILE, "rb" );

  if ( parseroutput == NULL )
  {
    fprintf ( stderr, "Failed to open parser output file\n" );
    return -1;
  }

  ast = createAST ( parseroutput, blocksize, ast, auxdata, nonterminals, terminals );

  if ( fclose ( parseroutput ) != 0 )
  {
    fprintf ( stderr, "Failed to close parser output file\n" );
    return -1;
  }

  printf ( "AST successfully built\n" );

  return 0;
}

