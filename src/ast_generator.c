#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "headers/ast.h"

#define BUFFERLEN 400
#define INSTRLEN 5
#define DIGSTART 48
#define DIGEND 57
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

void printNodeType ( int type )
{
  switch ( type )
  {
    case 0: printf ( "AST_PROGRAM_NODE\n" );
            break;
    case 1: printf ( "AST_GLOBALDEFINES_NODE\n" );
            break;
    case 2: printf ( "AST_GLOBALDEFINE_NODE\n" );
            break;
    case 3: printf ( "AST_DEFINE_NODE\n" );
            break;
    case 4: printf ( "AST_LET_NODE\n" );
            break;
    case 5: printf ( "AST_ASSIGNABLE_NODE\n" );
            break;
    case 6: printf ( "AST_DATATYPE_NODE\n" );
            break;
    case 7: printf ( "AST_IDENTIFIER_NODE\n" );
            break;
    case 8: printf ( "AST_FUNCTIONDEFINES_NODE\n" );
            break;
    case 9: printf ( "AST_FUNCBODY_NODE\n" );
            break;
    case 10: printf ( "AST_QUALIFIEDPARAMETERS_NODE\n" );
            break;
    case 11: printf ( "AST_QUALIFIEDPARAMETER_NODE\n" );
            break;
    case 12: printf ( "AST_RETURNTYPE_NODE\n" );
            break;
    case 13: printf ( "AST_READ_NODE\n" );
            break;
    case 14: printf ( "AST_PRINT_NODE\n" );
            break;
    case 15: printf ( "AST_COMPARE_NODE\n" );
            break;
    case 16: printf ( "AST_BOOLEXP_NODE\n" );
            break;
    case 17: printf ( "AST_EXP_NODE\n" );
            break;
    case 18: printf ( "AST_PASSEDPARAMS_NODE\n" );
            break;
    case 19: printf ( "AST_RETURNSTMT_NODE\n" );
            break;
    case 20: printf ( "AST_FUNCTIONCALL_NODE\n" );
            break;
    case 21: printf ( "AST_IF_NODE\n" );
            break;
    case 22: printf ( "AST_BLOCK_NODE\n" );
            break;
    case 23: printf ( "AST_FOR_NODE\n" );
            break;
    case 24: printf ( "AST_BDFT_NODE\n" );
            break;
    case 25: printf ( "AST_EDGECREATE_NODE\n" );
            break;
    case 26: printf ( "AST_BOOLOP_NODE\n" );
            break;
    case 27: printf ( "AST_AROP_NODE\n" );
            break;
    case 28: printf ( "AST_FORIN_NODE\n" );
            break;
    case 29: printf ( "AST_ADJTO_NODE\n" );
            break;
  }
}

