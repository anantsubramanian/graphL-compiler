#include "headers/dfa.h"
#include <stdio.h>

#define FILEPATH "config/dfa_description"

int main()
{
  DFA *dfa;
  dfa = getNewDFA();
  dfa = initializeFromFile ( dfa, FILEPATH );
  while (1)
  {
    char c;
    if ( c == 'x' )
      break;
    c = getchar();
    if ( c < '0' || c > '9' ) continue;
    gotoNextState ( dfa, c );
    if ( isFinal ( getCurrentState (dfa) ) == TRUE )
      printf ( "%s\n", getCurrentState (dfa) -> name );
  }
  return 0;
}
