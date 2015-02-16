#include <stdio.h>
#include <string.h>
#include "headers/dfa.h"

#define DFA_PATH "config/dfa_lexer_description"
#define TOKENS_FILE "TOKENS"
#define ERRORS_FILE "ERRORS"
#define BUFFERLEN 200
#define ERRORS 100
#define NEWLINE '\n'

// TODO (Anant)  : Change lexer to read from file given as a command line argument
// TODO (Anant)  : Read from file using two buffers
// TODO (Anant)  : Output literals/identifiers to symbol file and output pointer numbers

int main ( int argc, char *argv[] )
{
  DFA *dfa;
  dfa = getNewDFA();
  dfa = initializeFromFile ( dfa, DFA_PATH );

  int shouldread = TRUE;
  int started = FALSE;

  char c;
  char errorc;
  char stringliteral [ BUFFERLEN ];
  char identifier [ BUFFERLEN ];
  char floatorint [ BUFFERLEN ];

  int idenindex = 0;
  int stringlitindex = 0;
  int floatintindex = 0;

  int linenumber = 1;
  int errorcount = 0;

  FILE *errorsfile, *tokensfile;

  errorsfile = fopen ( ERRORS_FILE, "w+" );
  tokensfile = fopen ( TOKENS_FILE, "w+" );

  if ( errorsfile == NULL || tokensfile == NULL )
  {
    fprintf ( stderr, "Failed to open errors / tokens file\n" );
    return -1;
  }

  while ( TRUE )
  {
    errorc = c;
    c = getchar();

    if ( c == EOF )
      break;

    if ( c == NEWLINE )
      linenumber++;

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
          fprintf ( tokensfile, "<TK_INTLIT,%s>\n", floatorint );
        else if ( strcmp ( getCurrentState (dfa) -> name , "TK_FLOATLIT" ) == 0 )
          fprintf ( tokensfile, "<TK_FLOATLIT,%s>\n", floatorint );
        else if ( strcmp ( getCurrentState (dfa) -> name , "TK_STRINGLIT" ) == 0 )
          fprintf ( tokensfile, "<TK_STRINGLIT,%s>\n", stringliteral );
        else if ( strcmp ( getCurrentState (dfa) -> name , "TK_IDEN" ) == 0 )
          fprintf ( tokensfile, "<TK_IDEN,%s>\n", identifier );
        else
          fprintf ( tokensfile, "<%s>\n", getCurrentState (dfa) -> name );
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

  if ( fclose ( errorsfile ) != 0 )
    fprintf ( stderr, "Error while closing lexical error file\n" );
  if ( fclose ( tokensfile ) != 0 )
    fprintf ( stderr, "Error while closing lexical tokens file\n" );

  return 0;
}

