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
    if ( shouldread == TRUE )
    {
      c = getchar();
      if ( c == 'x' )
        break;
    }
    int prevstate = getCurrentState (dfa) -> state_number;
    gotoNextState ( dfa, c );
    if ( isFinal ( getCurrentState (dfa) ) == TRUE )
      printf ( "%s\n", getCurrentState (dfa) -> name );
    int newstate = getCurrentState (dfa) -> state_number;
    if ( newstate == 0 && shouldread == TRUE )
      shouldread = FALSE;
    else
      shouldread = TRUE;
  }
  return 0;
}
