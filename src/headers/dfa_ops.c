#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dfa.h"

#define MAX_NAME_LEN 100
#define BUFFERLENGTH 50
#define DEBUG_OVERWRITE 0

/**
 * Function that allocates memory for and returns the pointer
 * to a new Deterministic Finite Automaton object
 *
 * @return DFA* The pointer to the new DFA memory location
 */

DFA* getNewDFA ()
{
  DFA *dfa = NULL;
  dfa = malloc ( sizeof (DFA) );
  if ( dfa == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for DFA\n" );
    return dfa;
  }

  // Initially the DFA has no states and current state is undefined
  dfa->num_states = 0;
  dfa->all_states = NULL;
  dfa->current_state = -1;

  return dfa;
}

/**
 * Function that frees all the memory allocated for the passed
 * DFA and returns it to a fresh state
 *
 * @param  dfa DFA* The DFA that must be reset
 *
 * @return DFA* The pointer to the dfa, restored to its original
 *              empty state
 */

DFA* resetDFA ( DFA *dfa )
{
  if ( dfa == NULL )
  {
    fprintf ( stderr, "Attempted to reset a non-existent DFA\n" );
    return dfa;
  }

  // Free memory reserved for states of DFA if they exist
  if ( dfa->all_states != NULL )
  {
    free ( dfa->all_states );
    dfa->all_states = NULL;
    dfa->current_state = -1;
  }

  dfa->num_states = 0;
  return dfa;
}

/**
 * Function that sets the number of states of the given DFA
 * and allocates the memory for the necessary pointers
 *
 * @param  dfa DFA* The DFA whose number of states must be set
 * @param  int numStates The number of states to be used
 *
 * @return DFA* The pointer to the altered DFA
 *
 */

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

  // Initialize the newly added states if any
  int i;
  for (i = dfa->num_states; i < numStates; i++)
  {
    dfa->all_states[i].name = NULL;
    dfa->all_states[i].state_number = i;
    dfa->all_states[i].is_final = FALSE;
    dfa->all_states[i].special_property = NONE;
    int j;
    for (j = 0; j < TRANSITION_LIMIT; j++)
      dfa->all_states[i].next_state[j] = NULL;
  }

  dfa->num_states = numStates;

  return dfa;
}

/**
 * Function that takes a DFA back to its initial state, which
 * by convention is numbered 0
 *
 * @param  dfa DFA* The DFA whose current state must be changed
 *
 * @return DFA* The pointer to the altered DFA
 *
 */

DFA* gotoInitialState ( DFA *dfa )
{
  if ( dfa == NULL )
  {
    fprintf ( stderr, "No initial state for a non-existent DFA\n" );
    return NULL;
  }

  // The initial state is always 0
  dfa->current_state = 0;
  return dfa;
}

/**
 * Function that returns the pointer to a particular state of
 * the DFA. Can be used to modify the state
 *
 * @param  dfa DFA* The DFA whose state must be accessed
 * @param  int stateNumber The number of the state to be accessed
 *
 * @return STATE* The pointer to the accessed state
 *
 */

STATE* getState ( DFA *dfa, int stateNumber )
{
  if ( dfa == NULL )
  {
    fprintf ( stderr, "Trying to get state of non-existent DFA\n" );
    return NULL;
  }

  if ( stateNumber >= dfa->num_states )
  {
    fprintf ( stderr, "State requested is out-of-bounds\n" );
    return NULL;
  }

  return &( dfa->all_states[ stateNumber ] );
}

/**
 * Function that sets the name of a state
 *
 * @param  state STATE* The state whose name is to be set
 * @param  name char* The name string to be used
 *
 * @return STATE* The pointer to the altered state
 *
 */

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

/**
 * Function that adds a transition from one state to another
 * on an input character
 *
 * @param  char input The input symbol on which the transition
 *                    should occur
 * @param  state1 STATE* The from state
 * @param  state2 STATE* The to state
 *
 * @return STATE* The pointer to the altered from state
 *
 */

STATE* addTransition ( char input, STATE *state1, STATE *state2 )
{
  if ( state1 == NULL || state2 == NULL )
  {
    fprintf ( stderr, "One of the supplied states for transition addition is non existent\n" );
    return NULL;
  }

  if ( state1->next_state[ (int) input ] != NULL )
  {
    if ( DEBUG_OVERWRITE )
    {
      fprintf ( stderr, "A transition for state %d and ASCII %d already exists\n", state1->state_number, (int) input );
      fprintf ( stderr, "Overwriting transition..\n" );
    }
  }

  state1->next_state[ (int) input ] = state2;
  return state1;
}

/**
 * Function that sets a particular state as final
 *
 * @param  state STATE* The function which should be set as final
 *
 * @return STATE* The pointer to the altered state
 *
 */

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

