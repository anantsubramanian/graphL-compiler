#include "headers/dfa.h"
#include <stdio.h>

#define FILEPATH "config/dfa_lexer_description"
#define BUFFERLEN 200

// TODO (Aditya) : Output tokens in correct format, with spaces
// TODO (Aditya) : Add error states, names and transitions
// TODO (Aditya) : Output errors and tokens to files instead of stdout
// TODO (Aditya) : Output line numbers with errors
// TODO (Aditya) : Count number of errors and stop at #define amount
// TODO (Aditya) : Only display tokens if number of errors == 0
// TODO (Aditya) : Read from file using two buffers

int main()
{
  DFA *dfa;
  dfa = getNewDFA();
  dfa = initializeFromFile ( dfa, FILEPATH );
  int shouldread = TRUE;
  char c;
  char stringliteral[ BUFFERLEN ];
  char identifier[ BUFFERLEN ];
  int idenindex = 0;
  int stringlitindex = 0;
  int intliteral = 0;
  float floatliteral = 0.0, floatliteral2 = 0.0;
  int decimalcount = 1;
  int started = FALSE;
  while (1)
  {
    c = getchar();
    if ( c == EOF )
      break;

    if ( peek ( dfa, c ) == NULL || getSpecialProperty ( peek ( dfa, c ) ) == TRAP )
    {
      if ( isFinal ( getCurrentState (dfa) ) == TRUE )
      {
        if ( strcmp ( getCurrentState (dfa) -> name , "TK_INTLIT" ) == 0 )
          printf ( "%d\n", intliteral );
        else if ( strcmp ( getCurrentState (dfa) -> name , "TK_FLOATLIT" ) == 0 )
          printf ( "%f\n", floatliteral + floatliteral2 );
        else if ( strcmp ( getCurrentState (dfa) -> name , "TK_STRINGLIT" ) == 0 )
          printf ( "%s\n", stringliteral );
        else if ( strcmp ( getCurrentState (dfa) -> name , "TK_IDEN" ) == 0 )
          printf ( "%s\n", identifier );
        else
          printf ( "%s\n", getCurrentState (dfa) -> name );
      }
      
      if ( peek ( dfa, c ) == NULL )
        dfa = gotoInitialState ( dfa );
    }

    gotoNextState ( dfa, c );

    if ( getCurrentState (dfa) != NULL && getCurrentState (dfa) -> name != NULL )
    {
      if ( strcmp ( getCurrentState (dfa) -> name , "Decimal Point" ) == 0 )
      {
        floatliteral = (float) intliteral;
        decimalcount = 1;
        floatliteral2 = 0;
      }
      else if ( strcmp ( getCurrentState (dfa) -> name, "TK_FLOATLIT" ) == 0 )
      {
        decimalcount *= 10;
        floatliteral2 = floatliteral2 + ((c - 48) / (float) decimalcount);
      }
      else
        floatliteral = 0;

      if ( strcmp ( getCurrentState (dfa) -> name , "TK_INTLIT" ) == 0 )
        intliteral = intliteral * 10 + c - 48;
      else
        intliteral = 0;

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
  return 0;
}
