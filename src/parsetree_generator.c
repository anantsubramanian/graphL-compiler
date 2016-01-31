#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "headers/parsetree.h"

#ifndef TRIE_DEFINED
  #include "headers/trie.h"
#endif

#ifndef CONSTANTS_DEFINED
  #include "headers/constants.h"
#endif

#ifndef PARSE_UTILS_DEFINED
  #include "headers/parse_utils.h"
#endif

#define NEWLINE '\n'

#define DEBUG_ONCREATE 0

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

PARSETREE* createParseTree ( FILE * parseroutput, int blocksize, PARSETREE *pst,
                             TRIE *terminals )
{
  PTNODE *currnode = pst -> root;

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
      token [ tokencounter ] = '\0';

      extractLineNum ( token );

      currnode = getLeftMostDesc ( currnode );

      if ( currnode == pst -> root )
        currnode = insertSpaceSeparatedWordsInPTree ( currnode, token );
      else
      {
        while ( findString ( terminals, currnode -> name ) != NULL )
        {
          // While the current node represents a terminal
          // go to the next pre-order node
          currnode = getNextPreOrder ( currnode );
        }

        if ( DEBUG_ONCREATE ) printf ( "At node %s:\n", currnode -> name );

        // Found the required non-terminal node
        // expand it using the words in token
        currnode = insertSpaceSeparatedWordsInPTree ( currnode, token );

        if ( DEBUG_ONCREATE )
        {
          printf ( "Created children: " );
          int childindx;
          for ( childindx = 0; childindx < currnode -> num_of_children; childindx++ )
            printf ( "%s ", currnode -> next [ childindx ] -> name );
          printf ( "\n" );
        }

        // Go to the appropriate node after insertion
        currnode = getLeftMostDesc ( currnode );
      }

      tokencounter = 0;
    }
    else
      token [ tokencounter++ ] = c;
  }

  return pst;
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
    *     PHASE 2 : Build PARSETREE from parser output     *
    *                                                      *
    ********************************************************
   */



  FILE *parseroutput = NULL;
  parseroutput = fopen ( PARSE_OUTPUT_FILE, "rb" );

  if ( parseroutput == NULL )
  {
    fprintf ( stderr, "Failed to open parser output file\n" );
    return -1;
  }

  PARSETREE* pst = NULL;
  pst = getNewParseTree ();

  pst = createParseTree ( parseroutput, blocksize, pst, terminals );

  if ( fclose ( parseroutput ) != 0 )
  {
    fprintf ( stderr, "Failed to close parser output file\n" );
    return -1;
  }

  printf ( "PARSETREE successfully built\n" );

  return 0;
}