/**
 * Function that gets the current state of the given DFA
 *
 * @param  dfa DFA* The DFA whose current state is to be found
 *
 * @return STATE* The pointer to the current state of dfa
 *
 */

STATE *getCurrentState ( DFA *dfa )
{
  if ( dfa == NULL || dfa->current_state == -1 )
  {
    fprintf ( stderr, "Can't get current state, DFA non-existent or not initialized\n" );
    return NULL;
  }

  return &( dfa->all_states[ dfa->current_state ] );
}

/**
 * Function that sets a special property for the given state
 *
 * @param  state STATE* The state whose special property is to be set
 * @param  property DFA_STATE_PROPERTY The special property value
 *
 * @return STATE* The pointer to the altered state
 *
 */

STATE *setSpecialProperty ( STATE *state, DFA_STATE_PROPERTY property )
{
  if ( state == NULL )
  {
    fprintf ( stderr, "Cannot set special property for non-existent state" );
    return NULL;
  }

  if ( property < NONE || property > ERROR )
  {
    fprintf ( stderr, "Invalid property value for state\n" );
    return NULL;
  }

  state->special_property = property;
  return state;
}

/**
 * Function that sets the current state of the DFA
 *
 * @param  dfa DFA* The DFA whose current state is to be set
 * @param  state STATE* The new current state
 *
 * @return DFA* The pointer to the altered DFA
 *
 */

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

/**
 * Function that checks whether the provided state is final
 * or not
 *
 * @param  state STATE* The state to be checked
 *
 * @return int 1 if the state is final, 0 otherwise
 *
 */

int isFinal ( STATE *state )
{
  if ( state == NULL )
  {
    fprintf ( stderr, "Requesting final status for non-existent state\n" );
    return -1;
  }

  return state->is_final;
}

/**
 * Function that simulates running the DFA in its current
 * state on an input symbol, and returns the modified DFA
 *
 * @param  dfa DFA* The DFA to simulate
 * @param  input char The transition character
 *
 * @return DFA* The pointer to the altered DFA
 *
 */

