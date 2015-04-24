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
#include "ast.h"

#define TRUE 1
#define MAXNAMELEN 50

/**
 * Function that allocates memory for, and returns a pointer
 * to a new Abstract Syntax Tree
 *
 * @return AST* The pointer to the allocated AST
 *
 **/

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

/**
 * Function set or clears the name of an AST. If the provided
 * name is NULL, it clears current name of the AST.
 *
 * @param  ast  AST* The AST whose name is to be set
 * @param  name char* The name to be used
 *
 * @return AST* The pointer to the altered AST
 *
 **/

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

/**
 * Function that adds a mapping between a node name
 * and an integer in the internal dictionary of the
 * provided AST.
 *
 * @param  ast AST* The AST whose dictionary must be modified
 * @param  name char* The name of the node, which is the key
 * @param  value int The value to be set for the key name
 *
 * @return AST* The pointer to the altered AST
 *
 **/

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

  // Assumes that the type mapping being inserted doesn't already exist
  TNODE *insertedNode = NULL;
  insertedNode = insertString ( ast -> node_typemap , name );

  insertedNode -> data . int_val = value;

  ast -> num_types ++;

  return ast;
}

/**
 * Function that returns the value for the given key in the
 * internal type mapping dictionary of the given AST
 *
 * @param ast AST* The AST whose dictionary must be queried
 * @param name char* The key for which the value must be found
 *
 * @return int The value for the key in the type map.
 *             If the key doesn't exist, -1 is returned.
 *
 **/

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

/**
 * Function that initializes an AST Node with sentinal values
 * and creates a linked list of pointers to its children.
 *
 * @param  node ANODE* The AST node that must be initialized
 * @param  parent ANODE* The parent of the node to be initialized
 *
 * @return ANODE* The initialized AST node
 *
 **/

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
  node -> result_type = D_TYPE_FIRST;
  node -> children = getLinkedList ( LL_GENERIC_TYPE );
  node -> children = setGenericSize ( node -> children, sizeof ( ANODE * ) );
  node -> offsetcount = 0;
  node -> offsetreg = 0;
  node -> offset1 = 0;
  node -> offset2 = 0;
  node -> offset3 = 0;
  node -> extra_data . data_type = 0;
  node -> extra_data . iflabel = 0;

  return node;
}

/**
 * Function that sets the type of an AST node
 *
 * @param  node ANODE* The AST node whose type must be set
 * @param  type int The type that the node must be assigned
 *
 * @return ANODE* The modified AST node
 *
 **/

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

/**
 * Function that sets or clears the name of an AST node.
 * If the paramemter provided is NULL, the name is cleared.
 *
 * @param  node ANODE* The AST node whose name must be set
 * @param  str char* The name which must be set
 *
 * @return ANODE* The modified AST node
 *
 **/

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

/**
 * Function that returns the parent of a given AST node
 *
 * @param  node ANODE* The AST node whose parent is required
 *
 * @return ANODE* The parent of the provided node
 *
 **/

ANODE* getParent ( ANODE *node )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Cannot get parent of non-existent node\n" );
    return NULL;
  }

  return node -> parent;
}

/**
 * Function that adds a child to an existing AST node.
 *
 * @param  node ANODE* The AST node to which the child
 *                     must be added
 * @param  type int The type of the child node
 * @param  action int The action to be taken on adding
 *                    the child, encoded into the bits
 *
 * @return ANODE* The node to be returned depending on
 *                the action, either the child, current
 *                node, or the parent
 *
 **/

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

/**
 * Function that dumps the data in a node to the dump file.
 * The function has the side effect of moving the file pointer.
 *
 * @param  node ANODE* The node to dump
 * @param  dumpfile FILE* The open file to dump to
 *
 * @return ANODE* The dumped node, intact if the dump was successful
 *
 */

ANODE* dumpNode  ( ANODE *node, FILE *dumpfile )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Cannot dump non existent AST node\n" );
    return NULL;
  }

  if ( dumpfile == NULL )
  {
    fprintf ( stderr, "Cannot dump AST node to non existent file\n" );
    return NULL;
  }

  fprintf ( dumpfile, "%d %d\n", node -> node_type, ( node -> name != NULL ) );
  if ( node -> name != NULL )
    fprintf ( dumpfile, "%s\n", node -> name );
  fprintf ( dumpfile, "%d ", node -> num_of_children );

  // ************************ Important note: ****************************
  // All members of the union are integers, so it doesn't really matter
  // which one is dumped. But needs to be checked if extra_data is changed.
  fprintf ( dumpfile, "%d\n", node -> extra_data . data_type );

  return node;
}

/**
 * Function that reads the next dump from the dump file pointed to by
 * the parameter passed. The function has the side effect of moving
 * the file pointer.
 *
 * @param  parent ANODE* The parent of the node to be created
 * @param  dumpfile FILE* The open dump file to read from
 *
 * @return ANODE* The pointer to the created node if successful
 *
 */

ANODE* readDumpNode  ( ANODE *parent, FILE *dumpfile )
{
  if ( parent == NULL )
  {
    fprintf ( stderr, "Parent node cannot be NULL while reading from AST dump\n" );
    return NULL;
  }

  if ( dumpfile == NULL )
  {
    fprintf ( stderr, "Cannot read AST node from a non existent dump file\n" );
    return NULL;
  }

  char buffer [ MAXNAMELEN ];
  int type_to_create = 0, hasname = 0;

  fscanf ( dumpfile, "%d", & type_to_create );
  fscanf ( dumpfile, "%d", & hasname );

  if ( hasname )
    fscanf ( dumpfile, "%s", buffer );

  ANODE *createdNode = addChild ( parent, type_to_create, GOTOCH );

  if ( hasname )
    createdNode = setAstNodeName ( createdNode, buffer );

  fscanf ( dumpfile, "%d", & ( createdNode -> num_of_children ) );

  // ************************ Important note: ****************************
  // All members of the union are integers, so it doesn't really matter
  // which one is assigned from the dump read. Needs to be re-checked
  // if extra_data is ever changed.

  int union_value = 0;
  fscanf ( dumpfile, "%d", & union_value );
  createdNode -> extra_data . data_type = union_value;

  return createdNode;
}

/**
 * Function that converts an input instruction string
 * into the corresponding binary encoding, stored in
 * an integer
 *
 * @param  instruction char* The string that has the
 *                           instruction as chars
 *
 * @return int The integer encoding on the instruction
 *
 **/

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

