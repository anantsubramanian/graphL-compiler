// Authors: Anant Subramanian <anant.subramanian15@gmail.com>
//          Aditya Bansal <adityabansal_adi@yahoo.co.in>
//
// BITS PILANI ID NOs: 2012A7TS010P
//                     2012A7PS122P
//
// Project Team Num: 1
// Project Group No. 1

#define TRUE 1
#define FALSE 0
#define TRANSITION_LIMIT 128

typedef enum dfa_state_property
{
  DFA_STATE_PROPERTY_FIRST,
  NONE,
  TRAP,
  ERROR,
  DFA_STATE_PROPERTY_LAST
} DFA_STATE_PROPERTY;

typedef struct state_struct
{
  char *name;
  int state_number;
  int is_final;
  int special_property;
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

extern STATE* peek ( DFA *dfa, char nextinp );

extern STATE* setSpecialProperty ( STATE *state , DFA_STATE_PROPERTY property );

extern DFA* setCurrentState ( DFA *dfa, STATE *state );

extern int isFinal ( STATE *st );

extern DFA* gotoNextState ( DFA *dfa, char input );

extern STATE* resetTransition ( STATE *state, char input );

extern DFA* initializeFromFile ( DFA *dfa, const char *filename );

extern DFA_STATE_PROPERTY getSpecialProperty ( STATE *state );

