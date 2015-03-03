#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

TRIE* getNewTrie ()
{
  TRIE* trie = NULL;
  trie = malloc ( sizeof (TRIE) );

  if ( trie == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for Trie\n" );
    return NULL;
  }

  trie->name = NULL;
  trie->root = malloc ( sizeof ( TNODE ) );
  if ( trie->root == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for the root\n" );
    return NULL;
  }

  initializeNode ( trie->root );

  // Give root a sentinel count value
  trie->root->count = -1;

  return trie;
}

TRIE* setTrieName ( TRIE *trie, const char *name )
{
  if ( trie == NULL )
  {
    fprintf ( stderr, "Cannot set the name of a non-existent Trie\n" );
    return NULL;
  }

  if ( name == NULL )
  {
    if ( trie->name != NULL )
    {
      free ( trie->name );
      trie->name = NULL;
    }
  }
  else
  {
    if ( trie->name != NULL )
      free ( trie->name );
    int len = strlen ( name );
    trie->name = malloc ( (len+1) * sizeof (char) );
    if ( trie->name == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for Trie's name\n" );
      return NULL;
    }
    strcpy ( trie->name, name );
  }

  return trie;
}

TNODE* initializeNode ( TNODE *node )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Attempting to initialize non-existent ndoe\n" );
    return NULL;
  }

  node->name = NULL;
  node->value = 0;
  node->is_final = FALSE;
  node->count = 0;

  int i;
  for ( i = 0; i < 128; i++ )
    node -> next [ i ] = NULL;

  return node;
}

TNODE* insertString ( TRIE *trie, const char *str )
{
  if ( trie == NULL )
  {
    fprintf ( stderr, "Cannot insert into a non-existent Trie\n" );
    return NULL;
  }

  if ( str == NULL )
  {
    fprintf ( stderr, "Potential, attempting to insert an empty string?\n" );
    return NULL;
  }

  int i = 0;
  TNODE *curnode = trie->root;
  for ( i = 0; str [i] != '\0'; i++ )
  {
    if ( curnode -> next [ str [i] ] == NULL ) break;
    curnode = curnode -> next [ str [i] ];
    curnode -> count ++;
  }

  if ( str [i] != '\0' )
  {
    for ( ; str [i] != '\0'; i++ )
    {
      curnode -> next [ str [i] ] = malloc ( sizeof ( TNODE ) );
      if ( curnode -> next [ str [i] ] == NULL )
      {
        fprintf ( stderr, "Failed to allocate memory for node while inserting\n" );
        return NULL;
      }
      curnode = curnode -> next [ str [i] ];
      initializeNode ( curnode );
      curnode -> count++;
    }
  }

  curnode -> is_final = TRUE;

  return curnode;
}

TNODE* findString ( TRIE *trie, const char *str )
{
  if ( trie == NULL )
  {
    fprintf ( stderr, "Attempting to find string in a non-existent trie\n" );
    return NULL;
  }

  if ( str == NULL )
  {
    fprintf ( stderr, "Possible error, search for an empty string\n" );
    return NULL;
  }

  TNODE *curnode = trie->root;
  int i;

  for ( i = 0; str [i] != '\0'; i++ )
  {
    if ( curnode -> next [ str [i] ] == NULL )
    {
      curnode = NULL;
      break;
    }
    curnode = curnode -> next [ str [i] ];
  }

  if ( str [i] != '\0' || isFinalTNode ( curnode ) == FALSE )
    return NULL;

  return curnode;
}

TNODE* setValue ( TNODE *node, int value )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Attempting to set value of non-existent node\n" );
    return NULL;
  }

  node->value = value;
  return node;
}

TNODE* setNodeName ( TNODE *node, const char *name )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Attempting to set name of non-existent node\n" );
    return NULL;
  }

  if ( node->name != NULL )
  {
    free ( node -> name );
    node -> name = NULL;
  }

  if ( name == NULL )
    fprintf ( stderr, "Potential error, attempting to set name as NULL\n" );
  else
  {
    int len = strlen ( name );
    node -> name = malloc ( (len+1) * sizeof (char) );
    if( node -> name == NULL )
    {
      fprintf ( stderr, "Allocation of Trie node name failed\n" );
      return NULL;
    }

    strcpy ( node->name, name );
  }

  return node;
}

TRIE* deleteString ( TRIE* trie, const char *str )
{
  // **Assumes String being deleted exists in the Trie
  if ( trie == NULL )
  {
    fprintf ( stderr, "Cannot delete string from non-existent Trie\n" );
    return NULL;
  }

  TNODE *curnode = trie->root;
  int i;
  for ( i = 0; str [i] != '\0'; i++ )
  {
    TNODE *tempnode = curnode;
    curnode = curnode -> next [ str [i] ];
    curnode -> count--;

    if ( curnode -> count == 0 && tempnode != NULL )
      tempnode -> next [ str [i] ] = NULL;

    if ( tempnode != NULL && tempnode -> count == 0 )
    {
      if ( tempnode -> name != NULL )
      {
        free ( tempnode -> name );
        tempnode -> name = NULL;
      }
      free ( tempnode );
    }
  }

  if ( curnode != NULL && curnode -> count == 0 )
  {
    if ( curnode -> name != NULL )
      free ( curnode -> name );
    free ( curnode );
  }

  return trie;
}

int isFinalTNode ( TNODE *node )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Checking final state status for non-existent node\n" );
    return -1;
  }

  return node->is_final;
}

