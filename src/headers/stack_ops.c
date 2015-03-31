// Authors: Anant Subramanian <anant.subramanian15@gmail.com>
//          Aditya Bansal <adityabansal_adi@yahoo.co.in>
//
// BITS PILANI ID NOs: 2012A7TS010P
//                     2012A7PS122P
//
// Project Team Num: 1
// Project Group No. 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"

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

STACK* setStackGenericSize ( STACK *s, unsigned int size )
{
  if ( s == NULL )
  {
    fprintf ( stderr, "Cannot set generic type size for non-existent stack\n" );
    return NULL;
  }

  s -> generic_size = size;

  // Set the same generic size for underlying linked list representation
  s -> stack = setGenericSize ( s -> stack, size );

  return s;
}

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
  // Let generic type not set warning bubble down to linked-list too
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

