#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

STACK* getStack ( int data_type )
{
  STACK *s = NULL;
  s = malloc ( sizeof (STACK) );

  if ( s == NULL )
  {
    fprintf ( stderr, "Error allocating memory for stack\n" );
    return NULL;
  }

  if ( data_type == STACK_INT_TYPE )
    s -> stack = getLinkedList ( LL_INT_TYPE );
  else if ( data_type == STACK_DOUBLE_TYPE )
    s -> stack = getLinkedList ( LL_DOUBLE_TYPE );
  else if ( data_type == STACK_STRING_TYPE )
    s -> stack = getLinkedList ( LL_STRING_TYPE );

  if ( s -> stack == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for stack LL\n" );
    return NULL;
  }

  s -> is_empty = TRUE;
  s -> data_type = data_type;

  return s;
}

int isEmpty ( STACK * s )
{
  if ( s == NULL )
  {
    fprintf ( stderr, "Checking empty status of non-existent stack\n" );
    return -1;
  }

  return s -> is_empty;
}

STACK* push ( STACK *s, void *topush )
{
  if ( s == NULL )
  {
    fprintf ( stderr, "Cannot push into non-existent stack\n" );
    return NULL;
  }

  // Let the type check bubble down to the underlying linked-list
  s -> stack = insertAtBack ( s -> stack, topush );
  s -> is_empty = FALSE;

  return s;
}

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
    return s;
  }

  s -> stack = deleteFromBack ( s -> stack );

  if ( s -> stack -> head == NULL )
    s -> is_empty = TRUE;

  return s;
}

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
    return NULL;
  }

  if ( s -> data_type == STACK_INT_TYPE )
    return & ( getBack ( s -> stack ) -> int_val );
  else if ( s -> data_type == STACK_DOUBLE_TYPE )
    return & ( getBack ( s -> stack ) -> double_val );
  else if ( s -> data_type == STACK_STRING_TYPE )
    return getBack ( s -> stack ) -> string_val;

  return NULL;
}

STACK* insertFromLinkedList ( STACK * stack, LINKEDLIST * list )
{
  LNODE iterator;
  if ( getReverseIterator ( list, &iterator ) == NULL )
  {
    fprintf ( stderr, "Failed to get iterator\n" );
    return NULL;
  }

  if ( ( stack -> data_type == STACK_INT_TYPE && list -> data_type != LL_INT_TYPE )
      || ( stack -> data_type == STACK_DOUBLE_TYPE && list -> data_type != LL_DOUBLE_TYPE )
      || ( stack -> data_type == STACK_STRING_TYPE && list -> data_type != LL_STRING_TYPE ) )
  {
    fprintf ( stderr, "The types of linkedlist & stack don't match for insertion\n" );
    return NULL;
  }

  while ( hasPrevious ( &iterator ) )
  {
    getPrevious ( list, &iterator );
    if ( stack -> data_type == STACK_INT_TYPE )
      stack = push ( stack, & iterator.int_val );
    else if ( stack -> data_type == STACK_DOUBLE_TYPE )
      stack = push ( stack, & iterator.double_val );
    else if ( stack -> data_type == STACK_STRING_TYPE )
      stack = push ( stack, iterator.string_val );
  }

  return stack;
}

