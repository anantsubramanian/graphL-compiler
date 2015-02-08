#define TRUE 1
#define FALSE 0
#define TRANSITION_LIMIT 128

typedef struct state_struct
{
	char *name;
	int state_number;
	int is_final;
	struct state_struct *next_state[TRANSITION_LIMIT];
} STATE;

typedef struct dfa_struct
{
	int num_states;
	int current_state;
	STATE *all_states;
} DFA;

extern DFA* getNewDFA ( );

extern DFA* resetDFA ( DFA *dfa );

extern DFA* setNumStates ( DFA *dfa, int numStates );

extern DFA* gotoInitialState ( DFA *dfa );

extern STATE* getState ( DFA *dfa, int stateNumber );

extern STATE* setName ( STATE *state , const char *name );

extern STATE* addTransition ( char input, STATE *state1, STATE *state2 );

extern STATE* setFinal ( STATE *state );

extern STATE* getCurrentState ( DFA *dfa );

extern DFA* setCurrentState ( DFA *dfa, STATE *state );

extern int isFinal ( STATE *st );

extern DFA* gotoNextState ( DFA *dfa, char input );

extern DFA* initializeFromFile ( DFA *dfa, const char *filename );

