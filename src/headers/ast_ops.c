#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

#ifndef DEBUG_FLAGS_DEFINED
  #include "debug.h"
#endif

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
  node -> line_no = -1;
  node -> extra_data . data_type = 0;

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
  fprintf ( dumpfile, "%d %d ", node -> num_of_children, node -> line_no );

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
  fscanf ( dumpfile, "%d", & ( createdNode -> line_no ) );

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

/**
 * Function that returns the first child of the provided node (if any)
 * or NULL
 *
 * @param  node ANODE* The target node
 *
 * @return ANODE* The first child of the given node
 **/

ANODE* getFirstChild ( ANODE *node )
{
  if ( node -> num_of_children == 0 )
  {
    fprintf ( stderr, "Cannot get first child of node with no children\n" );
    return NULL;
  }

  return * ( ANODE ** ) ( node -> children -> head -> data . generic_val );
}

/**
 * Function that returns the second child of the provided node (if any)
 * or NULL
 *
 * @param  node ANODE* The target node
 *
 * @return ANODE* The second child of the given node
 **/

ANODE* getSecondChild ( ANODE *node )
{
  if ( node -> num_of_children < 2 )
  {
    fprintf ( stderr, "Cannot get non existent second child of node\n" );
    return NULL;
  }

  return * ( ANODE ** ) ( node -> children -> head -> next -> data . generic_val );
}

/**
 * Function that returns the third child of the provided node (if any)
 * or NULL
 *
 * @param  node ANODE* The target node
 *
 * @return ANODE* The third child of the given node
 **/

ANODE* getThirdChild ( ANODE *node )
{
  if ( node -> num_of_children < 3 )
  {
    fprintf ( stderr, "Cannot get non existent 3rd child\n" );
    return NULL;
  }

  return * ( ANODE ** ) ( node -> children -> head -> next -> next -> data . generic_val );
}

/**
 * Function that returns the fourth child of the provided node (if any)
 * or NULL
 *
 * @param  node ANODE* The target node
 *
 * @return ANODE* The fourth child of the given node
 **/

ANODE* getFourthChild ( ANODE *node )
{
  if ( node -> num_of_children < 4 )
  {
    fprintf ( stderr, "Cannot get non existent 4th child\n" );
    return NULL;
  }

  return * ( ANODE ** ) ( node -> children -> head -> next -> next -> next -> data . generic_val );
}

/**
 * Function that returns the fifth child of the provided node (if any)
 * or NULL
 *
 * @param  node ANODE* The target node
 *
 * @return ANODE* The fifth child of the given node
 **/

ANODE* getFifthChild ( ANODE *node )
{
  if ( node -> num_of_children < 5 )
  {
    fprintf ( stderr, "Cannot get non existent 5th child\n" );
    return NULL;
  }

  return * ( ANODE ** ) ( node -> children -> head -> next -> next -> next -> next -> data . generic_val );
}

/**
 * Function that rotates a given node left and returns the pointer
 * to the rotated node.
 *
 * @param  node ANODE* The node to rotate
 *
 * @return ANODE* The left rotated node
 *
 **/

ANODE* rotateLeft ( ANODE *node )
{
  if ( node == NULL )
  {
    fprintf ( stderr, "Cannot left-rotate a non-existent node\n" );
    return NULL;
  }

  if ( node -> num_of_children != 2 || getSecondChild ( node ) -> num_of_children != 2 )
  {
    fprintf ( stderr, "Node that is to be rotated left and its right child must have exactly 2 children\n" );
    return NULL;
  }

  /*
   * Transform    a      to       c
   *             / \             / \
   *            b   c           a   e
   *               / \         / \
   *              d   e       b   d
   *
   */

  ANODE *a = node;
  ANODE *c = getSecondChild ( a );
  ANODE *d = getFirstChild ( c );

  // Need to modify nodes a and c
  // So we need to modify values in their linked lists of children

  LINKEDLIST *achildren = a -> children;
  LINKEDLIST *cchildren = c -> children;

  // Set a's right child as d
  // And d's parent as a
  memcpy ( achildren -> head -> next -> data . generic_val, &d, sizeof ( d ) );
  d -> parent = a;

  // Set c's parent as a's parent
  // Then set a's parent as c
  c -> parent = a -> parent;
  a -> parent = c;

  // Set c's left child as a
  memcpy ( cchildren -> head -> data . generic_val, &a, sizeof ( a ) );

  return c;
}

