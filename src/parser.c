#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "headers/trie.h"
#include "headers/linkedlist.h"

#define TOKENS_FILE "TOKENS"
#define DICT_FILE "TOKENMAP"
#define T_INDEX_FILE "config/terminals_index"
#define NT_INDEX_FILE "config/nonterminals_index"
#define PTABLE_FILE "config/parse_table"
#define RULES_FILE "config/rules_file"
#define MAXRULE 200
#define BUFFERLEN 200
#define NEWLINE '\n'

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

int main ( int argc, char *argv[] )
{
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

  // TODO: Parse the input file
  // TODO: Read input file as command line argument
  
  return 0;
}

