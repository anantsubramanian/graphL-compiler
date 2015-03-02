#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedlist.h"

LINKEDLIST* getLinkedList ()
{
  LINKEDLIST *list = NULL;
  list = malloc ( sizeof ( LINKEDLIST ) );

  if ( list == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for linked list\n" );
    return NULL;
  }

  list->head = NULL;
  list->tail = NULL;
  list->name = NULL;
  return list;
}

LINKEDLIST* insertAtBack ( LINKEDLIST * list, char * value )
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
  node->next = NULL;
  int len = strlen ( value );
  node->value = malloc ( (len+1) * sizeof (char) );
  if ( node->value == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for node value\n" );
    return NULL;
  }
  strcpy ( node->value, value );

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

LINKEDLIST* insertAtFront ( LINKEDLIST * list, char * value )
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
  node->prev = NULL;
  int len = strlen ( value );
  node->value = malloc ( (len+1) * sizeof (char) );
  if ( node->value == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for node value\n" );
    return NULL;
  }
  strcpy ( node->value, value );

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
      free ( temp );
    }
    else
    {
      LNODE *temp = list -> tail;
      list -> tail -> prev -> next = NULL;
      list -> tail = list -> tail -> prev;
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
      free ( temp );
    }
    else
    {
      LNODE *temp = list -> head;
      list -> head -> next -> prev = NULL;
      list -> head  = list -> head -> next;
      free ( temp );
    }
  }

  return list;
}

LNODE* copyNode ( LNODE *to, LNODE *from )
{
  if ( to == NULL || from == NULL )
  {
    fprintf ( stderr, "Attempting to copy from/to empty node\n" );
    return NULL;
  }

  to -> prev = from -> prev;
  to -> next = from -> next;

  // Intentionally referencing same location
  to -> value = from -> value;

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
  iterator -> value = NULL;

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
  iterator -> value = NULL;

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

LNODE* getNext ( LNODE * iterator )
{
  if ( iterator == NULL || iterator -> next == NULL )
  {
    fprintf ( stderr, "Potential error, no next element in list\n" );
    return NULL;
  }

  if ( copyNode ( iterator, iterator -> next ) == NULL )
  {
    fprintf ( stderr, "Failed to copy next node for iterator\n" );
    return NULL;
  }

  return iterator;
}

LNODE* getPrevious ( LNODE * iterator )
{
  if ( iterator == NULL || iterator -> prev == NULL )
  {
    fprintf ( stderr, "Potential error, no next element in list\n" );
    return NULL;
  }

  if ( copyNode ( iterator, iterator -> prev ) == NULL )
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

