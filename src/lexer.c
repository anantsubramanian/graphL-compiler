#include "headers/dfa.h"
#include <stdio.h>

#define FILEPATH "config/dfa_lexer_description"
#define BUFFERLEN 200
#define ERRORS 100
#define NEWLINE '\n'

// TODO (Anant)  : Change lexer to read from file given as a command line argument
// TODO (Anant)  : Read from file using two buffers
// TODO (Anant)  : Output literals/identifiers to symbol file and output pointer numbers

int main()
{
  DFA *dfa;
  dfa = getNewDFA();
  dfa = initializeFromFile ( dfa, FILEPATH );

  int shouldread = TRUE;
  int started = FALSE;

  char c;
  char stringliteral [ BUFFERLEN ];
  char identifier [ BUFFERLEN ];
  char floatorint [ BUFFERLEN ];

  int idenindex = 0;
  int stringlitindex = 0;
  int floatintindex = 0;

  int linenumber = 1;
  int errorcount = 0;

  FILE *errorsfile, *tokensfile;

  errorsfile = fopen ( "Errors.txt", "w+" );
  tokensfile = fopen ( "Tokens.txt", "w+" );

  if ( errorsfile == NULL || tokensfile == NULL )
  {
    fprintf ( stderr, "Failed to open errors / tokens file\n" );
    return -1;
  }

  while ( TRUE )
  {
    c = getchar();

    if ( c == EOF )
      break;

    if ( c == 10)
      linenumber++;

    if ( peek ( dfa, c ) == NULL || getSpecialProperty ( peek ( dfa, c ) ) == TRAP )
    {
      // Token over / error encountered so should process intermediate result
      if ( isFinal ( getCurrentState (dfa) ) == TRUE )
      {
        if ( getSpecialProperty ( getCurrentState (dfa) ) == ERROR )
        {
          fprintf ( errorsfile, "%d : %s\n", linenumber, getCurrentState (dfa) -> name );
          // Just to match with Unit-testing for the time being
          fprintf ( tokensfile, "<%s>\n", getCurrentState (dfa) -> name );
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
      else
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
      else
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

  if ( errorcount == 0 )
  {
      tokensfile = fopen ( "Tokens.txt", "r" );
      if ( tokensfile == NULL )
      {
        fprintf ( stderr, "Failed to open tokens file to read the second time\n" );
        return -1;
      }

      while ( ! feof ( tokensfile ) )
      {
        fscanf ( tokensfile, "%c", &c );
        printf ( "%c", c );
      }

      if ( fclose ( tokensfile ) != 0 )
        fprintf ( stderr, "Failed to close tokens file while re-reading\n" );
  }
  return 0;
}
