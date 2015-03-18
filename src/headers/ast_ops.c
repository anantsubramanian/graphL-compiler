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
  ast -> num_types = 0;
  ast -> root = malloc ( sizeof ( ANODE ) );
  if ( ast -> root == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for the root\n" );
    return NULL;
  }

  // Initialize root with parent as NULL
  initializeAstNode ( ast->root, NULL );

  ast -> node_typemap = NULL;
  ast -> node_typemap = getNewTrie ( TRIE_INT_TYPE );

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

AST* addTypeMap ( AST *ast, const char *name, int value )
{
  // Assumes mapping being added doesn't exist in the typemap
  if ( ast == NULL )
  {
    fprintf ( stderr, "Cannot add type mapping to non-existent AST\n" );
    return NULL;
  }

  if ( name == NULL )
  {
    fprintf ( stderr, "Cannot add type mapping for non-existent string\n" );
    return NULL;
  }

  TNODE *insertedNode = NULL;
  insertedNode = insertString ( ast -> node_typemap , name );

  insertedNode -> data . int_val = value;

  ast -> num_types ++;

  return ast;
}

int getMapValue ( AST *ast, const char *name )
{
  if ( ast == NULL )
  {
    fprintf ( stderr, "Cannot get type mapping from non-existent AST\n" );
    return -1;
  }

  if ( name == NULL )
  {
    fprintf ( stderr, "Cannot get type mapping for non-existent string\n" );
    return -1;
  }

  TNODE *foundNode = NULL;
  foundNode = findString ( ast -> node_typemap, name );

  if ( foundNode == NULL )
    return -1;

  return foundNode -> data . int_val;
}

ANODE* initializeAstNode ( ANODE *node, ANODE *parent )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Attempting to initialize non-existent node\n" );
    return NULL;
  }

  node -> name = NULL;
  node -> node_type = -1;
  node -> num_of_children = 0;
  node -> parent = parent;
  node -> children = getLinkedList ( LL_GENERIC_TYPE );
  node -> children = setGenericSize ( node -> children, sizeof ( ANODE * ) );
  node -> extra_data . data_type = 0;

  return node;
}

ANODE* setNodeType ( ANODE *node, int type )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Cannot set node type for non-existent node\n" );
    return NULL;
  }

  node -> node_type = type;

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

ANODE* getParent ( ANODE *node )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Cannot get parent of non-existent node\n" );
    return NULL;
  }

  return node -> parent;
}

ANODE* addChild ( ANODE *node, int type, int action )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Cannot add child to non-existent node\n" );
    return NULL;
  }

  node -> num_of_children ++;

  ANODE* newChild = NULL;
  newChild = malloc ( sizeof ( ANODE ) );

  if ( newChild == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for AST child\n" );
    return NULL;
  }

  newChild = initializeAstNode ( newChild, node );
  newChild -> node_type = type;

  // Copy the address data from the pointer to the created node
  // into the linkedlist, i.e., a linked list of pointers
  // of void type. Should be dereferenced as pointers of ANODE*
  // type and then they are pointers to child nodes
  node -> children = insertAtBack ( node -> children, & newChild );

  if ( action & GOTOCH )
    return newChild;
  else if ( action & PARENT )
  {
    if ( node -> parent == NULL )
      return node;
    else
      return node -> parent;
  }
  else
    return node;
}

int createProperty ( char *instruction )
{
  if ( instruction == NULL )
  {
    fprintf ( stderr, "Cannot get property from non-existent instruction string\n" );
    return -1;
  }

  if ( strlen ( instruction ) < 3 )
  {
    fprintf ( stderr, "Cannot get property as the length of the instruction is not 3\n" );
    return -1;
  }

  int property = 0;

  if ( instruction [0] == 'C' )
    property |= CREATE;

  if ( instruction [1] == 'G' )
    property |= GOTOCH;
  else if ( instruction [1] == 'P' )
    property |= PARENT;
  else if ( instruction [1] == 'C' )
    property |= CONDRD;

  if ( instruction [2] == 'R' )
    property |= READ;

  return property;
}

