#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

LINKEDLIST* getLinkedList ( LINKEDLIST_TYPE data_type )
{
  LINKEDLIST *list = NULL;
  list = malloc ( sizeof ( LINKEDLIST ) );

  if ( list == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for linked list\n" );
    return NULL;
  }

  if ( data_type <= LL_TYPE_FIRST || data_type >= LL_TYPE_LAST )
  {
    fprintf ( stderr, "Invalid type for linked list\n" );
    return NULL;
  }

  list -> head = NULL;
  list -> tail = NULL;
  list -> name = NULL;
  list -> data_type = data_type;
  list -> generic_size = -1;

  return list;
}

LINKEDLIST* setGenericSize ( LINKEDLIST *list, unsigned int size )
{
  if ( list == NULL )
  {
    fprintf ( stderr, "Cannot set the generic type size of a non-existent list\n" );
    return NULL;
  }

  list -> generic_size = size;

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

  node -> next = NULL;

  // Set new node values and copy data
  node -> data . int_val = 0;
  node -> data . double_val = 0;
  node -> data . string_val = NULL;
  node -> data . generic_val = NULL;

  if ( list -> data_type == LL_INT_TYPE )
  {
    node -> data . int_val = * ( (int *) value );
  }
  else if ( list -> data_type == LL_DOUBLE_TYPE )
  {
    node -> data . double_val = * ( (double *) value );
  }
  else if ( list -> data_type == LL_STRING_TYPE )
  {
    char *strval = (char *) value;
    int len = strlen ( strval );
    node -> data . string_val = malloc ( (len+1) * sizeof (char) );
    if ( node -> data . string_val == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for node's string value\n" );
      return NULL;
    }
    strcpy ( node -> data . string_val, strval );
  }
  else
  {
    // Is a generic type list
    if ( list -> generic_size == -1 )
    {
      fprintf ( stderr, "Calling insert on generic list without specifying size\n" );
      return NULL;
    }

    node -> data . generic_val = malloc ( list -> generic_size );
    if ( node -> data . generic_val == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for generic type node data\n" );
      return NULL;
    }

    // Copy the raw data from value to generic_val
    memcpy ( node -> data . generic_val, value, list -> generic_size );
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

  node -> prev = NULL;

  // Set new node values and copy data
  node -> data . int_val = 0;
  node -> data . double_val = 0;
  node -> data . string_val = NULL;
  node -> data . generic_val = NULL;

  if ( list -> data_type == LL_INT_TYPE )
  {
    node -> data . int_val = * ( (int *) value );
  }
  else if ( list -> data_type == LL_DOUBLE_TYPE )
  {
    node -> data . double_val = * ( (double *) value );
  }
  else if ( list -> data_type == LL_STRING_TYPE )
  {
    char *strval = (char *) value;
    int len = strlen ( strval );
    node -> data . string_val = malloc ( (len+1) * sizeof (char) );
    if ( node -> data . string_val == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for node's string value\n" );
      return NULL;
    }
    strcpy ( node -> data . string_val, strval );
  }
  else
  {
    // Is a generic type list
    if ( list -> generic_size == -1 )
    {
      fprintf ( stderr, "Calling insert on generic list without specifying size\n" );
      return NULL;
    }

    node -> data . generic_val = malloc ( list -> generic_size );
    if ( node -> data . generic_val == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for generic type node data\n" );
      return NULL;
    }

    // Copy the raw data from value to generic_val
    memcpy ( node -> data . generic_val, value, list -> generic_size );
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
        free ( temp -> data . string_val );
      else if ( list -> data_type == LL_GENERIC_TYPE )
        free ( temp -> data . generic_val );

      free ( temp );
    }
    else
    {
      LNODE *temp = list -> tail;
      list -> tail -> prev -> next = NULL;
      list -> tail = list -> tail -> prev;

      if ( list -> data_type == LL_STRING_TYPE )
        free ( temp -> data . string_val );
      else if ( list -> data_type == LL_GENERIC_TYPE )
        free ( temp -> data . generic_val );

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
        free ( temp -> data . string_val );
      else if ( list -> data_type == LL_GENERIC_TYPE )
        free ( temp -> data . generic_val );

      free ( temp );
    }
    else
    {
      LNODE *temp = list -> head;
      list -> head -> next -> prev = NULL;
      list -> head  = list -> head -> next;

      if ( list -> data_type == LL_STRING_TYPE )
        free ( temp -> data . string_val );
      else if ( list -> data_type == LL_GENERIC_TYPE )
        free ( temp -> data . generic_val );

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

  to -> data . string_val = NULL;
  to -> data . generic_val = NULL;
  to -> data . int_val = 0;
  to -> data . double_val = 0;

  // Intentionally referencing same location
  if ( list -> data_type == LL_INT_TYPE )
    to -> data . int_val = from -> data . int_val;
  else if ( list -> data_type == LL_DOUBLE_TYPE )
    to -> data . double_val = from -> data . double_val;
  else if ( list -> data_type == LL_STRING_TYPE )
    to -> data . string_val = from -> data . string_val;
  else if ( list -> data_type == LL_GENERIC_TYPE )
    to -> data . generic_val = from -> data . generic_val;

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
  iterator -> data . int_val = 0;
  iterator -> data . double_val = 0;
  iterator -> data . string_val = NULL;
  iterator -> data . generic_val = NULL;

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
  iterator -> data . int_val = 0;
  iterator -> data . double_val = 0;
  iterator -> data . string_val = NULL;
  iterator -> data . generic_val = NULL;

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

