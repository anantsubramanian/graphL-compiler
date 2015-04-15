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
#include "trie.h"

/**
 * Function that allocates memory for and returns a pointer
 * to a trie of the specified type
 *
 * @param  data_type TRIE_TYPE The parameter type
 *
 * @return TRIE* The pointer to the allocated trie
 *
 */

TRIE* getNewTrie ( TRIE_TYPE data_type )
{
  TRIE* trie = NULL;
  trie = malloc ( sizeof (TRIE) );

  if ( trie == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for Trie\n" );
    return NULL;
  }

  if ( data_type <= TRIE_TYPE_FIRST || data_type >= TRIE_TYPE_LAST )
  {
    fprintf ( stderr, "Invalid type for Trie\n" );
    return NULL;
  }

  trie -> name = NULL;
  trie -> data_type = data_type;
  trie -> generic_size = -1;
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

/**
 * Function that sets/clears the name of the given trie
 *
 * @param  trie TRIE* The target trie
 * @param  name char* The string containing the name
 *
 * @return TRIE* The pointer to the altered trie
 *
 */

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

/**
 * Function that sets the generic size of the trie
 *
 * @param  trie TRIE* The target trie
 * @param  size uint The value of the size
 *
 * @return TRIE* The pointer to the altered trie
 *
 */

TRIE* setTrieGenericSize ( TRIE *trie, unsigned int size )
{
  if ( trie == NULL )
  {
    fprintf ( stderr, "Cannot set the generic type size of a non-existent trie\n" );
    return NULL;
  }

  if ( trie -> data_type != TRIE_GENERIC_TYPE )
  {
    fprintf ( stderr, "Cannot set generic type size of a non-generic trie\n" );
    return NULL;
  }

  trie -> generic_size = size;

  return trie;
}

/**
 * Function that initializes the data in a node with sentinel values
 *
 * @param  node TNODE* The pointer to the target node
 *
 * @return TNODE* The pointer to the altered node
 *
 */

TNODE* initializeNode ( TNODE *node )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Attempting to initialize non-existent ndoe\n" );
    return NULL;
  }

  node -> name = NULL;
  node -> data . int_val = 0;
  node -> data . double_val = 0;
  node -> data . string_val = NULL;
  node -> data . generic_val = NULL;
  node->is_final = FALSE;
  node->count = 0;

  int i;
  for ( i = 0; i < 128; i++ )
    node -> next [ i ] = NULL;

  return node;
}

/**
 * Function that inserts a string into a trie and returns
 * the pointer to the unique node for the string
 *
 * @param  trie TRIE* The target trie
 * @param  str char* The string to be inserted
 *
 * @return TNODE* The pointer to the unique node for the string
 *
 */

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
  TNODE *curnode = trie -> root;
  for ( i = 0; str [i] != '\0'; i++ )
  {
    if ( curnode -> next [ (int) str [i] ] == NULL ) break;
    curnode = curnode -> next [ (int) str [i] ];
    curnode -> count ++;
  }

  if ( str [i] != '\0' )
  {
    for ( ; str [i] != '\0'; i++ )
    {
      curnode -> next [ (int) str [i] ] = malloc ( sizeof ( TNODE ) );
      if ( curnode -> next [ (int) str [i] ] == NULL )
      {
        fprintf ( stderr, "Failed to allocate memory for node while inserting\n" );
        return NULL;
      }
      curnode = curnode -> next [ (int) str [i] ];
      initializeNode ( curnode );
      curnode -> count++;
    }
  }

  curnode -> is_final = TRUE;

  return curnode;
}

/**
 * Function that finds a string in the given trie and returns the
 * pointer to its unique node if it exists
 *
 * @param  trie TRIE* The target trie
 * @param  str char* The string to be located
 *
 * @return TNODE* The pointer to the unique node for the string
 *
 */

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
    if ( curnode -> next [ (int) str [i] ] == NULL )
    {
      curnode = NULL;
      break;
    }
    curnode = curnode -> next [ (int) str [i] ];
  }

  if ( str [i] != '\0' || isFinalTNode ( curnode ) == FALSE )
    return NULL;

  return curnode;
}

/**
 * Function that sets a value to a trie node, depending on the type
 * of the trie
 *
 * @param  trie TRIE* The target trie
 * @param  node TNODE* The target node
 * @param  value void* The pointer to the data
 *
 * @return TNODE* The pointer to the altered trie node
 *
 */

TNODE* setValue ( TRIE * trie, TNODE *node, void * value )
{
  if ( trie == NULL )
  {
    fprintf ( stderr, "Attempting to set value of non-existent trie\n" );
    return NULL;
  }

  if ( node == NULL )
  {
    fprintf ( stderr, "Attempting to set value of non-existent node\n" );
    return NULL;
  }

  if ( trie -> data_type == TRIE_INT_TYPE )
  {
    node -> data . int_val = * ( (int *) value );
  }
  else if ( trie -> data_type == TRIE_DOUBLE_TYPE )
  {
    node -> data . double_val = * ( (double *) value );
  }
  else if ( trie -> data_type == TRIE_STRING_TYPE )
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
    if ( trie -> generic_size == -1 )
    {
      fprintf ( stderr, "Calling insert on generic list without specifying size\n" );
      return NULL;
    }

    node -> data . generic_val = malloc ( trie -> generic_size );
    if ( node -> data . generic_val == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for generic type node data\n" );
      return NULL;
    }

    // Copy the raw data from value to generic_val
    memcpy ( node -> data . generic_val, value, trie -> generic_size );
  }

  return node;
}

/**
 * Function that sets the name of a trie node
 *
 * @param  node TNODE* The target node
 * @param  name char* The string containing the name
 *
 * @return TNODE* The pointer to the altered trie node
 *
 */

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

/**
 * Function that deletes a string from the given trie
 * The assumption is that the string being deleted exists in
 * the trie. If not, the behavior of the function is
 * unpredictable
 *
 * @param  trie TRIE* The target trie
 * @param  str char* The string being deleted
 *
 * @return TRIE* The pointer to the altered trie
 *
 */

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
    curnode = curnode -> next [ (int) str [i] ];
    curnode -> count--;

    if ( curnode -> count == 0 && tempnode != NULL )
      tempnode -> next [ (int) str [i] ] = NULL;

    if ( tempnode != NULL && tempnode -> count == 0 )
    {
      if ( tempnode -> name != NULL )
      {
        free ( tempnode -> name );
        tempnode -> name = NULL;
        if ( trie -> data_type == TRIE_GENERIC_TYPE && tempnode -> data . generic_val != NULL )
        {
          free ( tempnode -> data . generic_val );
          tempnode -> data . generic_val = NULL;
        }
      }
      free ( tempnode );
    }
  }

  if ( curnode != NULL && curnode -> count == 0 )
  {
    if ( curnode -> name != NULL )
      free ( curnode -> name );
    if ( trie -> data_type == TRIE_GENERIC_TYPE && curnode -> data . generic_val != NULL )
      free ( curnode -> data . generic_val );
    free ( curnode );
  }

  return trie;
}

/**
 * Function that checks if the supplied trie node is final or not
 *
 * @param  node TNODE* The target node
 *
 * @return int 1 if the node is final, 0 otherwise
 *
 */

int isFinalTNode ( TNODE *node )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Checking final state status for non-existent node\n" );
    return -1;
  }

  return node -> is_final;
}

