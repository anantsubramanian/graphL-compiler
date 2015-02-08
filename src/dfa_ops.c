#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "headers/dfa.h"

DFA* getNewDFA () 
{
	DFA *dfa = NULL;
	dfa = malloc ( sizeof (DFA) );
	if ( dfa == NULL )
		return dfa;

	dfa->num_states = 0;
	dfa->all_states = NULL;
	dfa->current_state = -1;

	return dfa;
}

DFA* resetDFA ( DFA *dfa )
{
	if ( dfa == NULL )
		return dfa;

	if ( dfa->all_states != NULL )
	{
		free ( dfa->all_states );
		dfa->all_states = NULL;
		dfa->current_state = -1;
	}
	
	dfa->num_states = 0;
	return dfa;
}

DFA* setNumStates ( DFA *dfa, int numStates )
{
	if ( dfa == NULL )
		return NULL;
	
	if ( dfa->num_states != 0 )
	{
		dfa->all_states = realloc ( dfa->all_states, sizeof (STATE) * numStates );
	}
	else
	{
		dfa->all_states = malloc ( sizeof (STATE) * numStates );		
	}
	
	if ( dfa->all_states == NULL )
		return NULL;
	
	int i;
	for (i = dfa->num_states; i < numStates; i++)
	{
		dfa->all_states[i].name = NULL;
		dfa->all_states[i].state_number = i;
		dfa->all_states[i].is_final = FALSE;
		int j;
		for (j = 0; j < TRANSITION_LIMIT; j++)
			dfa->all_states[i].next_state[j] = NULL;
	}
	dfa->num_states = numStates;
}

DFA* gotoInitialState ( DFA *dfa )
{
	if ( dfa == NULL )
		return NULL;
	
	dfa->current_state = 0;
	return dfa;
}

STATE* getState ( DFA *dfa, int stateNumber )
{
	if ( dfa == NULL )
		return NULL;

	if ( stateNumber > dfa->num_states )
		return NULL;

	return &( dfa->all_states[ stateNumber ] );
}

STATE* setName ( STATE *state, const char *name )
{
	if ( state == NULL )
		return NULL;

	if ( name == NULL )
		return state;

	int len = strlen ( name );
	state->name = malloc ( sizeof (char) * (len+1) );

	if ( state->name == NULL )
		return NULL;

	strcpy ( state->name, name );
	return state;
}

STATE* addTransition ( char input, STATE *state1, STATE *state2 )
{
	if ( state1 == NULL || state2 == NULL )
		return NULL;

	if ( state1->next_state[ input ] != NULL )
		return NULL;

	state1->next_state[ input ] = state2;
	return state1;
}

STATE *setFinal ( STATE *state )
{
	if ( state == NULL )
		return NULL;

	state->is_final = TRUE;
	return state;
}

STATE *getCurrentState ( DFA *dfa )
{
	if ( dfa == NULL || dfa->current_state == -1 )
		return NULL;

	return &( dfa->all_states[ dfa->current_state ] );
}

DFA* setCurrentState ( DFA *dfa, STATE *state )
{
	if ( dfa == NULL )
		return NULL;

	dfa->current_state = state->state_number;
	return dfa;
}

int isFinal ( STATE *state )
{
	if ( state == NULL )
		return -1;

	return state->is_final;
}

DFA* gotoNextState ( DFA *dfa, char input )
{
	if ( dfa == NULL )
		return NULL;

	STATE *next = dfa->all_states[ dfa->current_state ].next_state [ input ];

	if ( next == NULL )
		gotoInitialState ( dfa );
	else
	{
		dfa = setCurrentState ( dfa, next );
	}
}

