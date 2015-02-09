#include "headers/dfa.h"
#include <stdio.h>

#define FILEPATH "config/dfa_lexer_description"

int main()
{
  DFA *dfa;
  dfa = getNewDFA();
  dfa = initializeFromFile ( dfa, FILEPATH );
  int shouldread = TRUE;
  char c;
  while (1)
  {
    c = getchar();
    if ( c == 'x' )
      break;
    if ( peek ( dfa, c ) == NULL || getSpecialProperty ( peek ( dfa, c ) ) == TRAP )
    {
      if ( isFinal ( getCurrentState (dfa) ) == TRUE )
        printf ( "%s\n", getCurrentState (dfa) -> name );
      else
        printf ( "Error in Lexer\n" );
      dfa = gotoInitialState ( dfa );
    }
    gotoNextState ( dfa, c );
  }
  return 0;
}