DFA* gotoNextState ( DFA *dfa, char input )
{
  if ( dfa == NULL )
  {
    fprintf ( stderr, "Cannot traverse non-existent DFA\n" );
    return NULL;
  }

  if ( dfa->current_state < 0 || dfa->current_state >= dfa->num_states )
  {
    fprintf ( stderr, "DFA is in an inconsistent state\n" );
    return NULL;
  }

  STATE *next = dfa->all_states[ dfa->current_state ].next_state [ (int) input ];

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

/**
 * Function that erases an existing transition from a
 * DFA state
 *
 * @param  state STATE* The state whose transition is to be erased
 * @param  input char The character for the transition to erase
 *
 * @return STATE* The pointer to the altered state
 *
 */

STATE* resetTransition ( STATE *state, char input )
{
  if ( state == NULL )
  {
    fprintf ( stderr, "Cannot reset transition for non-existent state\n" );
    return NULL;
  }

  state->next_state [ (int) input ] = NULL;
  return state;
}

/**
 * Function that initializes a complete DFA from a file. The format
 * of the file can be found in the src/config folder
 *
 * @param  dfa DFA* The DFA to be initialized
 * @param  filename char* The name of file to use
 *
 * @return DFA* The pointer to the initialized DFA
 *
 */

DFA* initializeFromFile ( DFA *dfa, const char *filename )
{
  FILE *file = NULL;

  char heading [BUFFERLENGTH];

  file = fopen ( filename, "r" );

  if ( file == NULL )
  {
    fprintf ( stderr, "Unable to open file to initialize DFA\n" );
    return NULL;
  }

  if ( file == NULL )
  {
    fprintf ( stderr, "Cannot initialize from a non-existent file\n" );
    return NULL;
  }

  if ( dfa == NULL )
  {
    fprintf ( stderr, "Cannot initialize a non-existent DFA from a file\n" );
    return NULL;
  }

  if ( dfa->num_states != 0 )
  {
    fprintf ( stderr, "DFA being initialized from file already has states\n" );
    return NULL;
  }

  int num_states = 0;

  // Read the number of states
  fscanf ( file, "%s", heading );
  fscanf ( file, "%d", &num_states );

  if ( num_states <= 0 )
  {
    fprintf ( stderr, "Malformed file, incorrect number of states\n" );
    return NULL;
  }

  dfa = setNumStates ( dfa, num_states );

  int final_states = 0;

  fscanf ( file, "%s", heading );
  fscanf ( file, "%d", &final_states );

  if ( final_states < 0 )
  {
    fprintf ( stderr, "Malformed file, incorrect number of state descriptions\n" );
    return NULL;
  }

  int i = 0;

  // Final states heading
  fscanf ( file, "%s", heading );

  // Read list of final states
  for ( i = 0; i < final_states; i++ )
  {
    int finalstate;
    fscanf ( file, "%d", &finalstate );
    setFinal ( getState ( dfa, finalstate ) );
  }

  fscanf ( file, "%s", heading );

  int special_states = 0;
  fscanf ( file, "%d", &special_states );

  if ( special_states < 0 )
  {
    fprintf ( stderr, "Incorrect number of special states in file\n" );
    return NULL;
  }

  // Read 2 headings for special states
  fscanf ( file, "%s", heading );
  fscanf ( file, "%s", heading );

  // Read special states details
  for ( i = 0; i < special_states; i++ )
  {
    int statenum;
    char property;
    fscanf ( file, "%d", &statenum );
    if ( statenum < 0 || statenum > num_states )
    {
      fprintf ( stderr, "Incorrect special state number\n" );
      return NULL;
    }

    do
    {
      fscanf ( file, "%c", &property );
    } while ( property != 'T' && property != 'E' && property != 'N' );

    if ( property == 'T' )
      setSpecialProperty ( getState ( dfa, statenum ), TRAP );
    else if ( property == 'E' )
      setSpecialProperty ( getState ( dfa, statenum ), ERROR );
  }

  fscanf ( file, "%s", heading );

  int named_states = 0;
  fscanf ( file, "%d", &named_states );

  for ( i = 0; i < 3; i++)
    fscanf ( file, "%s", heading );

  // Read state names
  for ( i = 0; i < named_states; i++ )
  {
    int countstates = 1;
    int statenum;
    char name [MAX_NAME_LEN];

    // Read name field
    int index = 0;
    do
    {
      fscanf ( file, "%c", &name[ index ] );
    } while ( name[ index ] != '"' );

    do
    {
      fscanf ( file, "%c", &name[ index ] );
    } while ( name[ index++ ] != '"' );

    name[ index-1 ] = '\0';

    fscanf ( file, "%d", &countstates );
    if ( countstates <= 0 )
    {
      fprintf ( stderr, "Incorrect number of states while setting names\n" );
      return NULL;
    }

    // For each of the counstates states, set the same name
    for ( i = 0; i < countstates; i++ )
    {
      fscanf ( file, "%d", &statenum );
      if ( statenum >= num_states || statenum < 0 )
      {
        fprintf ( stderr, "Malformed file, incorrect state number in description\n" );
        return NULL;
      }

      setName ( getState ( dfa, statenum ) , name );
    }
  }

  int num_transitions;
  fscanf ( file, "%s", heading );
  fscanf ( file, "%d", &num_transitions );

  if ( num_transitions < 0 )
  {
    fprintf ( stderr, "Malformed file, incorrent number of transitions specified\n" );
    return NULL;
  }

  // Transitions start
  // Read seven headings
  for ( i = 0; i < 7; i++ )
    fscanf ( file, "%s", heading );

  // Read the transitions
  for ( i = 0; i < num_transitions; i++ )
  {
    int numfrom, numto;
    char printable;
    fscanf ( file, "%d", &numfrom );

    int shouldReset = FALSE;
    if ( numfrom < 0 )
    {
      shouldReset = TRUE;
      numfrom = abs ( numfrom );
    }
    else if ( numfrom == 0 )
    {
      fprintf ( stderr, "Incorrect number of from states\n" );
      return NULL;
    }

    int fromlist [ numfrom ];
    int fromindx = 0;

    // Populate the 'from' states list
    for ( fromindx = 0; fromindx < numfrom; fromindx++ )
    {
      fscanf ( file, "%d", fromlist + fromindx );
      if ( fromlist [ fromindx ] < 0 || fromlist [ fromindx ] > num_states )
      {
        fprintf ( stderr, "Incorrect state number in transitions\n" );
        return NULL;
      }
    }

    fscanf ( file, "%d", &numto );

    if ( shouldReset == FALSE && numto <= 0 )
    {
      fprintf ( stderr, "Invalid number of to states\n" );
      return NULL;
    }

    int tolist [ numto ];
    int toindx = 0;

    if ( shouldReset == FALSE)
    {
      // Populate the 'to' states list
      for ( toindx = 0; toindx < numto; toindx++ )
      {
        fscanf ( file, "%d", tolist + toindx );
        if ( tolist [ toindx ] < 0 || tolist [ toindx ] > num_states )
        {
          fprintf ( stderr, "Incorrect state number in transitions\n" );
          return NULL;
        }
      }
    }

    do
    {
      fscanf ( file, "%c", &printable );
    } while ( printable != 'Y' && printable != 'N' );

    if ( printable == 'Y' )
    {
      // If the ASCII character is printable, read it as a CHAR
      int numchars = 0;

      fscanf ( file, "%d", &numchars );
      if ( numchars <= 0 )
      {
        fprintf ( stderr, "Incorrect number of characters in transitions\n" );
        return NULL;
      }

      int j;

      // For each of the numchars characters, add the transitions
      for ( j = 0; j < numchars; j++ )
      {
        char a;
        do
        {
          fscanf ( file, "%c", &a );
        } while ( a <= 32 );

        if ( shouldReset == TRUE )
        {
          // Reset transitions for each from state
          for ( fromindx = 0; fromindx < numfrom; fromindx++ )
            resetTransition ( getState ( dfa, fromlist [ fromindx ] ), a );
        }
        else
        {
          // Add transitions from every state in fromlist to every state in tolist
          for ( fromindx = 0; fromindx < numfrom; fromindx++ )
            for ( toindx = 0; toindx < numto; toindx++ )
              addTransition ( a, getState ( dfa, fromlist [ fromindx ] ),
                                 getState ( dfa, tolist [ toindx ] ) );
        }
      }
    }
    else if ( printable == 'N' )
    {
      // If the ASCII character is entered as a number representing its ASCII value
      int numvals = 0;

      fscanf ( file, "%d", &numvals );
      if ( numvals <= 0 )
      {
        fprintf ( stderr, "Incorrect number of ASCII values in transitions\n" );
        return NULL;
      }

      int j;

      // For each of the numvals ASCII values given
      for ( j = 0; j < numvals; j++ )
      {
        int asciival;
        int lowerlimit = 0, upperlimit = 0;
        fscanf ( file, "%d", &asciival );

        if ( asciival == -1 )
        {
          // Transition valid for any ASCII character
          lowerlimit = 0;
          upperlimit = 128;
        }
        else if ( asciival == -2 )
        {
          // Transition valid for any lower case character
          lowerlimit = 97;
          upperlimit = 123;
        }
        else if ( asciival == -3 )
        {
          // Transition valid for any upper case character
          lowerlimit = 65;
          upperlimit = 91;
        }
        else if ( asciival == -4 )
        {
          // Transition valid for any digit
          lowerlimit = 48;
          upperlimit = 58;
        }
        else if ( asciival == -5 )
        {
          // Transition valid for any whitespace character
          lowerlimit = 0;
          upperlimit = 33;
        }
        else if ( asciival < 0 || asciival > 127 )
        {
          fprintf ( stderr, "Malformed file, incorrent ASCII value provided\n" );
          return NULL;
        }
        else
        {
          // Transition valid only for that ASCII character
          lowerlimit = asciival;
          upperlimit = asciival + 1;
        }

        int p;
        if ( shouldReset )
        {
          // Reset transitions for each p for each state in from list
          for ( fromindx = 0; fromindx < numfrom; fromindx++ )
            for ( p = lowerlimit; p < upperlimit; p++ )
              resetTransition ( getState ( dfa, fromlist [ fromindx ] ), (char) p );
        }
        else
        {
          // For each char 'p' add a transition from fromlist to tolist state
          for ( fromindx = 0; fromindx < numfrom; fromindx++ )
            for ( toindx = 0; toindx < numto; toindx++ )
              for ( p = lowerlimit; p < upperlimit; p++ )
                addTransition ( (char) p, getState ( dfa, fromlist [ fromindx ] ),
                                          getState ( dfa, tolist [ toindx ] ) );
        }
      }
    }
  }

  if ( fclose ( file ) != 0 )
    fprintf ( stderr, "Failed to close file used to initialize DFA\n" );

  dfa = gotoInitialState (dfa);
  return dfa;
}

/**
 * Function that returns the next state for the DFA, on an
 * input character, without altering the DFA itself
 *
 * @param  dfa DFA* The DFA to peek
 * @param  nextinp char The input char on which to peek
 *
 * @return STATE* The next state if it exists or NULL
 *
 */

STATE* peek ( DFA *dfa, char nextinp )
{
  if ( dfa == NULL )
  {
    fprintf ( stderr, "Cannot peek a non-existent DFA\n" );
    return NULL;
  }

  if ( dfa->current_state == -1 )
  {
    fprintf ( stderr, "Cannot peek an uninitialized DFA\n" );
    return NULL;
  }

  return dfa->all_states[ dfa->current_state ].next_state[ (int) nextinp ];
}

/**
 * Function that returns the special property value for
 * a state if it exists
 *
 * @param  state STATE* The state to be inspected
 *
 * @return DFA_STATE_PROPERTY The special property value for the state
 */

DFA_STATE_PROPERTY getSpecialProperty ( STATE *state )
{
  if ( state == NULL )
  {
    fprintf ( stderr, "No property for non-existent state\n" );
    return -1;
  }

  return state->special_property;
}

