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

  ast->name = NULL;
  ast->root = malloc ( sizeof ( ANODE ) );
  if ( ast->root == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for the root\n" );
    return NULL;
  }

  initializeNode ( ast->root );

  // Set root children = 0
  ast->root->num_of_children = -1;

  return ast;
}

AST* setAstName ( AST *ast, const char *name )
{
  if ( ast == NULL )
  {
    fprintf ( stderr, "Cannot set the name of a non-existent Ast\n" );
    return NULL;
  }

  if ( name == NULL )
  {
    if ( ast->name != NULL )
    {
      free ( ast->name );
      ast->name = NULL;
    }
  }
  else
  {
    if ( ast->name != NULL )
      free ( ast->name );
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

  node->name = NULL;
  node->num_of_children = -1;

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
    fprintf ( stderr, "Potential, attempting to insert an empty string?\n" );
    return NULL;
  }

  else
  {
    if ( node->name != NULL )
      free ( ast->name );
    int len = strlen ( name );
    node->name = malloc ( (len+1) * sizeof (char) );
    if ( node->name == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for node's name\n" );
      return NULL;
    }
    strcpy ( node->name, name );
  }
}

ANODE* setChildren ( ANODE *node, int value )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Attempting to set value of non-existent node\n" );
    return NULL;
  }

  node->num_of_children = value;
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

  node -> next = malloc ( (node -> num_of_children) * sizeof (ANODE) );
  int i;
  for( i = 0 ; i < node -> num_of_children ; i++)
  	initializeNode ( node -> next[i]; );
  
  return node;
}


ANODE* insertSpaceSeparatedWords ( ANODE * node, char * wordlist )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Attempting to insert space sep. words into non-existent node\n" );
    return NULL;
  }
  
  if ( node -> next == NULL )
  {
  	fprintf ( stderr, "Allocate memory for children before inserting\n" );
    return NULL;
  }

  int indx = 0;
  int nodecount = 0;
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
    
    setNodeName ( node -> next[nodecount++] , buffer);
    
  } while ( indx < len );

  return node;
}
