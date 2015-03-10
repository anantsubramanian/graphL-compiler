#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

LINKEDLIST* getLinkedList ( int data_type )
{
  LINKEDLIST *list = NULL;
  list = malloc ( sizeof ( LINKEDLIST ) );

  if ( list == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for linked list\n" );
    return NULL;
  }

  list -> head = NULL;
  list -> tail = NULL;
  list -> name = NULL;
  list -> data_type = data_type;

  return list;
}

LINKEDLIST* insertAtBack ( LINKEDLIST * list, void * value )
{
  if ( list == NULL )
  {
    fprintf ( stderr, "Attempting to insert into non-existent list\n" );
    return NULL;
  }

  LNODE *node = NULL;
  node = malloc ( sizeof (LNODE) );
  if ( node == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory while inserting at back\n" );
    return NULL;
  }

  // Set new node values and copy data
  node -> next = NULL;

  if ( list -> data_type == LL_INT_TYPE )
  {
    node -> int_val = * ( (int *) value );
    node -> double_val = 0;
    node -> string_val = NULL;
  }
  else if ( list -> data_type == LL_DOUBLE_TYPE )
  {
    node -> double_val = * ( (double *) value );
    node -> int_val = 0;
    node -> string_val = NULL;
  }
  else if ( list -> data_type == LL_STRING_TYPE )
  {
    char *strval = (char *) value;
    int len = strlen ( strval );
    node -> string_val = malloc ( (len+1) * sizeof (char) );
    if ( node -> string_val == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for node's string value\n" );
      return NULL;
    }
    strcpy ( node -> string_val, strval );
  }
  else
  {
    fprintf ( stderr, "Trying to call non-generic insert on generic string list\n" );
    return NULL;
  }

  if ( list -> head == NULL )
  {
    node -> prev = NULL;
    list -> head = node;
    list -> tail = node;
  }
  else
  {
    node -> prev = list -> tail;
    list -> tail -> next = node;
    list -> tail = node;
  }

  return list;
}

LINKEDLIST* insertAtFront ( LINKEDLIST * list, void * value )
{
  if ( list == NULL )
  {
    fprintf ( stderr, "Attempting to insert into non-existent list\n" );
    return NULL;
  }

  LNODE *node = NULL;
  node = malloc ( sizeof (LNODE) );
  if ( node == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory while inserting at back\n" );
    return NULL;
  }

  // Set new node values and copy data
  node -> prev = NULL;

  if ( list -> data_type == LL_INT_TYPE )
  {
    node -> int_val = * ( (int *) value );
    node -> double_val = 0;
    node -> string_val = NULL;
  }
  else if ( list -> data_type == LL_DOUBLE_TYPE )
  {
    node -> double_val = * ( (double *) value );
    node -> int_val = 0;
    node -> string_val = NULL;
  }
  else if ( list -> data_type == LL_STRING_TYPE )
  {
    char *strval = (char *) value;
    int len = strlen ( strval );
    node -> string_val = malloc ( (len+1) * sizeof (char) );
    if ( node -> string_val == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for node's string value\n" );
      return NULL;
    }
    strcpy ( node -> string_val, strval );
  }
  else
  {
    fprintf ( stderr, "Trying to call non-generic insert on generic string list\n" );
    return NULL;
  }

  if ( list -> head == NULL )
  {
    node -> next = NULL;
    list -> head = node;
    list -> tail = node;
  }
  else
  {
    node -> next = list -> head;
    list -> head -> prev = node;
    list -> head = node;
  }

  return list;
}

LINKEDLIST* insertSpaceSeparatedWords ( LINKEDLIST * list, char * wordlist )
{
  if ( list == NULL )
  {
    fprintf ( stderr, "Attempting to insert space sep. words into non-existent list\n" );
    return NULL;
  }

  if ( list -> data_type != LL_STRING_TYPE )
  {
    fprintf ( stderr, "Cannot insert space separated words into a non-string LL\n" );
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
    while ( indx < len && wordlist [ indx ] > 32 )
    {
      buffer [ buffindx++ ] = wordlist [ indx ];
      indx++;
    }

    buffer [ buffindx ] = '\0';

    list = insertAtBack ( list, buffer );
  } while ( indx < len );

  return list;
}

LINKEDLIST* deleteFromBack ( LINKEDLIST * list )
{
  if ( list == NULL )
  {
    fprintf ( stderr, "Attempting to insert into non-existent list\n" );
    return NULL;
  }

  if ( list -> head == NULL )
  {
    fprintf ( stderr, "Attempting to delete from empty list\n" );
    return NULL;
  }
  else
  {
    if ( list -> head == list -> tail )
    {
      LNODE *temp = list -> head;
      list -> head = list -> tail = NULL;

      if ( list -> data_type == LL_STRING_TYPE )
        free ( temp -> string_val );

      free ( temp );
    }
    else
    {
      LNODE *temp = list -> tail;
      list -> tail -> prev -> next = NULL;
      list -> tail = list -> tail -> prev;

      if ( list -> data_type == LL_STRING_TYPE )
        free ( temp -> string_val );

      free ( temp );
    }
  }

  return list;
}

