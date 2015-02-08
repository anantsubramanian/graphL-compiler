#include "headers/dfa.h"
#include <stdio.h>

int main()
{
	DFA *dfa;
	dfa = getNewDFA();
	dfa = setNumStates(dfa, 5);
	dfa = gotoInitialState(dfa);
	addTransition ('0', getState (dfa, 0), getState (dfa, 1) );
	addTransition ('0', getState (dfa, 1), getState (dfa, 2) );
	addTransition ('0', getState (dfa, 2), getState (dfa, 3) );
	addTransition ('0', getState (dfa, 3), getState (dfa, 4) );
	addTransition ('0', getState (dfa, 4), getState (dfa, 4) );
	addTransition ('1', getState (dfa, 4), getState (dfa, 0) );
	addTransition ('1', getState (dfa, 3), getState (dfa, 0) );
	addTransition ('1', getState (dfa, 2), getState (dfa, 0) );
	addTransition ('1', getState (dfa, 1), getState (dfa, 0) );
	addTransition ('1', getState (dfa, 0), getState (dfa, 0) );
	setFinal ( getState (dfa, 4) );
	setName ( getState (dfa, 4), "0000 detected!" );

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
