#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

/**
 * Function that gets a new Linked List parameterized to the
 * type specified as a parameter
 *
 * @param  data_type LINKEDLIST_TYPE The type of values stored
 *
 * @return LINKEDLIST* The pointer to the allocated LinkedList
 */

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

/**
 * Function that sets the generic size of the LinkedList for
 * generic type linked lists
 *
 * @param  list LINKEDLIST* The linked list whose size is to be set
 * @param  size uint The value of the size to be set
 *
 * @return LINKEDLIST* The pointer to the altered LinkedList
 */

LINKEDLIST* setGenericSize ( LINKEDLIST *list, unsigned int size )
{
  if ( list == NULL )
  {
    fprintf ( stderr, "Cannot set the generic type size of a non-existent list\n" );
    return NULL;
  }

  if ( list -> data_type != LL_GENERIC_TYPE )
  {
    fprintf ( stderr, "Attempting to set Generic size of non-generic linked list\n" );
    return NULL;
  }

  list -> generic_size = size;

  return list;
}

/**
 * Function to insert data at the end of a linked list
 *
 * @param  list LINKEDLIST* The target linked list
 * @param  value void* The pointer to the data location
 *
 * @return LINKEDLIST* The pointer to the altered LinkedList
 */

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

/**
 * Function to insert data at the front of a linked list
 *
 * @param  list LINKEDLIST* The target linked list
 * @param  value void* The pointer to the data location
 *
 * @return LINKEDLIST* The pointer to the altered LinkedList
 */

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

/**
 * Function to insert a (multiple) space separated list of words,
 * provided as a string, into a linked list
 *
 * @param  list LINKEDLIST* The target linked list
 * @param  wordlist char* The string containing the words
 *
 * @return LINKEDLIST* The pointer to the altered LinkedList
 */

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

/**
 * Function to delete a value from the back of a linked list
 *
 * @param  list LINKEDLIST* The target linked list
 *
 * @return LINKEDLIST* The pointer to the altered LinkedList
 */

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

/**
 * Function to delete a value from the front of a linked list
 *
 * @param  list LINKEDLIST* The target linked list
 *
 * @return LINKEDLIST* The pointer to the altered LinkedList
 */

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

/**
 * Function to copy all data from a given linked list node
 * to another node. The copy is a shallow copy
 *
 * @param  list LINKEDLIST* The target linked list
 * @param  to LNODE* The target node
 * @param  from LNODE* The source node
 *
 * @return LNODE* The pointer to the altered to node
 */

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

/**
 * Function that gets an iterator to the linked list,
 * and stores it in the passed LNODE
 *
 * @param  list LINKEDLIST* The target linked list
 * @param  iterator LNODE* The target iterator
 *
 * @return LNODE* The pointer to the iterator
 */

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

/**
 * Function that gets a reverse iterator to the linked list,
 * and stores it in the passed LNODE
 *
 * @param  list LINKEDLIST* The target linked list
 * @param  iterator LNODE* The target iterator
 *
 * @return LNODE* The pointer to the iterator
 */

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

/**
 * Function that checks if a given iterator has a next node
 *
 * @param  iterator LNODE* The target iterator
 *
 * @return int 1 if a next element exists, 0 otherwise
 */

int hasNext ( LNODE * iterator )
{
  if ( iterator == NULL || iterator -> next == NULL )
    return FALSE;

  return TRUE;
}

/**
 * Function that checks if a given iterator has a previous node
 *
 * @param  iterator LNODE* The target iterator
 *
 * @return int 1 if a previous element exists, 0 otherwise
 */

int hasPrevious ( LNODE * iterator )
{
  if ( iterator == NULL || iterator -> prev == NULL )
    return FALSE;

  return TRUE;
}

/**
 * Function that gets the next element given an iterator
 *
 * @param  list LINKEDLIST* The target linked list
 * @param  iterator LNODE* The target iterator
 *
 * @return LNODE* The altered iterator
 */

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

/**
 * Function that gets the previous element given an iterator
 *
 * @param  list LINKEDLIST* The target linked list
 * @param  iterator LNODE* The target iterator
 *
 * @return LNODE* The altered iterator
 */

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

/**
 * Function that gets the first element of the given list
 *
 * @param  list LINKEDLIST* The target linked list
 *
 * @return LNODE* The pointer to the first element
 */

LNODE* getFront ( LINKEDLIST * list )
{
  if ( list == NULL )
  {
    fprintf ( stderr, "Cannot get front, list doesn't exist\n" );
    return NULL;
  }

  return list -> head;
}

/**
 * Function that gets the last element of the given list
 *
 * @param  list LINKEDLIST* The target linked list
 *
 * @return LNODE* The pointer to the last element
 */

LNODE* getBack ( LINKEDLIST * list )
{
  if ( list == NULL )
  {
    fprintf ( stderr, "Cannot get back, list doesn't exist\n" );
    return NULL;
  }

  return list -> tail;
}

/**
 * Function that deletes a linked list and frees all associated memory
 *
 * @param  list LINKEDLIST* The target linked list
 *
 */

void deleteLinkedList ( LINKEDLIST * list )
{
  if ( list == NULL )
  {
    fprintf ( stderr, "Cannot delete a non-existent linked list\n" );
    return;
  }

  while ( list -> head != NULL )
  {
    list = deleteFromFront ( list );
  }
}

