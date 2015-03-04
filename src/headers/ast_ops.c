#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

#define TRUE 1

AST* getNewAst ()
{
  AST* ast = NULL;
  ast = malloc ( sizeof (AST) );

  if ( ast == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for Ast\n" );
    return NULL;
  }

  ast -> name = NULL;
  ast -> root = malloc ( sizeof ( ANODE ) );
  if ( ast -> root == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for the root\n" );
    return NULL;
  }

  // Initialize root with parent as NULL
  initializeAstNode ( ast->root, NULL );

  // Set root's num children to sentinel value
  ast -> root -> num_of_children = -1;
  ast -> root -> child_id = 0;

  return ast;
}

AST* setAstName ( AST *ast, const char *name )
{
  if ( ast == NULL )
  {
    fprintf ( stderr, "Cannot set the name of a non-existent Ast\n" );
    return NULL;
  }

  if ( ast -> name != NULL )
  {
    free ( ast -> name );
    ast -> name = NULL;
  }

  if ( name != NULL )
  {
    int len = strlen ( name );
    ast->name = malloc ( (len+1) * sizeof (char) );
    if ( ast->name == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for Ast's name\n" );
      return NULL;
    }
    strcpy ( ast->name, name );
  }

  return ast;
}

ANODE* initializeAstNode ( ANODE *node, ANODE *parent )
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

ANODE* setAstNodeName ( ANODE *node, const char *str )
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

ANODE* setNumChildren ( ANODE *node, int value )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Attempting to set value of non-existent node\n" );
    return NULL;
  }

  if ( value <= 0 )
    fprintf ( stderr, "Attempting to set AST node's num children <= 0\n" );

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

ANODE* allocateChildren ( ANODE * node )
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
    fprintf ( stderr, "Cannot allocate negative number of children for AST node\n" );
    return NULL;
  }

  // If next array has already been populated, free it
  if ( node -> next != NULL )
    free ( node -> next );

  node -> next = malloc ( (node -> num_of_children) * sizeof (ANODE *) );

  int i;
  for( i = 0 ; i < node -> num_of_children ; i++)
  {
    node -> next [i] = NULL;
    node -> next [i] = malloc ( sizeof ( ANODE ) );
    if ( node -> next [i] == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for child" );
      return NULL;
    }

    initializeAstNode ( node -> next [i], node );
    node -> next [i] -> child_id = i;
  }

  return node;
}

ANODE* insertSpaceSeparatedWords ( ANODE * node, char * wordlist )
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

    setAstNodeName ( node -> next [nodecount++] , buffer );

  } while ( indx < len );

  return node;
}

ANODE* getLeftMostDesc ( ANODE *node )
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

ANODE* getNextPreOrder ( ANODE *node )
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

