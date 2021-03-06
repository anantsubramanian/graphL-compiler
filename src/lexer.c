#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "headers/dfa.h"
#include "headers/trie.h"
#include "headers/constants.h"

#define DICTNAME "Token Dictionary"
#define BUFFERLEN 200
#define ERRORS 15
#define NEWLINE '\n'

int main ( int argc, char *argv[] )
{
  if ( argc <= 1 )
  {
    fprintf ( stderr, "Please provide the file path as a command line argument\n" );
    return -1;
  }

  // Get the system block size
  struct stat fi;
  stat ( "/", &fi );
  int blocksize = fi.st_blksize;

  DFA *dfa = NULL;
  dfa = getNewDFA();
  dfa = initializeFromFile ( dfa, DFA_PATH );

  TRIE* dictionary = NULL;
  dictionary = getNewTrie( TRIE_INT_TYPE );
  dictionary = setTrieName ( dictionary, DICTNAME );
  int tokenid = 0;

  int started = FALSE;

  char buffers [2] [ blocksize ];
  int curbuff = -1;
  int charindx = -1;
  int charsread = 0;

  char c;
  char errorc;
  char stringliteral [ BUFFERLEN + 1 ];
  char identifier [ BUFFERLEN ];
  char floatorint [ BUFFERLEN ];

  int idenindex = 0;
  int stringlitindex = 0;
  int floatintindex = 0;

  int linenumber = 1;
  int errorcount = 0;

  FILE *inputfile;
  FILE *errorsfile, *tokensfile, *tokenmapfile;

  inputfile = fopen ( argv[1] , "rb" );
  errorsfile = fopen ( ERRORS_FILE, "w+" );
  tokenmapfile = fopen ( DICT_FILE, "w+" );
  tokensfile = fopen ( TOKENS_FILE, "w+" );

  if ( inputfile == NULL )
  {
    fprintf ( stderr, "Fatal error! Unable to locate/open input file\n" );
    return -1;
  }

  if ( errorsfile == NULL || tokensfile == NULL || tokenmapfile == NULL )
  {
    fprintf ( stderr, "Failed to open errors/tokens/tokenmap file\n" );
    return -1;
  }

  while ( TRUE )
  {
    errorc = c;

    // Get char from appropriate buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( (charsread = fread ( buffers [ curbuff ], sizeof ( char ),
                                blocksize, inputfile ) ) == 0 )
        break;
    }
    c = buffers [ curbuff ] [ charindx ];

    if ( charsread < blocksize && charindx >= charsread )
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == NEWLINE )
      linenumber++;

    if ( floatintindex >= BUFFERLEN )
    {
      floatorint [ BUFFERLEN - 1 ] = '\0';
      fprintf ( errorsfile, "Line %d: %s\n\t\t%s\n\n", linenumber, floatorint,
                "Maximum integer literal length exceeded\n" );
      floatintindex = 0;
      errorcount++;
    }

    if ( idenindex >= BUFFERLEN )
    {
      identifier [ BUFFERLEN - 1 ] = '\0';
      fprintf ( errorsfile, "Line %d: %s\n\t\t%s\n\n", linenumber, identifier,
                "Maximum identifier length exceeded\n" );
      idenindex = 0;
      errorcount++;
    }

    if ( stringlitindex >= BUFFERLEN )
    {
      stringliteral [ BUFFERLEN ] = '\0';
      fprintf ( errorsfile, "Line %d: %s\n\t\t%s\n\n", linenumber, stringliteral,
                "Maximum string literal length exceeded\n" );
      stringlitindex  = 0;
      errorcount++;
    }

    if ( peek ( dfa, c ) == NULL || getSpecialProperty ( peek ( dfa, c ) ) == TRAP )
    {
      // Token over / error encountered so should process intermediate result
      if ( isFinal ( getCurrentState (dfa) ) == TRUE )
      {
        if ( getSpecialProperty ( getCurrentState (dfa) ) == ERROR )
        {
          fprintf ( errorsfile, "Line %d: ", errorc == NEWLINE ? linenumber - 1 : linenumber );
          if ( floatintindex != 0 )
          {
            fprintf ( errorsfile, "%s%c\n", floatorint, errorc <= 32 ? ' ' : errorc );
            floatintindex = 0;
            floatorint [ 0 ] = '\0';
          }
          else if ( idenindex != 0 )
            fprintf ( errorsfile, "%s%c\n", identifier, errorc <= 32 ? ' ' : errorc );
          else
          {
            int len = strlen ( stringliteral );
            stringliteral [ len - 1 ] = '\0';
            fprintf ( errorsfile, "%s%c\n", stringliteral, errorc <= 32 ? ' ' : errorc );
            stringlitindex = 0;
          }

          fprintf ( errorsfile, "\t\t%s\n\n", getCurrentState (dfa) -> name );
          errorcount++;

          if ( errorcount >= ERRORS )
            break;
        }
        else if ( strcmp ( getCurrentState (dfa) -> name , "TK_INTLIT" ) == 0 )
        {
          TNODE* find = findString ( dictionary, floatorint );
          int valueToPrint = tokenid;
          if ( find == NULL )
          {
            find = insertString ( dictionary, floatorint );
            fprintf ( tokenmapfile, "%d %s\n", tokenid, floatorint );
            setValue ( dictionary, find, &(tokenid) );
            tokenid++;
          }
          else
            valueToPrint = find -> data.int_val;
          fprintf ( tokensfile, "<TK_INTLIT,%d,%d>\n", valueToPrint,
                    c == NEWLINE ? linenumber - 1 : linenumber );
        }
        else if ( strcmp ( getCurrentState (dfa) -> name , "TK_FLOATLIT" ) == 0 )
        {
          TNODE* find = findString ( dictionary, floatorint );
          int valueToPrint = tokenid;
          if ( find == NULL )
          {
            find = insertString ( dictionary, floatorint );
            fprintf ( tokenmapfile, "%d %s\n", tokenid, floatorint );
            setValue ( dictionary, find, &(tokenid) );
            tokenid++;
          }
          else
            valueToPrint = find -> data.int_val;
          fprintf ( tokensfile, "<TK_FLOATLIT,%d,%d>\n", valueToPrint,
                    c == NEWLINE ? linenumber - 1 : linenumber );
        }
        else if ( strcmp ( getCurrentState (dfa) -> name , "TK_STRINGLIT" ) == 0 )
        {
          TNODE* find = findString ( dictionary, stringliteral );
          int valueToPrint = tokenid;
          if ( find == NULL )
          {
            find = insertString ( dictionary, stringliteral );
            fprintf ( tokenmapfile, "%d %s\n", tokenid, stringliteral );
            setValue ( dictionary, find, &(tokenid) );
            tokenid++;
          }
          else
            valueToPrint = find -> data.int_val;
          fprintf ( tokensfile, "<TK_STRINGLIT,%d,%d>\n", valueToPrint,
                    c == NEWLINE ? linenumber - 1 : linenumber );
        }
        else if ( strcmp ( getCurrentState (dfa) -> name , "TK_IDEN" ) == 0 )
        {
          TNODE* find = findString ( dictionary, identifier );
          int valueToPrint = tokenid;
          if ( find == NULL )
          {
            find = insertString ( dictionary, identifier );
            fprintf ( tokenmapfile, "%d %s\n", tokenid, identifier );
            setValue ( dictionary, find, &(tokenid) );
            tokenid++;
          }
          else
            valueToPrint = find -> data.int_val;
          fprintf ( tokensfile, "<TK_IDEN,%d,%d>\n", valueToPrint,
                    c == NEWLINE ? linenumber - 1 : linenumber );
        }
        else
          fprintf ( tokensfile, "<%s,%d>\n", getCurrentState (dfa) -> name,
                    c == NEWLINE ? linenumber - 1 : linenumber );
      }

      // If there is no further transition, start from the initial state again
      if ( peek ( dfa, c ) == NULL )
        dfa = gotoInitialState ( dfa );
    }

    gotoNextState ( dfa, c );

    if ( getCurrentState (dfa) != NULL && getCurrentState (dfa) -> name != NULL )
    {
      if ( strcmp ( getCurrentState (dfa) -> name , "Decimal Point" ) == 0 )
      {
        floatorint [ floatintindex++ ] = '.';
        floatorint [ floatintindex ] = '\0';
      }
      else if ( strcmp ( getCurrentState (dfa) -> name, "TK_FLOATLIT" ) == 0 )
      {
        floatorint [ floatintindex++ ] = c;
        floatorint [ floatintindex ] = '\0';
      }
      else if ( strcmp ( getCurrentState (dfa) -> name , "TK_INTLIT" ) == 0 )
      {
        floatorint [ floatintindex++ ] = c;
        floatorint [ floatintindex ] = '\0';
      }
      else if ( getSpecialProperty ( getCurrentState (dfa) ) != ERROR )
      {
        floatorint [ floatintindex ] = '\0';
        floatintindex = 0;
      }

      if ( strcmp ( getCurrentState (dfa) -> name , "Started Quote" ) == 0 )
      {
        stringliteral [ stringlitindex ++ ] = c;
        stringliteral [ stringlitindex ] = '\0';
      }
      else
      {
        stringliteral [ stringlitindex ] = '"';
        stringliteral [ stringlitindex + 1 ] = '\0';
        stringlitindex = 0;
      }

      if ( strcmp ( getCurrentState (dfa) -> name , "TK_IDEN" ) == 0 )
      {
        started = TRUE;
        identifier [ idenindex ++ ] = c;
        identifier [ idenindex ] = '\0';
      }
      else if ( ( isalpha( c ) || isdigit( c )  || c == '_' ) && started )
      {
        identifier [ idenindex ++ ] = c;
        identifier [ idenindex ] = '\0';
      }
      else if ( getSpecialProperty ( getCurrentState (dfa) ) != ERROR )
      {
        started = FALSE;
        idenindex = 0;
      }
    }
    else
    {
      started = FALSE;
      idenindex = 0;
    }

  }

  if ( fclose ( inputfile ) != 0 )
    fprintf ( stderr, "Error while closing input file\n" );
  if ( fclose ( errorsfile ) != 0 )
    fprintf ( stderr, "Error while closing lexical error file\n" );
  if ( fclose ( tokensfile ) != 0 )
    fprintf ( stderr, "Error while closing lexical tokens file\n" );
  if ( fclose ( tokenmapfile ) != 0 )
    fprintf ( stderr, "Error while closing tokens map file\n" );

  return 0;
}

