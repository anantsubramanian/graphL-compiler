#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

/**
 * Function that allocates memory for and returns a new stack
 * parameterized to the specified type
 *
 * @param  data_type STACK_TYPE The parameter type
 *
 * @return STACK* The pointer to the created Stack
 *
 */

STACK* getStack ( STACK_TYPE data_type )
{
  STACK *s = NULL;
  s = malloc ( sizeof (STACK) );

  if ( s == NULL )
  {
    fprintf ( stderr, "Error allocating memory for stack\n" );
    return NULL;
  }

  if ( data_type <= STACK_TYPE_FIRST || data_type >= STACK_TYPE_LAST )
  {
    fprintf ( stderr, "Invalid type provided while creating stack\n" );
    return NULL;
  }

  if ( data_type == STACK_INT_TYPE )
    s -> stack = getLinkedList ( LL_INT_TYPE );
  else if ( data_type == STACK_DOUBLE_TYPE )
    s -> stack = getLinkedList ( LL_DOUBLE_TYPE );
  else if ( data_type == STACK_STRING_TYPE )
    s -> stack = getLinkedList ( LL_STRING_TYPE );
  else if ( data_type == STACK_GENERIC_TYPE )
    s -> stack = getLinkedList ( LL_GENERIC_TYPE );
  else
  {
    fprintf ( stderr, "Invalid type provided for stack\n" );
    return NULL;
  }

  if ( s -> stack == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for stack LL\n" );
    return NULL;
  }

  s -> name = NULL;
  s -> is_empty = TRUE;
  s -> data_type = data_type;
  s -> generic_size = -1;

  return s;
}

/**
 * Function that sets the size of a generic type stack
 *
 * @param  s STACK* The target stack
 * @param  size uint The value of the size to be set
 *
 * @return STACK* The pointer to the altered Stack
 *
 */

STACK* setStackGenericSize ( STACK *s, unsigned int size )
{
  if ( s == NULL )
  {
    fprintf ( stderr, "Cannot set generic type size for non-existent stack\n" );
    return NULL;
  }

  if ( s -> data_type != STACK_GENERIC_TYPE )
  {
    fprintf ( stderr, "Cannot set the generic type size of a non-generic stack\n" );
    return NULL;
  }

  s -> generic_size = size;

  // Set the same generic size for underlying linked list representation
  s -> stack = setGenericSize ( s -> stack, size );

  return s;
}

/**
 * Function that sets/clears the name of a stack
 *
 * @param  stack STACK* The target stack
 * @param  name char* The string containing the name
 *
 * @return STACK* The pointer to the altered Stack
 *
 */

STACK* setStackName ( STACK *stack, char * name )
{
  if ( stack == NULL )
  {
    fprintf ( stderr, "Cannot set the name of a non-existent stack\n" );
    return NULL;
  }

  if ( stack -> name != NULL )
  {
    fprintf ( stderr, "Overwriting stack %s's name\n", stack -> name );
    free ( stack -> name );
    stack -> name = NULL;
  }

  if ( name != NULL )
  {
    int len = strlen ( name );
    stack -> name = malloc ( (len+1) * sizeof ( char ) );

    strcpy ( stack -> name, name );
  }

  return stack;
}

/**
 * Function that checks if a stack is empty
 *
 * @param  stack STACK* The target stack
 *
 * @return int 1 if the stack if empty, 0 otherwise
 *
 */

int isEmpty ( STACK * s )
{
  if ( s == NULL )
  {
    fprintf ( stderr, "Checking empty status of non-existent stack\n" );
    return -1;
  }

  return s -> is_empty;
}

/**
 * Function that pushes data onto the top of the stack
 *
 * @param  stack STACK* The target stack
 * @param  topush void* The pointer to the data to be pused
 *
 * @return STACK* The pointer to the altered stack
 *
 */

STACK* push ( STACK *s, void *topush )
{
  if ( s == NULL )
  {
    fprintf ( stderr, "Cannot push into non-existent stack\n" );
    return NULL;
  }

  // Let the type check bubble down to the underlying linked-list
  // Let generic type not set warning bubble down to linked-list too
  s -> stack = insertAtBack ( s -> stack, topush );
  s -> is_empty = FALSE;

  return s;
}

/**
 * Function that pops data from the top of the stack and discards it
 *
 * @param  stack STACK* The target stack
 *
 * @return STACK* The pointer to the altered stack
 *
 */

STACK* pop ( STACK *s )
{
  if ( s == NULL )
  {
    fprintf ( stderr, "Cannot pop from non-existent stack\n" );
    return NULL;
  }

  if ( s -> stack -> head == NULL )
  {
    fprintf ( stderr, "Potential error, stack is empty\n" );
    if ( s -> name != NULL )
      fprintf ( stderr, "In stack %s\n", s -> name );

    return s;
  }

  // Linkedlist automatically handles freeing of generic type memory
  s -> stack = deleteFromBack ( s -> stack );

  if ( s -> stack -> head == NULL )
    s -> is_empty = TRUE;

  return s;
}

/**
 * Function that returns the pointer to the data on top of the stack
 * Note that the function does not create a copy of the data, it
 * returns the true pointer to the data currently ON the stack
 *
 * @param  stack STACK* The target stack
 *
 * @return void* The pointer to the data on top of the stack
 *
 */

