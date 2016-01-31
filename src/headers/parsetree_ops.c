#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parsetree.h"

#define TRUE 1

/**
 * Function that allocates memory for and returns a new Parse Tree
 *
 * @return PARSETREE* The pointer to the allocated parse tree
 *
 */

PARSETREE* getNewParseTree ()
{
  PARSETREE* pst = NULL;
  pst = malloc ( sizeof (PARSETREE) );

  if ( pst == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for Parse Tree\n" );
    return NULL;
  }

  pst -> name = NULL;
  pst -> root = malloc ( sizeof ( PTNODE ) );
  if ( pst -> root == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for the root\n" );
    return NULL;
  }

  // Initialize root with parent as NULL
  initializeParseTreeNode ( pst->root, NULL );

  // Set root's num children to sentinel value
  pst -> root -> num_of_children = -1;
  pst -> root -> child_id = 0;

  return pst;
}

/**
 * Function that sets/clears the name of the parse tree
 *
 * @param pst PARSETREE* The target parse tree
 * @param name char* The name containing string
 *
 * @return PARSETREE* The pointer to the allocated parse tree
 *
 */

PARSETREE* setParseTreeName ( PARSETREE *pst, const char *name )
{
  if ( pst == NULL )
  {
    fprintf ( stderr, "Cannot set the name of a non-existent Parse Tree\n" );
    return NULL;
  }

  if ( pst -> name != NULL )
  {
    free ( pst -> name );
    pst -> name = NULL;
  }

  if ( name != NULL )
  {
    int len = strlen ( name );
    pst -> name = malloc ( (len+1) * sizeof (char) );
    if ( pst -> name == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for Parse Tree's name\n" );
      return NULL;
    }
    strcpy ( pst -> name, name );
  }

  return pst;
}

/**
 * Function that initializes a parse tree node by giving its attributes
 * sentinel values
 *
 * @param node PTNODE* The target parse tree node
 * @param parent PTNODE* The parent of the target node
 *
 * @return PTNODE* The pointer to the initialized node
 *
 */

PTNODE* initializeParseTreeNode ( PTNODE *node, PTNODE *parent )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Attempting to initialize non-existent node\n" );
    return NULL;
  }

  node -> name = NULL;
  node -> num_of_children = -1;
  node -> child_id = -1;
  node -> parent = parent;
  node -> next = NULL;

  return node;
}

/**
 * Function that sets/clears the name of a parse tree node
 *
 * @param node PTNODE* The target parse tree node
 * @param str char* The string containing the name
 *
 * @return PTNODE* The pointer to the altered node
 *
 */

PTNODE* setParseTreeNodeName ( PTNODE *node, const char *str )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Cannot insert into a non-existent Ast node\n" );
    return NULL;
  }

  if ( str == NULL )
  {
    fprintf ( stderr, "Potential error, attempting to set node name as NULL\n" );
    node -> name = NULL;
  }
  else
  {
    if ( node -> name != NULL )
      free ( node -> name );

    int len = strlen ( str );
    node -> name = malloc ( (len+1) * sizeof (char) );
    if ( node -> name == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for node's name\n" );
      return NULL;
    }

    strcpy ( node->name, str );
  }
  return node;
}

/**
 * Function that sets the number of children of a parse tree node
 *
 * @param node PTNODE* The target parse tree node
 * @param value int The number of children
 *
 * @return PTNODE* The pointer to the altered node
 *
 */

PTNODE* setNumChildren ( PTNODE *node, int value )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Attempting to set value of non-existent node\n" );
    return NULL;
  }

  if ( value <= 0 )
    fprintf ( stderr, "Attempting to set PARSETREE node's num children <= 0\n" );

  // If the node already has children, free them
  if ( node -> num_of_children != -1 )
  {
    int i;
    for ( i = 0; i < node -> num_of_children; i++ )
    {
      free ( node -> next [i] );
      node -> next [i] = NULL;
    }
  }

  node -> num_of_children = value;
  return node;
}

/**
 * Function that recursively deletes and frees the memory
 * of a parse tree node and all its descendants
 *
 * @param node PTNODE* The target parse tree node
 *
 */

void deleteParseTreeNode ( PTNODE * node )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Cannot delete a non-existent parse tree node\n" );
    return;
  }

  if ( node -> name != NULL )
    free ( node -> name );

  // Recursively delete all of the current node's children
  int i;
  for ( i = 0; i < node -> num_of_children; i++ )
    deleteParseTreeNode ( node -> next [i] );

  free ( node );
}

/**
 * Function that allocates memory for the pointers to the
 * children and the children of the given node
 *
 * @param node PTNODE* The target parse tree node
 *
 * @return PTNODE* The altered parse tree node
 *
 */