void extractTokenData ( char *inputtoken, char **token, char **name, int *linenumber )
{
  if ( inputtoken == NULL )
  {
    fprintf ( stderr, "Cannot extract non-existent token data\n" );
    return;
  }

  int len = strlen ( inputtoken + 1 );

  *token = malloc ( len * sizeof ( char ) );
  *name = malloc ( len * sizeof ( char ) );
  int i;

  // Get token type

  while ( i < len && inputtoken [i] != ',' ) i++;

  if ( i == len )
  {
    fprintf ( stderr, "No auxiliary data seems to be present?\n" );
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

AST* createAST ( FILE * parseroutput, int blocksize, AST *ast, TRIE *instructions,
                 TRIE *auxdata, TRIE *nonterminals, TRIE *terminals )
{
  // We start processing from the root node
  ANODE *currnode = ast -> root;

  STACK *stack = NULL;
  stack = getStack ( STACK_STRING_TYPE );

  char c;

  char buffers [2] [blocksize];
  char token [ BUFFERLEN ];

  int charindx = -1;
  int curbuff = -1;
  int charsread = 0;
  int tokencounter = 0;

  int conditional_read = 0;
  int conditional_value = -1;

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
      token [ tokencounter ] = '\0';

      // A whole line of expansion has been read. Need to process
      stack = pushReverseSpaceSeparatedWords ( stack, token );

      while ( ! isEmpty ( stack ) )
      {
        // Examine the next value on top of the stack, and perform the corresponding operation
        char *topvalue = strdup ( ( char * ) top ( stack ) );
        stack = pop ( stack );

        char *temptoken = NULL, *tokenname = NULL;
        int linenumber = -1;
        int hasLineNumber = 0;

        // TODO: Anyway to make this jugaad cleaner?
        if ( topvalue [0] == '<' && topvalue [1] == 'T' )
        {
          hasLineNumber = 1;
          extractTokenData ( topvalue, &temptoken, &tokenname, &linenumber );
          free ( topvalue );
          topvalue = temptoken;
        }


        if ( conditional_read == 1 )
        {
          // If the previous value was a conditional read we may need to go to currnode's parent
          conditional_read = 0;

          // The conditional read may be on a terminal or on a non-terminal. Check both.
          // Simplifying assumption - the IDs for the non-terminals and terminals are unique.
          // TODO: Store extra data in the auxiliary Trie to differentiate
          // between terminals and non-terminals.
          TNODE *termval = findString ( terminals, topvalue );
          TNODE *nontermval = findString ( nonterminals, topvalue );

          if ( termval != NULL && termval -> data . int_val == conditional_value )
            currnode = getParent ( currnode );
          else if ( nontermval != NULL && nontermval -> data . int_val == conditional_value )
            currnode = getParent ( currnode );

          // Don't break as the topvalue should be processed even on conditional read
          // but do reset the conditional_value
          conditional_value = -1;
        }

        TNODE *currentval = findString ( instructions, topvalue );

        // TODO: Add code to perform special checks for scoping,
        // entering names and performing symbol table look-ups,
        // assigning data type for data type nodes, etc.

        // If there are no instructions for the current value, ignore it.
        if ( currentval == NULL )
        {
          printf ( "*Ignoring %s*\n", topvalue );
          free ( topvalue );
          continue;
        }

        int instruction = currentval -> data . int_val;

        if ( (instruction & CREATE) == CREATE )
        {
          // If we have to create a node, examine the type of node to be created
          TNODE *typeofnode = findString ( auxdata, topvalue );

          if ( typeofnode == NULL )
          {
            fprintf ( stderr, "Node instruction is create, but type to create not found\n" );
            return NULL;
          }

          int type_to_create = typeofnode -> data . int_val;

          printf ( "At node " );
          printNodeType ( currnode -> node_type );
          currnode = addChild ( currnode, type_to_create, instruction );

          printf ( "Got %s so creating: ", topvalue );
          printNodeType ( type_to_create );
          printf ( "\n\n" );

          // currnode is now either the same node, child, or parent depending on
          // the instruction. If the instruction says read, then we break.
          if ( (instruction & READ) == READ )
          {
            free ( topvalue );
            break;
          }
        }
        else if ( (instruction & GOTOCH) == GOTOCH )
        {
          fprintf ( stderr, "Instruction says goto child, but no child was created. So which child?\n" );
          return NULL;
        }
        else if ( (instruction & PARENT) == PARENT )
        {
          printf ( "At node " );
          printNodeType ( currnode -> node_type );
          printf ( "Got %s so ", topvalue );
          printf ( "Going to parent\n" );
          currnode = getParent ( currnode );

          // If the instruction says read, we should break
          if ( (instruction & READ) == READ )
          {
            free ( topvalue );
            break;
          }
        }
        else if ( (instruction & CONDRD) == CONDRD )
        {
          // Instruction is a conditional read
          // Set the conditional read flag and value so that
          // currnode will be changed to parent on that value
          conditional_read = 1;

          TNODE *cond_value = findString ( auxdata, topvalue );
          if ( cond_value == NULL )
          {
            fprintf ( stderr, "Conditional read & goto parent has no auxiliary data\n" );
            return NULL;
          }

          conditional_value = cond_value -> data . int_val;

          // Break so a read will occur
          free ( topvalue );
          break;
        }
        else if ( (instruction & READ) == READ )
        {
          printf ( "At node " );
          printNodeType ( currnode -> node_type );
          printf ( "Got %s so ", topvalue );
          printf ( "Reading next input...\n" );
          free ( topvalue );
          break;
        }
      }   // End of while

      tokencounter = 0;
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

  ast = createAST ( parseroutput, blocksize, ast, instructions, auxdata, nonterminals, terminals );

  if ( fclose ( parseroutput ) != 0 )
  {
    fprintf ( stderr, "Failed to close parser output file\n" );
    return -1;
  }

  printf ( "AST successfully built\n" );

  return 0;
}

