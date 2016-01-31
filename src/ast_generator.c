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
#include "headers/ast_construction.h"

#ifndef AST_DEFINED
  #include "headers/ast.h"
#endif

#ifndef SYMBOLTABLE_DEFINED
  #include "headers/symboltable.h"
#endif

#ifndef TRIE_DEFINED
  #include "headers/trie.h"
#endif

#ifndef CONSTANTS_DEFINED
  #include "headers/constants.h"
#endif

#ifndef PARSE_UTILS_DEFINED
  #include "headers/parse_utils.h"
#endif

#define AST_STACK_NAME "AST Generation Stack"

static int error_occurred;

AST* createAST (
    FILE * parseroutput, int blocksize, AST *ast, TRIE *instructions,
    TRIE *auxdata, TRIE *nonterminals, TRIE *terminals, TRIE *properties,
    SYMBOLTABLE *symboltable, FILE *astoutput )
{
  FILE *stbdumpfile = NULL;
  stbdumpfile = fopen ( STB_DUMP_FILE, "w+" );

  if ( stbdumpfile == NULL )
  {
    fprintf ( stderr, "Failed to open STB dump file\n" );
    error_occurred = 1;
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

  // Set the original node's parent as the newly created AROP node
  temp -> parent = toinsert;

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

int main ( )
{
  // Get the system block size
  struct stat fi;
  stat ( "/", &fi );
  int blocksize = fi.st_blksize;

  error_occurred = 0;

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

  if ( error_occurred == 1 || constructionErrorOccurred() == 1 )
    return -1;

  return 0;
}