PTNODE* allocateChildren ( PTNODE * node )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Invalid location provided for iterator\n" );
    return NULL;
  }

  if ( node -> num_of_children == -1 )
  {
  	fprintf ( stderr, "Set the number of children before calling for allocation\n" );
    return NULL;
  }

  if ( node -> num_of_children <= 0 )
  {
    fprintf ( stderr, "Cannot allocate negative number of children for PARSETREE node\n" );
    return NULL;
  }

  // If next array has already been populated, free it
  int i;

  if ( node -> next != NULL )
  {
    for ( i = 0; i < node -> num_of_children; i++ )
      deleteParseTreeNode ( node -> next [i] );
    free ( node -> next );
  }

  node -> next = malloc ( (node -> num_of_children) * sizeof (PTNODE *) );

  for( i = 0 ; i < node -> num_of_children ; i++)
  {
    node -> next [i] = NULL;
    node -> next [i] = malloc ( sizeof ( PTNODE ) );
    if ( node -> next [i] == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for child" );
      return NULL;
    }

    initializeParseTreeNode ( node -> next [i], node );
    node -> next [i] -> child_id = i;
  }

  return node;
}

/**
 * Function that creates the children of a node according to a
 * list of provided space separated words
 *
 * @param node PTNODE* The target parse tree node
 * @param wordlist char* The space separated word list
 *
 * @return PTNODE* The altered parse tree node
 *
 */

PTNODE* insertSpaceSeparatedWordsInPTree ( PTNODE * node, char * wordlist )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Attempting to insert space sep. words into non-existent node\n" );
    return NULL;
  }

  int indx = 0, i = 0;
  int nodecount = 0;
  int len = 0, spacecount = 0;
  int last_was_space = 0;

  // Count spaces and maximum token length
  while ( wordlist [i] != '\0' )
  {
    if ( wordlist [ i++ ] <= 32 )
    {
      last_was_space = 1;
      spacecount++;
      while ( wordlist [i] != '\0' && wordlist [ i ] <= 32 ) i++;
    }
    else
      last_was_space = 0;

    len++;
  }

  // Set the number of children as spaces / spaces + 1 depending on last char
  node = setNumChildren ( node, last_was_space ? spacecount : spacecount + 1 );
  node = allocateChildren ( node );

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

    setParseTreeNodeName ( node -> next [nodecount++] , buffer );

  } while ( indx < len );

  return node;
}

/**
 * Function that gets the first node in the pre-order traversal
 * of the subtree rooted at the given node
 *
 * @param node PTNODE* The target parse tree node
 *
 * @return PTNODE* The left most descendant
 *
 */

PTNODE* getLeftMostDesc ( PTNODE *node )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Cannot get left-most descendent of a non-existent node\n" );
    return NULL;
  }

  while ( node -> next != NULL )
  {
    node = node -> next [0];
    if ( node == NULL )
    {
      fprintf ( stderr, "Potential error, tree might have been populated incorrectly\n" );
      return NULL;
    }
  }

  return node;
}

/**
 * Function that gets the next node in a pre-order traversal
 * of the given tree
 *
 * @param node PTNODE* The current parse tree node
 *
 * @return PTNODE* The next node in a pre-order traversal
 *
 */

PTNODE* getNextPreOrder ( PTNODE *node )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Cannot get next pre-order of non-existent node\n" );
    return NULL;
  }

  if ( node -> next == NULL )
  {
    // Is a leaf node
    if ( node -> parent == NULL )
    {
      fprintf ( stderr, "Leaf node has no parent? Either root or a weird tree\n" );
      return NULL;
    }

    if ( node -> child_id < ( node -> parent -> num_of_children - 1 ) )
    {
      // Node has a sibling that should be visited next
      int this_id = node -> child_id;
      node = node -> parent;

      // Return the left-most descendent of the next ordered sibling of node
      return getLeftMostDesc ( node -> next [ this_id + 1 ] );
    }
    else
    {
      // Node is a last leaf child, find its ancestor which is not a last child
      // and get the left most desc of its next ordered sibling
      do
      {
        node = node -> parent;
        if ( node == NULL || node -> parent == NULL )
        {
          fprintf ( stderr, "No next pre order node exists\n" );
          return NULL;
        }

        if ( node -> child_id < ( node -> parent -> num_of_children - 1 ) )
        {
          // Found the required ancestor
          int this_id = node -> child_id;
          node = node -> parent;

          return getLeftMostDesc ( node -> next [ this_id + 1 ] );
        }
      } while ( TRUE );
    }
  }
  else
  {
    fprintf ( stderr, "Passed node is an internal node, should be a leaf node\n" );
    return NULL;
  }

  // If for some reason this line gets exectued, then our programming system
  // has broken down, C has failed us, so the return value is the least of our
  // concerns..
  return getLeftMostDesc ( node );
}