void* top ( STACK * s )
{
  if ( s == NULL )
  {
    fprintf ( stderr, "Cannot get the top of non-existent stack\n" );
    return NULL;
  }

  if ( s -> stack -> head == NULL )
  {
    fprintf ( stderr, "Potential error, getting top of empty stack\n" );
    if ( s -> name != NULL )
      fprintf ( stderr, "In stack %s\n", s -> name );
    return NULL;
  }

  if ( s -> data_type == STACK_INT_TYPE )
    return & ( ( getBack ( s -> stack ) -> data ) . int_val );
  else if ( s -> data_type == STACK_DOUBLE_TYPE )
    return & ( ( getBack ( s -> stack ) -> data ) . double_val );
  else if ( s -> data_type == STACK_STRING_TYPE )
    return  ( getBack ( s -> stack ) -> data ) . string_val;
  else
    return ( getBack ( s -> stack ) -> data ) . generic_val;

  return NULL;
}

/**
 * Function that pushes all the data in a linked list onto the stack,
 * starting from the tail and moving towards the head of the list
 *
 * @param  stack STACK* The target stack
 * @param  list LINKEDLIST* The source linked list
 *
 * @return STACK* The pointer to the altered stack
 *
 */

STACK* insertFromLinkedList ( STACK * stack, LINKEDLIST * list )
{
  if ( list == NULL || stack == NULL )
  {
    fprintf ( stderr, "Cannot push data onto an empty stack/from an empty LL\n" );
    return NULL;
  }

  LNODE iterator;
  if ( getReverseIterator ( list, &iterator ) == NULL )
  {
    fprintf ( stderr, "Failed to get reverse iterator to LL while pushing onto stack\n" );
    return NULL;
  }

  if ( ( stack -> data_type == STACK_INT_TYPE && list -> data_type != LL_INT_TYPE )
      || ( stack -> data_type == STACK_DOUBLE_TYPE && list -> data_type != LL_DOUBLE_TYPE )
      || ( stack -> data_type == STACK_STRING_TYPE && list -> data_type != LL_STRING_TYPE )
      || ( stack -> data_type == STACK_GENERIC_TYPE && list -> data_type != LL_GENERIC_TYPE ) )
  {
    fprintf ( stderr, "The types of linkedlist & stack don't match for insertion\n" );
    if ( stack -> name != NULL )
      fprintf ( stderr, "For stack %s\n",  stack -> name );
    return NULL;
  }

  if ( stack -> data_type == STACK_GENERIC_TYPE && stack -> generic_size != list -> generic_size )
  {
    fprintf ( stderr, "Generic type sizes of linked list and stack don't match. Cannot insert\n" );
    if ( stack -> name != NULL )
      fprintf ( stderr, "For stack %s\n", stack -> name );
    return NULL;
  }

  while ( hasPrevious ( &iterator ) )
  {
    getPrevious ( list, &iterator );
    if ( stack -> data_type == STACK_INT_TYPE )
      stack = push ( stack, & (iterator.data.int_val) );
    else if ( stack -> data_type == STACK_DOUBLE_TYPE )
      stack = push ( stack, & (iterator.data.double_val) );
    else if ( stack -> data_type == STACK_STRING_TYPE )
      stack = push ( stack, (iterator.data.string_val) );
    else
      stack = push ( stack, (iterator.data.generic_val) );
  }

  return stack;
}

/**
 * Function that pushes all words from a (multiple) space separated list
 * of words, provided as a string, onto a stack in the reverse order
 *
 * @param  stack STACK* The target stack
 * @param  wordlist char* The space separated word list
 *
 * @return STACK* The pointer to the altered stack
 *
 */

STACK* pushReverseSpaceSeparatedWords ( STACK * stack, const char * wordlist )
{
  STACK *tempstack = NULL;
  tempstack = getStack ( STACK_STRING_TYPE );

  if ( stack == NULL )
  {
    fprintf ( stderr, "Attempting to push space separated words into non-existent stack\n" );
    return NULL;
  }

  if ( wordlist == NULL )
  {
    fprintf ( stderr, "Attempting to push non-existent space separated word list on stack\n" );
    if ( stack -> name != NULL )
      fprintf ( stderr, "For stack %s\n", stack -> name );
    return NULL;
  }

  if ( stack -> data_type != STACK_STRING_TYPE )
  {
    fprintf ( stderr, "Cannot insert space separated words list into non-string stack\n" );
    if ( stack -> name != NULL )
      fprintf ( stderr, "For stack %s\n", stack -> name );
    return NULL;
  }

  int indx = 0;
  int len = strlen ( wordlist );
  char buffer [ len + 1 ];

  do
  {
    while ( indx < len && wordlist [ indx ] <= 32 ) indx++;
    if ( indx == len ) break;

    int buffindx = 0;
    int inquote = 0;
    while ( indx < len && ( inquote == 1 || wordlist [ indx ] > 32 ) )
    {
      buffer [ buffindx++ ] = wordlist [ indx ];
      if ( wordlist [ indx ] == '"' )
      {
        if ( inquote == 0 )
          inquote = 1;
        else
          inquote = 0;
      }
      indx++;
    }

    buffer [ buffindx ] = '\0';

    tempstack = push ( tempstack, buffer );
  } while ( indx < len );

  // Now reverse the contents by popping tempstack and pushing into stack
  while ( !isEmpty ( tempstack ) )
  {
    stack = push ( stack, top ( tempstack ) );
    tempstack = pop ( tempstack );
  }

  // We are done with tempstack, free the memory
  free ( tempstack );

  return stack;
}

