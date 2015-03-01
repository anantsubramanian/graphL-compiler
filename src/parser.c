#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "headers/trie.h"
#include "headers/stack.h"

#define TOKENS_FILE "TOKENS"
#define DICT_FILE "TOKENMAP"
#define T_INDEX_FILE "config/terminals_index"
#define NT_INDEX_FILE "config/nonterminals_index"
#define PTABLE_FILE "config/parse_table"
#define RULES_FILE "config/rules_file"
#define START_SYMBOL "<program>"
#define MAXLINE 500
#define MAXRULE 200
#define BUFFERLEN 200
#define NO_TRANSITION -1
#define NEWLINE '\n'
#define COMMA ','

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
      temp -> value = value;
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
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == NEWLINE )
      lines++;
  }

  return lines;
}

char* getLine ( FILE *inputfile, int blocksize, int linenumber )
{
  char c;

  int curbuff = -1;
  int charindx = -1;
  int lines = 1;
  int charsread = 0;
  char buffers [2] [blocksize];

  char token [ MAXLINE ];
  int tokenindx = 0;

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
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == NEWLINE )
    {
      if ( lines == linenumber )
      {
        token [ tokenindx ] = '\0';
        return strdup ( token );
      }

      lines++;
      tokenindx = 0;
    }
    else
      token [ tokenindx++ ] = c;

  }

  fprintf ( stderr, "Unable to retrieve requested line\n" );
  exit (-1);

  return NULL;
}

void populateGrammarRules ( FILE *rulesfile, int blocksize, LINKEDLIST* ruleLists [] )
{
  char c;

  char buffers [2] [blocksize];
  char token [ MAXRULE ];

  int curbuff = -1;
  int charindx = -1;
  int charsread = 0;
  int tokenindex = 0;

  int value = 0;
  int torval = 1;

  while ( TRUE )
  {
    // Read next character from the buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, rulesfile ) ) == 0 )
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
      token [ tokenindex ] = '\0';
      ruleLists [value] = getLinkedList ();
      ruleLists [value] = insertSpaceSeparatedWords ( ruleLists [value], token );
      value = 0;
      torval = 1;
    }
    else if ( c == ' ' && torval == 1 )
    {
      torval = 0;
      tokenindex = 0;
    }
    else if ( torval == 1 )
      value = value * 10 + c - 48;
    else
      token [ tokenindex++ ] = c;
  }
}

void populateParseTable ( FILE *ptablefile, int blocksize, int **parseTable )
{
  char c;

  char buffers [2] [blocksize];

  int curbuff = -1;
  int charindx = -1;
  int charsread = 0;
  int currow = 0;
  int curcol = 0;

  int value = 0;
  int negative = 0;

  while ( TRUE )
  {
    // Read next character from the buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, ptablefile ) ) == 0 )
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
      currow++;
      curcol = 0;
      value = 0;
      negative = 0;
    }
    else if ( c == ' ' )
    {
      if ( negative == TRUE )
        value = -value;
      parseTable [currow] [curcol] = value;

      curcol++;
      value = 0;
      negative = 0;
    }
    else if ( c == '-' )
      negative = TRUE;
    else
      value = value * 10 + c - 48;
  }
}

void populateAttributes ( FILE *ptablefile, int blocksize, char attributes [] [MAXRULE] )
{
  char c;

  char buffers [2] [blocksize];
  char token [ MAXRULE ];

  int curbuff = -1;
  int charindx = -1;
  int charsread = 0;
  int tokenindex = 0;

  int torval = 0;
  int value = 0;

  while ( TRUE )
  {
    // Read next character from the buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, ptablefile ) ) == 0 )
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
      strcpy ( attributes [ value ], token );
      tokenindex = 0;
      torval = 0;
    }
    else if ( c == ' ' )
    {
      token [ tokenindex ] = '\0';
      value = 0;
      torval = 1;
    }
    else if ( torval == 0 )
      token [ tokenindex++ ] = c;
    else
      value = value * 10 + c - 48;
  }
}

