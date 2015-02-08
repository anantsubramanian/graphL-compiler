#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "headers/dfa.h"

DFA* getNewDFA () 
{
	DFA *dfa = NULL;
	dfa = malloc ( sizeof (DFA) );
	if ( dfa == NULL )
	{
		fprintf ( stderr, "Failed to allocate memory for DFA\n" );
		return dfa;
	}

	dfa->num_states = 0;
	dfa->all_states = NULL;
	dfa->current_state = -1;

	return dfa;
}

DFA* resetDFA ( DFA *dfa )
{
	if ( dfa == NULL )
	{
		fprintf ( stderr, "Attempted to reset a non-existent DFA\n" );
		return dfa;
	}

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
	{
		fprintf ( stderr, "Cannot set number of states if DFA doesn't exist\n" );
		return NULL;
	}
	
	if ( dfa->num_states != 0 )
	{
		dfa->all_states = realloc ( dfa->all_states, sizeof (STATE) * numStates );
	}
	else
	{
		dfa->all_states = malloc ( sizeof (STATE) * numStates );		
	}
	
	if ( dfa->all_states == NULL )
	{
		fprintf ( stderr, "Failed to allocate memory for those many states\n" );
		return NULL;
	}
	
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
	{
		fprintf ( stderr, "No initial state for a non-existent DFA\n" );
		return NULL;
	}
	
	dfa->current_state = 0;
	return dfa;
}

STATE* getState ( DFA *dfa, int stateNumber )
{
	if ( dfa == NULL )
	{
		fprintf ( stderr, "Trying to get state of non-existent DFA\n" );
		return NULL;
	}

	if ( stateNumber > dfa->num_states )
	{
		fprintf ( stderr, "State requested is out-of-bounds\n" );
		return NULL;
	}

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
	{
		fprintf ( stderr, "Couldn't allocate memory for state name\n" );
		return NULL;
	}

	strcpy ( state->name, name );
	return state;
}

STATE* addTransition ( char input, STATE *state1, STATE *state2 )
{
	if ( state1 == NULL || state2 == NULL )
	{
		fprintf ( stderr, "One of the supplied states for transition addition is non existent\n" );
		return NULL;
	}

	if ( state1->next_state[ input ] != NULL )
	{
		fprintf ( stderr, "The supplied transition already exists\n" );
		return NULL;
	}

	state1->next_state[ input ] = state2;
	return state1;
}

STATE *setFinal ( STATE *state )
{
	if ( state == NULL )
	{
		fprintf ( stderr, "Attempting to set non-existent state as final\n" );
		return NULL;
	}

	state->is_final = TRUE;
	return state;
}

STATE *getCurrentState ( DFA *dfa )
{
	if ( dfa == NULL || dfa->current_state == -1 )
	{
		fprintf ( stderr, "Can't get current state, DFA non-existent or not initialized\n" );
		return NULL;
	}

	return &( dfa->all_states[ dfa->current_state ] );
}

DFA* setCurrentState ( DFA *dfa, STATE *state )
{
	if ( dfa == NULL )
	{
		fprintf ( stderr, "Can't set current state for non-existent DFA\n" );
		return NULL;
	}

	dfa->current_state = state->state_number;
	return dfa;
}

int isFinal ( STATE *state )
{
	if ( state == NULL )
	{
		fprintf ( stderr, "Requesting final status for non-existent state\n" );
		return -1;
	}

	return state->is_final;
}

DFA* gotoNextState ( DFA *dfa, char input )
{
	if ( dfa == NULL )
	{
		fprintf ( stderr, "Cannot traverse non-existent DFA\n" );
		return NULL;
	}

	STATE *next = dfa->all_states[ dfa->current_state ].next_state [ input ];

	if ( next == NULL )
	{
		fprintf ( stderr, "Transition for supplied symbol doesn't exist for this state\n" );
		fprintf ( stderr, "Going back to initial state...\n" );
		gotoInitialState ( dfa );
	}
	else
	{
		dfa = setCurrentState ( dfa, next );
	}
	return dfa;
}