/**
 * Function that gets the data type associated
 * with a node
 *
 * @param currnode ANODE The node whose data type is to be retrieved
 *
 * @return DATATYPE The data type of the node
 */

DATATYPE getDataType ( ANODE *currnode )
{
  if ( currnode == NULL )
  {
    fprintf ( stderr, "Cannot get data type of child of non-existent node\n" );
    exit (-1);
    return -1;
  }

  if ( currnode -> num_of_children <= 0 )
  {
    fprintf ( stderr, "Trying to get data type of child of node with no children\n" );
    exit (-1);
    return -1;
  }

  ANODE *firstchild = *( ( ANODE ** ) ( currnode -> children -> head -> data . generic_val ) );

  return firstchild -> extra_data . data_type;
}

/**
 * Get the name of a node given its type
 *
 * @param type DATATYPE the type of the node
 *
 * @return const char* The name of the node
 */

const char* getNodeTypeName ( int type )
{
  return nodeTypes [type];
}

/**
 * Dump the AST details in a pre-order traversal
 * format to the given file
 *
 * @param node ANODE* The AST root for the subtree to dump
 * @param astdumpfile FILE* The file to dump to
 */

void preOrderDumpAst ( ANODE *node, FILE *astdumpfile )
{
  dumpNode ( node, astdumpfile );

  LNODE iterator;
  getIterator ( node -> children, &iterator );

  while ( hasNext ( &iterator ) )
  {
    getNext ( node -> children, &iterator );

    preOrderDumpAst ( * ( ANODE ** ) ( iterator . data . generic_val ), astdumpfile );
  }
}

/**
 * Function that reads and creates the AST structure from
 * a pre order dump file generated using the preOrderDumpAst
 * method
 *
 * @param node ANODE* The root node of the AST to be populated
 * @param astdumpfile FILE* The dump file to read from
 */

void readAstDumpFile ( ANODE *node, FILE *astdumpfile )
{
  if ( DEBUG_AST_CONSTRUCTION )
    printf ( "At node: %s\n", getNodeTypeName ( node -> node_type ) );

  ANODE *createdNode = readDumpNode ( node, astdumpfile );

  createdNode -> parent = node;

  if ( DEBUG_AST_CONSTRUCTION )
    printf ( "Created node: %s with %d children\n\n",
             getNodeTypeName ( createdNode -> node_type ), createdNode -> num_of_children );

  int i;
  int childcount = createdNode -> num_of_children;

  // addChild will modify this too, so we must reset it after getting the info for the loop below
  createdNode -> num_of_children = 0;

  for ( i = 0; i < childcount; i++ )
  {
    if ( DEBUG_AST_CONSTRUCTION )
      printf ( "Child %d / %d for %s:\n\n", i, childcount,
               getNodeTypeName ( createdNode -> node_type ) );

    readAstDumpFile ( createdNode, astdumpfile );
  }

  if ( DEBUG_AST_CONSTRUCTION )
    printf ( "Done with: %s\n\n", getNodeTypeName ( createdNode -> node_type ) );
}

/**
 * Function that handles symbol table specific operations while
 * reading from a dump file
 *
 * @param currnode ANODE* The node being inspected
 * @param symboltable SYMBOLTABLE* The symbol table to populate
 * @param stbdumpfile FILE* The dump file to read from
 *
 */

