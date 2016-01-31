// Authors: Anant Subramanian <anant.subramanian15@gmail.com>
//          Aditya Bansal <adityabansal_adi@yahoo.co.in>
//
// BITS PILANI ID NOs: 2012A7TS010P
//                     2012A7PS122P
//
// Project Team Num: 1
// Project Group No. 1

#include "parse_utils.h"

/**
 * Compute the number of lines in a file
 *
 * @param inputfile FILE* input file pointer
 * @param blocksize int The block size to use while reading the file
 *
 * @return int The number of lines in the file
 */

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

/**
 * Populates the given Trie with the given mapfile (see config files for
 * syntax), and the count of unique mappings found.
 *
 * @param mapfile FILE* the map file pointer
 * @param blocksize int the block size to use while reading the file
 * @param trie TRIE* the Trie DS to populate
 * @param count int* the ouput count variable
 */

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

/**
 * Reads a given line number of a file, allocates memory for a char[]
 * and returns the pointer to the created array.
 * Note: The onus is on the user to free the allocated memory
 *
 * @param inputfile FILE* the input file
 * @param blocksize int the bloksize to use while reading the file
 * @param linenumber int the line number to fetch
 *
 * @return char* The read line
 */

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