void parseInputProgram ( FILE *inputfile, int blocksize, int **parseTable,
                         TRIE* terminals, TRIE* nonterminals, LINKEDLIST* ruleLists [],
                         char *inputprogram )
{

  STACK *stack = NULL;
  stack = getStack ();

  stack = push ( stack, START_SYMBOL );

  char c;

  char buffers [2] [blocksize];
  char token [ MAXRULE ];

  int curbuff = -1;
  int charindx = -1;
  int charsread = 0;
  int tokenindex = 0;

  int epscolumn = findString ( terminals, "e" ) -> value;

  int value = 0;
  int linenum = 0;
  int torvalorlno = 0;

  while ( TRUE )
  {
    // Read next character from the buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, inputfile ) ) == 0 )
        break;
    }
    c = buffers [ curbuff ] [ charindx ];

    if ( charsread < blocksize && charindx >= charsread )
    {
      // Input token stream has ended, stack should be empty / should be nullable
      if ( ! isEmpty ( stack ) )
      {
        while ( ! isEmpty ( stack ) )
        {
          char *topval = strdup ( top ( stack ) );
          stack = pop ( stack );

          TNODE *nonterm = findString ( nonterminals, topval );
          if ( findString ( terminals, topval ) != NULL || nonterm == NULL )
          {
            printf ( "Error at line %d:\n\tUnexpected end to input program.\n", linenum );
            exit (-1);
          }

          int nontermval = nonterm -> value;
          if ( parseTable [ nontermval ] [ epscolumn ] == NO_TRANSITION )
          {
            printf ( "Error at line %d:\n\tUnexpected end to input program.\n", linenum );
            exit (-1);
          }
        }
      }
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == NEWLINE )
    {
      TNODE *tomatch = findString ( terminals, token + 1 );
      if ( tomatch == NULL )
      {
        printf ( "FATAL ERROR: Unrecognized input token %s\nConsider \
                 re-compiling the lexer module\n", token + 1 );
        exit (-1 );
      }

      int column = tomatch -> value;

      while ( TRUE )
      {
        if ( isEmpty ( stack ) )
        {
          FILE* programfile = NULL;
          programfile = fopen ( inputprogram, "rb" );

          if ( programfile == NULL )
          {
            fprintf ( stderr, "Failed to open input program to print errors\n" );
            exit (-1);
          }

          printf ( "Error at line %d: %s\n\tTrailing characters at the end of \
                    the program\n", linenum, getLine ( programfile, blocksize, linenum ) );

          if ( fclose ( programfile ) != 0 )
            fprintf ( stderr, "Failed to close input program used to display errors\n" );

          exit (-1);
        }

        char *topval = strdup ( top ( stack ) );

        stack = pop ( stack );

        TNODE *current = findString ( nonterminals, topval );

        if ( current != NULL )
        {
          // Top of stack is a non-terminal
          int nontermindex = current -> value;

          if ( parseTable [ nontermindex ] [ column ] != NO_TRANSITION )
          {
            LNODE *iter = getIterator ( ruleLists [ parseTable [ nontermindex ] [ column ] ] );
            stack = insertFromLinkedList ( stack, ruleLists [ parseTable [ nontermindex ] [ column ] ] );
          }
          else if ( parseTable [ nontermindex ] [ epscolumn ] != NO_TRANSITION )
            continue;
          else
          {
            FILE* programfile = NULL;
            programfile = fopen ( inputprogram, "rb" );

            if ( programfile == NULL )
            {
              fprintf ( stderr, "Failed to open input program to print errors\n" );
              exit (-1);
            }

            printf ( "Error at line %d: %s\n\tUnexpected token %s encountered\n",
                      linenum, getLine ( programfile, blocksize, linenum ), token + 1 );

            if ( fclose ( programfile ) != 0 )
              fprintf ( stderr, "Failed to close input program used to display errors\n" );

            exit (-1);
          }
        }
        else
        {
          // Top of the stack is a terminal
          current = findString ( terminals, topval );

          if ( current == NULL )
          {
            printf ( "FATAL ERROR: Unrecognized terminal %s encountered \
                      while parsing.\nConsider re-checking grammar rules.\n" );
            exit (-1);
          }
          int stackterminal = current -> value;

          if ( stackterminal != column )
          {
            FILE* programfile = NULL;
            programfile = fopen ( inputprogram, "rb" );

            if ( programfile == NULL )
            {
              fprintf ( stderr, "Failed to open input program to print errors\n" );
              exit (-1);
            }

            printf ( "Error at line %d: %s\n\tExpected %s, but got %s\n",
                      linenum, getLine ( programfile, blocksize, linenum ),
                      topval, token + 1 );

            if ( fclose ( programfile ) != 0 )
              fprintf ( stderr, "Failed to close input program used to display errors\n" );

            exit (-1);
          }
          else
            break;    // Found the required terminal, so exit the while loop
        }
      }

      tokenindex = 0;
      torvalorlno = 0;
    }
    else if ( c == COMMA )
    {
      token [ tokenindex ] = '\0';

      if ( torvalorlno == 0 )
        value = 0;
      else
        linenum = 0;

      torvalorlno++;
    }
    else if ( c == '>' )
    {
      if ( torvalorlno == 1 )
      {
        linenum = value;
        value = -1;
      }
    }
    else if ( torvalorlno == 0 )
      token [ tokenindex++ ] = c;
    else if ( torvalorlno == 1 )
      value = value * 10 + c - 48;
    else if ( c != '>' )
      linenum = linenum * 10 + c - 48;
  }
}