LINKEDLIST* deleteFromFront ( LINKEDLIST * list )
{
  if ( list == NULL )
  {
    fprintf ( stderr, "Attempting to insert into non-existent list\n" );
    return NULL;
  }

  if ( list -> head == NULL )
  {
    fprintf ( stderr, "Attempting to delete from empty list\n" );
    return NULL;
  }
  else
  {
    if ( list -> head == list -> tail )
    {
      LNODE *temp = list -> head;
      list -> head = list -> tail = NULL;

      if ( list -> data_type == LL_STRING_TYPE )
        free ( temp -> string_val );

      free ( temp );
    }
    else
    {
      LNODE *temp = list -> head;
      list -> head -> next -> prev = NULL;
      list -> head  = list -> head -> next;

      if ( list -> data_type == LL_STRING_TYPE )
        free ( temp -> string_val );

      free ( temp );
    }
  }

  return list;
}

LNODE* copyNode ( LINKEDLIST *list, LNODE *to, LNODE *from )
{
  if ( to == NULL || from == NULL )
  {
    fprintf ( stderr, "Attempting to copy from/to empty node\n" );
    return NULL;
  }

  to -> prev = from -> prev;
  to -> next = from -> next;

  to -> int_val = 0;
  to -> double_val = 0;
  to -> string_val = NULL;

  // Intentionally referencing same location
  if ( list -> data_type == LL_INT_TYPE )
    to -> int_val = from -> int_val;
  else if ( list -> data_type == LL_DOUBLE_TYPE )
    to -> double_val = from -> double_val;
  else if ( list -> data_type == LL_STRING_TYPE )
    to -> string_val = from -> string_val;

  return to;
}

LNODE* getIterator ( LINKEDLIST * list, LNODE * iterator )
{
  if ( list == NULL )
  {
    fprintf ( stderr, "Cannot get iterator of non-existent list\n" );
    return NULL;
  }

  if ( iterator == NULL )
  {
    fprintf ( stderr, "Invalid location provided for iterator\n" );
    return NULL;
  }

  iterator -> next = list -> head;
  iterator -> prev = NULL;
  iterator -> int_val = -1;
  iterator -> double_val = -1;
  iterator -> string_val = NULL;

  return iterator;
}

LNODE* getReverseIterator ( LINKEDLIST * list, LNODE * iterator )
{
  if ( list == NULL )
  {
    fprintf ( stderr, "Cannot get iterator of non-existent list\n" );
    return NULL;
  }

  if ( iterator == NULL )
  {
    fprintf ( stderr, "Invalid location provided for iterator\n" );
    return NULL;
  }

  iterator -> next = NULL;
  iterator -> prev = list -> tail;
  iterator -> int_val = -1;
  iterator -> double_val = -1;
  iterator -> string_val = NULL;

  return iterator;
}

int hasNext ( LNODE * iterator )
{
  if ( iterator == NULL || iterator -> next == NULL )
    return FALSE;

  return TRUE;
}

int hasPrevious ( LNODE * iterator )
{
  if ( iterator == NULL || iterator -> prev == NULL )
    return FALSE;

  return TRUE;
}

LNODE* getNext ( LINKEDLIST * list, LNODE * iterator )
{
  if ( iterator == NULL || iterator -> next == NULL )
  {
    fprintf ( stderr, "Potential error, no next element in list\n" );
    return NULL;
  }

  if ( copyNode ( list, iterator, iterator -> next ) == NULL )
  {
    fprintf ( stderr, "Failed to copy next node for iterator\n" );
    return NULL;
  }

  return iterator;
}

LNODE* getPrevious ( LINKEDLIST * list, LNODE * iterator )
{
  if ( iterator == NULL || iterator -> prev == NULL )
  {
    fprintf ( stderr, "Potential error, no next element in list\n" );
    return NULL;
  }

  if ( copyNode ( list, iterator, iterator -> prev ) == NULL )
  {
    fprintf ( stderr, "Failed to copy previous node for iterator\n" );
    return NULL;
  }

  return iterator;
}

LNODE* getFront ( LINKEDLIST * list )
{
  if ( list == NULL )
  {
    fprintf ( stderr, "Cannot get front, list doesn't exist\n" );
    return NULL;
  }

  return list -> head;
}

LNODE* getBack ( LINKEDLIST * list )
{
  if ( list == NULL )
  {
    fprintf ( stderr, "Cannot get back, list doesn't exist\n" );
    return NULL;
  }

  return list -> tail;
}

