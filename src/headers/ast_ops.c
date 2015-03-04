#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

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

  initializeNode ( ast->root );

  // Set root's num children to sentinel value
  ast -> root -> num_of_children = -1;

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

ANODE* initializeNode ( ANODE *node )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Attempting to initialize non-existent node\n" );
    return NULL;
  }

  node -> name = NULL;
  node -> num_of_children = -1;
  node -> next = NULL;

  return node;
}

ANODE* setNodeName ( ANODE *node, const char *str )
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

    initializeNode ( node -> next [i] );
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

    setNodeName ( node -> next[nodecount++] , buffer);

  } while ( indx < len );

  return node;
}