int main ( int argc, char *argv[] )
{

  if ( argc <= 1 )
  {
    fprintf ( stderr, "Please provide the input program as an argument\n" );
    return -1;
  }

  // Get the system block size
  struct stat fi;
  stat ( "/", &fi );
  int blocksize = fi.st_blksize;

  TRIE* terminals = NULL, *nonterminals = NULL;
  int terminalscount = 0, nonterminalscount = 0;

  terminals = getNewTrie ();
  nonterminals = getNewTrie ();

  FILE *tmapfile = NULL, *ntmapfile = NULL;



  /***********************************************************
    *                                                        *
    *   PHASE 1 : Populate terminal and non-terminal tries   *
    *                                                        *
    **********************************************************
   */



  tmapfile = fopen ( T_INDEX_FILE, "rb" );
  ntmapfile = fopen ( NT_INDEX_FILE, "rb" );

  if ( tmapfile == NULL || ntmapfile == NULL )
  {
    fprintf ( stderr, "Failed to open (non)terminals index file\n" );
    return -1;
  }

  populateTrie ( tmapfile, blocksize, terminals, &terminalscount );
  populateTrie ( ntmapfile, blocksize, nonterminals, &nonterminalscount );

  // Convert indices to limits
  terminalscount++;
  nonterminalscount++;

  if ( fclose ( tmapfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close terminals map file\n" );
    return -1;
  }
  if ( fclose ( ntmapfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close non-terminals map file\n" );
    return -1;
  }
  tmapfile = ntmapfile = NULL;



  /***********************************************************
    *                                                        *
    *       PHASE 2 : Populate linked list of rules          *
    *                                                        *
    **********************************************************
   */



  FILE *rulesfile = NULL;
  rulesfile = fopen ( RULES_FILE, "rb" );

  if ( rulesfile == NULL )
  {
    fprintf ( stderr, "Failed to open rules file\n" );
    return -1;
  }

  int linecount = getLineCount ( rulesfile, blocksize );

  if ( fclose ( rulesfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close rules file\n" );
    return -1;
  }
  rulesfile = NULL;

  LINKEDLIST* ruleLists [linecount];
  int i;
  for ( i = 0; i < linecount; i++ )
    ruleLists [i] = NULL;

  rulesfile = fopen ( RULES_FILE, "rb" );

  if ( rulesfile == NULL )
  {
    fprintf ( stderr, "Failed to open rules file\n" );
    return -1;
  }

  populateGrammarRules ( rulesfile, blocksize, ruleLists );

  if ( fclose ( rulesfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close rules file\n" );
    return -1;
  }



  /***********************************************************
    *                                                        *
    *            PHASE 3 : Populate Parse Table              *
    *                                                        *
    **********************************************************
   */


  int **parseTable = NULL;

  parseTable = malloc ( nonterminalscount * sizeof ( int * ) );
  if ( parseTable == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for parse table\n" );
    return -1;
  }

  for ( i = 0; i < nonterminalscount; i++ )
  {
    parseTable [i] = NULL;
    parseTable [i] = malloc ( terminalscount * sizeof ( int ) );

    if ( parseTable [i] == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for row of parse table\n" );
      return -1;
    }
  }

  FILE *parsetablefile = NULL;
  parsetablefile = fopen ( PTABLE_FILE, "rb" );

  if ( parsetablefile == NULL )
  {
    fprintf ( stderr, "Failed to open parse table file\n" );
    return -1;
  }

  populateParseTable ( parsetablefile, blocksize, parseTable );

  if ( fclose ( parsetablefile ) != 0 )
  {
    fprintf ( stderr, "Failed to close parse table file\n" );
    return -1;
  }



  /***********************************************************
    *                                                        *
    *               PHASE 4 : Parse Input File               *
    *                                                        *
    **********************************************************
   */



  FILE *inputfile = NULL, *attributefile = NULL;
  attributefile = fopen ( DICT_FILE, "rb" );

  if ( attributefile == NULL )
  {
    fprintf ( stderr, "Failed to open attribute map file\n" );
    return -1;
  }

  int attributelines = getLineCount ( attributefile, blocksize );

  if ( fclose ( attributefile ) != 0 )
  {
    fprintf ( stderr, "Failed to close attributes file\n" );
    return -1;
  }
  attributefile = NULL;

  char attributes [ attributelines ] [ MAXRULE ];

  attributefile = fopen ( DICT_FILE, "rb" );

  if ( attributefile == NULL )
  {
    fprintf ( stderr, "Failed to open attribute map file\n" );
    return -1;
  }

  populateAttributes ( attributefile, blocksize, attributes );

  if ( fclose ( attributefile ) != 0 )
  {
    fprintf ( stderr, "Failed to close attributes file\n" );
    return -1;
  }

  inputfile = fopen ( TOKENS_FILE, "rb" );
  if ( inputfile == NULL )
  {
    fprintf ( stderr, "Failed to open program to parse\n" );
    return -1;
  }

  parseInputProgram ( inputfile, blocksize, parseTable, terminals,
                      nonterminals, ruleLists, argv [1] );

  printf ( "Parsing completed successfully\n" );

  return 0;
}