void handleTypeSpecificActions ( ANODE *currnode, SYMBOLTABLE *symboltable, FILE *stbdumpfile )
{
  ANODE *parent = currnode -> parent;

  if ( currnode -> node_type == AST_IDENTIFIER_NODE
       || currnode -> node_type == AST_LITERAL_NODE )
  {
    if ( parent -> node_type == AST_FUNCTION_NODE
         || parent -> node_type == AST_GLOBALDEFINE_NODE
         || parent -> node_type == AST_DEFINE_NODE
         || parent -> node_type == AST_QUALIFIEDPARAMETER_NODE )
    {
      // Entry to be read from dump file is a define entry
      unsigned int entryindex = readDumpEntry ( symboltable, stbdumpfile, 'd' );

      if ( DEBUG_STB_AUXOPS )
      {
        char *name = NULL;
        if ( parent -> node_type == AST_FUNCTION_NODE )
          name = getEntryByIndex ( symboltable, entryindex ) -> data . func_data . name;
        else
          name = getEntryByIndex ( symboltable, entryindex ) -> data . var_data . name;

        printf ( "Definition of %s registered\n", name);
      }

      VARIABLE *vardata = & ( getEntryByIndex ( symboltable, entryindex ) -> data . var_data );

      if ( parent -> node_type == AST_GLOBALDEFINE_NODE )
        vardata -> var_type = V_GLOBAL_TYPE;
      else if ( parent -> node_type == AST_QUALIFIEDPARAMETER_NODE )
        vardata -> var_type = V_PARAM_TYPE;
      else if ( parent -> node_type == AST_DEFINE_NODE )
        vardata -> var_type = V_LOCAL_TYPE;

      currnode -> extra_data . symboltable_index = entryindex;
    }
    else
    {
      // Entry to be read is a variable reference entry (or a literal entry)
      // Perform the same action for both
      unsigned int entryindex = readDumpEntry ( symboltable, stbdumpfile, 'r' );

      if ( DEBUG_STB_AUXOPS )
      {
        char *name = NULL;
        if ( currnode -> node_type == AST_LITERAL_NODE )
        {
          printf ( "sdf %s\n", getEntryByIndex ( symboltable, entryindex ) -> data . lit_data . value );
          name = getEntryByIndex ( symboltable, entryindex ) -> data . lit_data . value;
        }
        else if ( getEntryByIndex ( symboltable, entryindex ) -> entry_type == ENTRY_FUNC_TYPE )
          name = getEntryByIndex ( symboltable, entryindex ) -> data . func_data . name;
        else
          name = getEntryByIndex ( symboltable, entryindex ) -> data . var_data . name;

        printf ( "Reference of %s registered\n", name );
      }

      currnode -> extra_data . symboltable_index = entryindex;
    }
  }
  else if ( currnode -> node_type == AST_QUALIFIEDPARAMETERS_NODE )
  {
    // Parent WILL be a function node, so set its number and type of parameters
    ANODE *funciden = getFirstChild ( parent );

    unsigned int funcindex = funciden -> extra_data . symboltable_index;

    STBENTRY *funcentry = getEntryByIndex ( symboltable, funcindex );

    if ( funcentry == NULL )
    {
      fprintf ( stderr, "Function entry not found in symbol table for some reason...\n" );
      exit (-1);
    }

    FUNCTION *funcdata = & ( funcentry -> data . func_data );

    // The number of parameters of the function = num of children of qualified parameters node
    funcdata -> num_params = currnode -> num_of_children;

    // Iterate over children of qualified parameters node, each one is a qualified parameter
    // whose first child is a datatype node from which we can extract the data types of the
    // parameters to this function
    LNODE iterator;

    getIterator ( currnode -> children, & iterator );

    while ( hasNext ( & iterator ) )
    {
      getNext ( currnode -> children, & iterator );

      ANODE *qualifiedparamnode = * ( ANODE ** ) ( iterator . data . generic_val );
      DATATYPE dtype = getFirstChild ( qualifiedparamnode ) -> extra_data . data_type;

      funcdata -> paramtypes = insertAtBack ( funcdata -> paramtypes, & dtype );

      if ( DEBUG_STB_AUXOPS )
        printf ( "Set the data type of %s's param as %s\n", funcdata -> name, getDataTypeName ( dtype ) );
    }
  }
  else if ( currnode -> node_type == AST_RETURNTYPE_NODE )
  {
    // A returntype node could have no children, in which case the return type is nothing

    ANODE *returniden = getFirstChild ( parent );
    unsigned int retindex = returniden -> extra_data . symboltable_index;
    STBENTRY *retentry = getEntryByIndex ( symboltable, retindex );

    if ( retentry == NULL )
    {
      fprintf ( stderr, "Return type entry not found in the symbol table" );
      exit (-1);
    }

    FUNCTION *funcdata = & ( retentry -> data . func_data );
    if ( currnode -> num_of_children > 0 )
      funcdata -> ret_type = getFirstChild ( currnode ) -> extra_data . data_type;
    else
      funcdata -> ret_type = D_NOTHING_TYPE;

    if ( DEBUG_STB_AUXOPS )
      printf ( "Set the return type of %s as %s\n", funcdata -> name, getDataTypeName ( funcdata -> ret_type ) );

  }
}

