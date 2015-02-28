#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

STACK* getStack ()
{
  STACK *s = NULL;
  s = malloc ( sizeof (STACK) );

  if ( s == NULL )
  {
    fprintf ( stderr, "Error allocating memory for stack\n" );
    return NULL;
  }

  s -> stack = getLinkedList ();

  if ( s -> stack == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for stack LL\n" );
    return NULL;
  }

  s->is_empty = TRUE;
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

STACK* push ( STACK *s, char *topush )
{
  if ( s == NULL )
  {
    fprintf ( stderr, "Cannot push into non-existent stack\n" );
    return NULL;
  }

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

char* top ( STACK * s )
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

  return getBack ( s -> stack ) -> value;
}

STACK* insertFromLinkedList ( STACK * stack, LINKEDLIST * list )
{
  LNODE *iterator = getIterator ( list );
  while ( hasNext ( iterator ) )
  {
    iterator = getNext ( iterator );
    stack = push ( stack, iterator -> value );
  }

  return stack;
}

