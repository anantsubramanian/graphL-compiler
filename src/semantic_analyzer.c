#include <stdio.h>
#include "headers/ast.h"

#ifndef SYMBOLTABLE_DEFINED
  #include "headers/symboltable.h"
#endif

int semantic_error = 0;

int checkSemanticError ()
{
  return semantic_error;
}

/**
 * Checks if specific keywords are only present in their valid locations
 * in the given program
 */

int checkInvalidKeywordLocation ( ANODE *currnode, int *infunction,
                                  int *loopcount, int *bdftcount )
{
  if ( currnode -> node_type == AST_BREAK_NODE && *loopcount <= 0 )
  {
    fprintf ( stderr,
              "Error at line %d:\n\tBreak statements can only occur inside loops.\n\n",
              currnode -> line_no );

    // Continue processing after ignoring the node
    return 1;
  }
  else if ( currnode -> node_type == AST_RETURNSTMT_NODE && *infunction == 0 )
  {
    fprintf (
      stderr,
      "Error at line %d:\n\tReturn statements can only occur inside function definitions.\n\n",
      currnode -> line_no
    );
    return 1;
  }
  else if ( currnode -> node_type == AST_DEPTH_NODE && *loopcount <= 0 && *bdftcount <= 0 )
  {
    fprintf (
      stderr,
      "Error at line %d:\n\tDepth can only be used inside a BFT / DFT based loop.\n\n",
      currnode -> line_no
    );
    return 1;
  }

  return 0;
}

/**
 * Checks that both sides of an assignment statement have the same type
 * or can be implicitly type casted to the required type
 */

int checkInvalidAssignment ( ANODE *currnode, SYMBOLTABLE *symboltable )
{
  if ( currnode -> node_type == AST_LET_NODE )
  {
    // The left and right children of Let node should have the same type
    if ( getFirstChild ( currnode ) -> result_type != getSecondChild ( currnode ) -> result_type )
    {
      STBENTRY *entry =
        getEntryByIndex ( symboltable, getFirstChild ( getFirstChild ( currnode ) )
          -> extra_data . symboltable_index
        );
      char *name = entry -> data . var_data . name;
      if ( getFirstChild ( currnode ) -> result_type == D_FLOAT_TYPE
           && getSecondChild ( currnode ) -> result_type == D_INT_TYPE )
      {
        fprintf (
          stderr,
          "Warning at line %d:\n\tImplicit conversion from Int to Float in Let statement\n",
          currnode -> line_no
        );
        fprintf ( stderr, "\tNote: %s has type Float\n\n", name );
      }
      else
      {
        fprintf ( stderr,
                  "Error at line %d:\n\tAssigning incompatible types in let statement\n",
                  currnode -> line_no );
        if ( getFirstChild ( currnode ) -> num_of_children == 1 )
          fprintf ( stderr,
                    "\tNote: %s has type %s, but RHS has type %s\n\n",
                    name,
                    getDataTypeName ( getFirstChild ( currnode ) -> result_type ),
                    getDataTypeName ( getSecondChild ( currnode ) -> result_type ) );
        else
          fprintf ( stderr,
                    "\tNote: LHS has type %s, but RHS has type %s\n\n",
                    getDataTypeName ( getFirstChild ( currnode ) -> result_type ),
                    getDataTypeName ( getSecondChild ( currnode ) -> result_type ) );

        return 1;
      }
      return 0;
    }

    STBENTRY *entry = getEntryByIndex (
        symboltable, getFirstChild ( getFirstChild ( currnode ) ) -> extra_data . symboltable_index );

    if ( entry -> entry_type == ENTRY_FUNC_TYPE )
    {
      fprintf ( stderr,
                "Error at line %d:\n\tAttempting to assign to a function identifier\n",
                currnode -> line_no );
      fprintf ( stderr,
                "\tNote: %s declared as function on line %d\n\n",
                entry -> data . func_data . name,
                entry -> data . func_data . decl_line );
      return 1;
    }
  }

  return 0;
}

/**
 * Checks whether the parameters given for a dereference (referencing
 * the member of an object) are valid.
 */

int checkInvalidDereference ( ANODE *currnode, SYMBOLTABLE *symboltable )
{
  if ( currnode -> node_type == AST_ASSIGNABLE_NODE )
  {
    // The type of an assignable node is either the type of the identifier
    // or the member of the identifier if there are more than one children

    if ( currnode -> num_of_children == 1 )
      currnode -> result_type = getFirstChild ( currnode ) -> result_type;
    else if ( currnode -> num_of_children == 2 )
    {
      fprintf ( stderr, "Invalid tree produced! Assignable can't have exactly 2 children\n" );
      return 1;
    }
    else if ( currnode -> num_of_children == 3 )
    {
      // If Assignable has three children, then its first child should be a complex type
      currnode -> result_type = getThirdChild ( currnode ) -> result_type;
      STBENTRY *entry =
        getEntryByIndex ( symboltable, getFirstChild ( currnode )
          -> extra_data . symboltable_index
        );

      if ( getFirstChild ( currnode ) -> result_type == D_STRING_TYPE
           || getFirstChild ( currnode ) -> result_type == D_INT_TYPE
           || getFirstChild ( currnode ) -> result_type == D_FLOAT_TYPE )
      {
        fprintf ( stderr,
                  "Error at line %d:\n\tCannot get members of primitive type\n",
                  currnode -> line_no );
        fprintf ( stderr,
                  "\tNote: %s was declared as a/an %s on line %d\n\n",
                  entry -> data . var_data . name,
                  getDataTypeName ( getFirstChild ( currnode ) -> result_type ),
                  entry -> data . var_data . decl_line );
        return 1;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_VERTEX_TYPE
           && getThirdChild ( currnode ) -> num_of_children > 0 )
      {
        fprintf (
          stderr,
          "Error at line %d:\n\tCannot get non-primitive type member of a Vertex object\n",
          currnode -> line_no
        );
        fprintf ( stderr,
                  "\tNote: %s was declared as a VERTEX on line %d\n\n",
                  entry -> data . var_data . name,
                  entry -> data . var_data . decl_line );
        return 1;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_EDGE_TYPE )
      {
        if ( getThirdChild ( currnode ) -> num_of_children > 0
             && getFirstChild ( getThirdChild ( currnode ) ) -> node_type == AST_ROOT_NODE )
        {
          fprintf ( stderr, "Error at line %d:\n\tEdge object has no root member\n", currnode -> line_no );
          fprintf ( stderr,
                    "\tNote: %s was declared as an EDGE on line %d\n\n",
                    entry -> data . var_data . name,
                    entry -> data . var_data . decl_line );
          return 1;
        }
      }

      if ( getFirstChild ( currnode ) -> result_type == D_GRAPH_TYPE )
      {
        fprintf ( stderr,
                  "Error at line %d:\n\tGraph object has no members that can be referenced\n",
                  currnode -> line_no );
        fprintf ( stderr,
                  "\tNote: %s was declared as an GRAPH on line %d\n\n",
                  entry -> data . var_data . name,
                  entry -> data . var_data . decl_line );
        return 1;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_TREE_TYPE
           && getThirdChild ( currnode ) -> num_of_children > 0
           && getFirstChild ( getThirdChild ( currnode ) ) -> node_type != AST_ROOT_NODE )
      {
        fprintf (
          stderr,
          "Error at line %d:\n\tOnly root member of a Tree object may be referenced\n",
          currnode -> line_no
        );
        fprintf ( stderr,
                  "\tNote: %s was declared as an TREE on line %d\n\n",
                  entry -> data . var_data . name,
                  entry -> data . var_data . decl_line );
        return 1;
      }
    }
    else
    {
      fprintf ( stderr, "Invalid tree produced! Assignable can't have more than 3 children\n" );
      return 1;
    }
  }
  else if ( currnode -> node_type == AST_ASSIGNFUNC_NODE )
  {
    // ASSIGNFUNC node could have 1, 2 or 3 children

    // If it has one child or two children, then the result type is the type of the
    // first child (i.e. the identifier)
    if ( currnode -> num_of_children == 1 )
      currnode -> result_type = getFirstChild ( currnode ) -> result_type;
    else if ( currnode -> num_of_children == 2 )
    {
      currnode -> result_type = getFirstChild ( currnode ) -> result_type;
      STBENTRY *entry =
        getEntryByIndex ( symboltable, getFirstChild ( currnode )
          -> extra_data . symboltable_index
        );
      if ( entry -> entry_type != ENTRY_FUNC_TYPE )
      {
        fprintf ( stderr,
                  "Error at line %d:\n\tAttempting to call a variable like a function\n",
                  currnode -> line_no );
        fprintf ( stderr,
                  "\tNote: %s declared as a/an %s at line number %d\n\n",
                  entry -> data . var_data . name,
                  getDataTypeName ( entry -> data . var_data . data_type ),
                  entry -> data . var_data . decl_line );
        return 1;
      }
    }
    else if ( currnode -> num_of_children == 3 )
    {
      currnode -> result_type = getThirdChild ( currnode ) -> result_type;
      STBENTRY *entry =
        getEntryByIndex ( symboltable, getFirstChild ( currnode )
          -> extra_data . symboltable_index
        );
      VARIABLE *vardata = & ( entry -> data . var_data );

      if ( getFirstChild ( currnode ) -> result_type == D_STRING_TYPE
           || getFirstChild ( currnode ) -> result_type == D_INT_TYPE
           || getFirstChild ( currnode ) -> result_type == D_FLOAT_TYPE )
      {
        fprintf ( stderr,
                  "Error at line %d:\n\tCannot get members of primitive type\n",
                  currnode -> line_no );
        fprintf ( stderr,
                  "\tNote: %s has type %s\n\n",
                  vardata -> name,
                  getDataTypeName ( vardata -> data_type ) );
        return 1;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_VERTEX_TYPE
           && getThirdChild ( currnode ) -> num_of_children > 0 )
      {
        fprintf (
          stderr,
          "Error at line %d:\n\tCannot get non-primitive type member of a Vertex object\n",
          currnode -> line_no
        );
        fprintf ( stderr,
                  "\tNote: %s declared as a VERTEX on line %d\n\n",
                  vardata -> name,
                  vardata -> decl_line );
        return 1;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_EDGE_TYPE )
      {
        if ( getThirdChild ( currnode ) -> num_of_children > 0
             && getFirstChild ( getThirdChild ( currnode ) ) -> node_type == AST_ROOT_NODE )
        {
          fprintf ( stderr,
                    "Error at line %d:\n\tEdge object has no root member\n",
                    currnode -> line_no );
          fprintf ( stderr,
                    "\tNote: %s declared as an EDGE on line %d\n\n",
                    vardata -> name,
                    vardata -> decl_line );
          return 1;
        }
      }

      if ( getFirstChild ( currnode ) -> result_type == D_GRAPH_TYPE )
      {
        fprintf ( stderr,
                  "Error at line %d:\n\tGraph object has no members that can be referenced\n",
                  currnode -> line_no );
        fprintf ( stderr,
                  "\tNote: %s declared as a GRAPH on line %d\n\n",
                  vardata -> name,
                  vardata -> decl_line );
        return 1;
      }

      if ( getFirstChild ( currnode ) -> result_type == D_TREE_TYPE
           && getThirdChild ( currnode ) -> num_of_children > 0
           && getFirstChild ( getThirdChild ( currnode ) ) -> node_type != AST_ROOT_NODE )
      {
        fprintf ( stderr,
                  "Error at line %d:\n\tOnly the root member of a Tree object may be referenced\n",
                  currnode -> line_no );
        fprintf ( stderr,
                  "\tNote: %s declared as a TREE on line %d\n\n",
                  vardata -> name,
                  vardata -> decl_line );
        return 1;
      }
    }
  }
  else if ( currnode -> node_type == AST_ENDASSIGN_NODE )
  {
    // It could have one or two children. If it has one child then it is the type of that child
    // else it is the type of the current node


    if ( currnode -> num_of_children == 0 )
      currnode -> result_type = currnode -> extra_data . data_type;
    else if ( currnode -> num_of_children == 1 )
      currnode -> result_type = getFirstChild ( currnode ) -> result_type;
    else
    {
      currnode -> result_type = currnode -> extra_data . data_type;

      if ( getFirstChild ( currnode ) -> node_type == AST_WEIGHT_NODE )
      {
        fprintf ( stderr,
                  "Error at line %d:\n\tPrimitive type objects have no members\n\n",
                  currnode -> line_no );
        return 1;
      }
    }
  }

  return 0;
}

/**
 * Performs auxiliary operations on the AST node when an identifier is
 * encountered during analysis
 */

int processIdentifier ( ANODE *currnode, SYMBOLTABLE *symboltable )
{
  if ( currnode -> node_type == AST_IDENTIFIER_NODE )
  {
    // The type of an identifier node is that of the variable in the symbol table, of the return
    // type of the function in the symbol table

    unsigned int index = currnode -> extra_data . symboltable_index;

    STBENTRY *entry = getEntryByIndex ( symboltable, index );

    if ( entry -> entry_type == ENTRY_VAR_TYPE )
      currnode -> result_type = entry -> data . var_data . data_type;
    else if ( entry -> entry_type == ENTRY_FUNC_TYPE )
      currnode -> result_type = entry -> data . func_data . ret_type;

    return 1;
  }

  return 0;
}

/**
 * Checks for errors in the IO (Read and Print) statements
 */

int checkIOErrors ( ANODE *currnode )
{
  if ( currnode -> node_type == AST_READ_NODE )
  {
    // The type of the assignable child of a read node should be an integer or a float

    ANODE *child = getFirstChild ( currnode );

    if ( child -> result_type != D_INT_TYPE && child -> result_type != D_FLOAT_TYPE )
    {
      fprintf ( stderr,
                "Error at line %d:\n\tCannot read non-primitive value using the read statement\n\n",
                currnode -> line_no );
      return 1;
    }
  }
  else if ( currnode -> node_type == AST_PRINT_NODE )
  {
    // The type of the assignable or literal child of a print node should be an integer, a float
    // or a string

    ANODE *child = getFirstChild ( currnode );

    if ( child -> result_type != D_INT_TYPE && child -> result_type != D_FLOAT_TYPE
         && child -> result_type != D_STRING_TYPE )
    {
      fprintf ( stderr,
                "Error at line %d:\n\tCannot print a value that is not a string, an int or a float\n",
                currnode -> line_no );
      return 1;
    }
  }

  return 0;
}

/**
 * Checks for valid type matching between various variables/literals
 * appearing in type expressions, including comparison expressions
 */

int checkExpressionErrors ( ANODE *currnode )
{
  if ( currnode -> node_type == AST_COMPARE_NODE )
  {
    // The two children of a compare node MUST be of the exact same type
    // Otherwise throw an error

    if ( getFirstChild ( currnode ) -> result_type != getSecondChild ( currnode ) -> result_type )
    {
      fprintf (
        stderr,
        "Error at line %d:\n\tValues on the two sides of the compare expression are not the same\n",
        currnode -> line_no
      );
      fprintf ( stderr,
                "\tNote: LHS has type %s and RHS has type %s\n\n",
                getDataTypeName ( getFirstChild ( currnode ) -> result_type ),
                getDataTypeName ( getSecondChild ( currnode ) -> result_type ) );
      return 1;
    }
  }
  else if ( currnode -> node_type == AST_EXP_NODE || currnode -> node_type == AST_AROP_NODE )
  {
    if ( currnode -> num_of_children == 1 )
      currnode -> result_type = getFirstChild ( currnode ) -> result_type;
    else if ( currnode -> num_of_children == 2 )
    {
      DATATYPE firsttype, secondtype, result;

      firsttype = getFirstChild ( currnode ) -> result_type;
      secondtype = getSecondChild ( currnode ) -> result_type;

      if ( firsttype == D_INT_TYPE || firsttype == D_FLOAT_TYPE )
      {
        if ( getFirstChild ( currnode ) -> result_type != getSecondChild ( currnode ) -> result_type )
        {
          fprintf (
            stderr,
            "Error at line %d:\n\tFloats and Ints can only be added to other Floats and Ints respectively\n\n",
            currnode -> line_no
          );
          return 1;
        }

        if ( firsttype == D_FLOAT_TYPE && currnode -> extra_data . arop_type == A_MODULO_TYPE )
        {
          // The right child gives us the operation type
          fprintf ( stderr,
                    "Error at line %d:\n\tModulo operator cannot be applied to Floats\n\n",
                    currnode -> line_no );
          return 1;
        }

        result = firsttype;
      }
      else if ( firsttype == D_STRING_TYPE || secondtype == D_STRING_TYPE )
      {
        fprintf ( stderr,
                  "Error at line %d:\n\tOperations on String are not allowed\n\n",
                  currnode -> line_no );
        return 1;
      }
      else if ( firsttype == D_GRAPH_TYPE || firsttype == D_TREE_TYPE )
      {
        if ( currnode -> extra_data . arop_type != A_PLUS_TYPE
             && currnode -> extra_data . arop_type != A_MINUS_TYPE )
        {
          fprintf ( stderr,
                    "Error at line %d:\n\tOnly addition or removal operations allowed on Graphs and Trees\n\n",
                    currnode -> line_no );
          semantic_error = 1;
        }
        else if ( secondtype != D_VERTEX_TYPE && secondtype != D_EDGE_TYPE )
        {
          fprintf (
            stderr,
            "Error at line %d:\n\tOnly Vertices and Edges may be added and removed from Graphs/Trees\n\n",
            currnode -> line_no
          );
          semantic_error = 1;
        }

        result = firsttype;
      }
      else if ( secondtype == D_GRAPH_TYPE || secondtype == D_TREE_TYPE )
      {
        if ( currnode -> extra_data . arop_type != A_PLUS_TYPE
             && currnode -> extra_data . arop_type != A_MINUS_TYPE )
        {
          fprintf (
            stderr,
            "Error at line %d:\n\tOnly addition or removal operations allowed on Graphs and Trees\n\n",
            currnode -> line_no
          );
          semantic_error = 1;
        }
        else if ( firsttype != D_VERTEX_TYPE && firsttype != D_EDGE_TYPE )
        {
          fprintf (
            stderr,
            "Error at line %d:\n\tOnly Vertices and Edges may be added and removed from Graphs/Trees\n\n",
            currnode -> line_no
          );
          semantic_error = 1;
        }

        result = secondtype;
      }
      else
      {
        fprintf ( stderr,
                  "Error at line %d:\n\tInvalid operands provided in expression\n\n",
                  currnode -> line_no );
        semantic_error = 1;
      }

      currnode -> result_type = result;
    }
    else
    {
      fprintf ( stderr, "Invalid tree produced! EXP node should not have more than 2 children\n" );
      semantic_error = 1;
    }
  }

  return 0;
}

/**
 * Checks for function definition and function call related errors
 */

int checkFunctionRelatedErrors ( ANODE *currnode, SYMBOLTABLE *symboltable )
{
  if ( currnode -> node_type == AST_PASSEDPARAMS_NODE )
  {
    // At a passed params node, we need to check that all the parameters are of the
    // same type as the called function

    // The first child of the parent will help us get the function identifier name from the STB
    unsigned int index = getFirstChild ( currnode -> parent ) -> extra_data . symboltable_index;

    FUNCTION *funcentry = & ( getEntryByIndex ( symboltable, index ) -> data . func_data );

    // Firstly, the number of parameters must match with the number of children of currnode
    if ( funcentry -> num_params != currnode -> num_of_children )
    {
      fprintf (
        stderr,
        "Error at line %d:\n\tNumber of passed parameters don't match with the function definition\n",
        currnode -> line_no
      );
      fprintf ( stderr,
                "\tNote: %s accepts %d parameters, but %d parameters are being passed\n\n",
                funcentry -> name,
                funcentry -> num_params,
                currnode -> num_of_children );
      return 1;
    }

    // If the number of parameters match, their types must also match
    LNODE funciterator, paramiterator;

    getIterator ( funcentry -> paramtypes, & funciterator );
    getIterator ( currnode -> children, & paramiterator );
    int paramnumber = 0;

    while ( hasNext ( & funciterator ) )
    {
      paramnumber ++;
      getNext ( funcentry -> parameters, & funciterator );
      getNext ( currnode -> children, & paramiterator );

      DATATYPE paramtype = funciterator . data . int_val;
      DATATYPE passedtype = ( * ( ANODE ** ) ( paramiterator . data . generic_val ) ) -> result_type;

      if ( paramtype != passedtype )
      {
        fprintf (
          stderr,
          "Error at line %d:\n\tThe type of the passed parameter to the function doesn't match the definition\n",
          currnode -> line_no
        );
        fprintf ( stderr,
                  "\tNote: Parameter %d of %s should be of type %s, given type is %s\n\n",
                  paramnumber, funcentry -> name,
                  getDataTypeName ( paramtype ),
                  getDataTypeName ( passedtype ) );
        return 1;
      }
    }
  }
  else if ( currnode -> node_type == AST_RETURNSTMT_NODE )
  {
    // The type being returned should match the type in the function definition
    // Get the function data in the symbol table by getting the identifier node
    // which is the first child of the grandparent of currnode

    // Scenario 2: The return type is nothing, in which case the data_type extra data is set
    // and RETURNSTMT node has no children

    unsigned int index = getFirstChild ( currnode -> parent -> parent ) -> extra_data . symboltable_index;

    if ( currnode -> num_of_children == 0 )
    {
      // The return type is nothing

      if ( getEntryByIndex ( symboltable, index ) -> data . func_data . ret_type != D_NOTHING_TYPE )
      {
        fprintf (
          stderr,
          "Error at line %d:\n\tType of value being returned does not match the return type \
          in the function definition\n",
          currnode -> line_no
        );
        fprintf ( stderr,
                  "\tNote: Expected return type of %s is %s, but code returns Nothing\n\n",
                  getEntryByIndex ( symboltable, index ) -> data . func_data . name,
                  getDataTypeName ( getEntryByIndex ( symboltable, index ) -> data . func_data . ret_type ) );
        return 1;
      }

      return 0;
    }

    if ( getFirstChild ( currnode ) -> result_type
         != getEntryByIndex ( symboltable, index ) -> data . func_data . ret_type )
    {
      fprintf (
        stderr,
        "Error at line %d:\n\tType of value being returned does not match the return type \
        in the function definition\n",
        currnode -> line_no
      );
      fprintf ( stderr,
                "\tNote: Expected return type of %s is %s, but code returns a/an %s\n\n",
                getEntryByIndex ( symboltable, index ) -> data . func_data . name,
                getDataTypeName ( getEntryByIndex ( symboltable, index ) -> data . func_data . ret_type ),
                getDataTypeName ( getFirstChild ( currnode ) -> result_type ) );
      return 1;
    }
  }
  else if ( currnode -> node_type == AST_FUNCTIONCALL_NODE )
  {
    unsigned int index = getFirstChild ( currnode ) -> extra_data . symboltable_index;

    if ( getEntryByIndex ( symboltable, index ) -> data . func_data . ret_type != D_NOTHING_TYPE )
    {
      fprintf ( stderr,
                "Warning at line %d:\n\tReturn value of function is not being used\n",
                currnode -> line_no );
      fprintf ( stderr,
                "\tNote: %s returns a/an %s\n\n",
                getEntryByIndex ( symboltable, index ) -> data . func_data . name,
                getDataTypeName ( getEntryByIndex ( symboltable, index ) -> data . func_data . ret_type ) );

      return 0;
    }
  }

  return 0;
}

/**
 * Checks for errors in the usage of loop statements
 * and Graph/Tree traversals
 */

int checkLoopErrors ( ANODE *currnode, SYMBOLTABLE *symboltable )
{
  if ( currnode -> node_type == AST_FOR_NODE )
  {
    // A for node could have 2, 4 or 6 children. We need to deal with each of the cases

    if ( currnode -> num_of_children == 2 )
    {
      ANODE *child = getFirstChild ( currnode );

      if ( child -> node_type == AST_LITERAL_NODE && child -> result_type != D_INT_TYPE )
      {
        fprintf ( stderr,
                  "Error at line %d:\n\tThe number of iterations of a for loop must be an Integer\n",
                  currnode -> line_no );
        fprintf ( stderr, "\tNote: Provided type is a %s\n\n", getDataTypeName ( child -> result_type ) );
        semantic_error = 1;
      }
      else if ( child -> node_type == AST_IDENTIFIER_NODE )
      {
        unsigned int index = child -> extra_data . symboltable_index;

        STBENTRY *entry = getEntryByIndex ( symboltable, index );
        char *name = NULL;
        if ( entry -> entry_type == ENTRY_FUNC_TYPE )
          name = entry -> data . func_data . name;
        else
          name = entry -> data . var_data . name;

        if ( entry -> entry_type == ENTRY_FUNC_TYPE || entry -> data . var_data .data_type != D_INT_TYPE )
        {
          fprintf (
            stderr,
            "Error at line %d:\n\tThe identifier in a for loop must be an integer variable\n",
            currnode -> line_no
          );
          fprintf ( stderr, "\tNote: %s is not an integer\n\n", name );
          return 1;
        }
      }
    }
    else if ( currnode -> num_of_children == 3 )
    {
      fprintf ( stderr, "Invalid tree produced! For node cannot have 3 children\n" );
      semantic_error = 1;
    }
    else if ( currnode -> num_of_children == 4 )
    {
      // The first child must be a Vertex or an Edge and the third child must be a Graph or Tree

      if ( getFirstChild ( currnode ) -> result_type != D_VERTEX_TYPE
           && getFirstChild ( currnode ) -> result_type != D_EDGE_TYPE )
      {
        fprintf ( stderr,
                  "Error at line %d:\n\tThe iteration variable must be a vertex or an edge\n",
                  currnode -> line_no );
        fprintf ( stderr,
                  "\tNote: Provided variable has type %s\n\n",
                  getDataTypeName ( getFirstChild ( currnode ) -> result_type ) );
        return 1;
      }

      if ( getThirdChild ( currnode ) -> result_type != D_GRAPH_TYPE
           && getThirdChild ( currnode ) -> result_type != D_TREE_TYPE )
      {
        fprintf ( stderr,
                  "Error at line %d:\n\tThe object to iterate over must be a Graph or Tree\n",
                  currnode -> line_no );
        fprintf ( stderr,
                  "\tNote: Provided type is %s\n\n",
                  getDataTypeName ( getThirdChild ( currnode ) -> result_type ) );
        return 1;
      }
    }
    else if ( currnode -> num_of_children == 5 )
    {
      fprintf ( stderr, "Invalid tree produced! For node cannot have 5 children\n" );
      semantic_error = 1;
    }
    else if ( currnode -> num_of_children == 6 )
    {
      // We can have two cases here. Either the construct can be:
      // For u in G adjacent to v:
      // or it can be:
      // For u in BFT ( G, v )
      // We use the third node (which might be a BDFT node) to differentiate b/w the two

      if ( getFirstChild ( currnode ) -> result_type != D_VERTEX_TYPE
           && getFirstChild ( currnode ) -> result_type != D_EDGE_TYPE )
      {
        fprintf ( stderr,
                  "Error at line %d:\n\tThe iteration variable must be a vertex or an edge\n",
                  currnode -> line_no );
        fprintf ( stderr,
                  "\tNote: Provided variable has type %s\n\n",
                  getDataTypeName ( getFirstChild ( currnode ) -> result_type ) );
        return 1;
      }

      if ( getThirdChild ( currnode ) -> node_type == AST_BDFT_NODE )
      {
        if ( getFourthChild ( currnode ) -> result_type != D_GRAPH_TYPE
             && getFourthChild ( currnode ) -> result_type != D_TREE_TYPE )
        {
          fprintf (
            stderr,
            "Error at line %d:\n\tThe first parameter to BFT or DFT must be a Graph or Tree\n",
            currnode -> line_no
          );
          fprintf ( stderr,
                    "\tNote: Provided type is %s\n\n",
                    getDataTypeName ( getFourthChild ( currnode ) -> result_type ) );
          return 1;
        }

        if ( getFifthChild ( currnode ) -> result_type != D_VERTEX_TYPE )
        {
          fprintf ( stderr,
                    "Error at line %d:\n\tThe second parameter to BFT/DFT must be a Vertex\n\n",
                    currnode -> line_no );
          return 1;
        }
      }
      else
      {
        // The third child is not BDFT, thus adjacent to must be there

        if ( getThirdChild ( currnode ) -> result_type != D_GRAPH_TYPE
             && getThirdChild ( currnode ) -> result_type != D_TREE_TYPE )
        {
          fprintf ( stderr,
                    "Error at line %d:\n\tThe object to iterate over must be a Graph or Tree\n",
                    currnode -> line_no );
          fprintf ( stderr,
                    "\tNote: Type provided is %s\n\n",
                    getDataTypeName ( getThirdChild ( currnode ) -> result_type ) );
          return 1;
        }

        if ( getFifthChild ( currnode ) -> result_type != D_VERTEX_TYPE )
        {
          fprintf ( stderr,
                    "Error at line %d:\n\tThe adjacent to parameter must be a Vertex\n",
                    currnode -> line_no );
          fprintf ( stderr,
                    "\tNote: Provided type is %s\n\n",
                    getDataTypeName ( getFifthChild ( currnode ) -> result_type ) );
          return 1;
        }
      }
    }
  }

  return 0;
}

/**
 * Checks for errors in the edge creation statement
 */

int checkEdgeCreation ( ANODE *currnode )
{
  if ( currnode -> node_type == AST_EDGECREATE_NODE )
  {
    if ( getFirstChild ( currnode ) -> result_type != D_VERTEX_TYPE
         || getSecondChild ( currnode ) -> result_type != D_VERTEX_TYPE )
    {
      fprintf ( stderr,
                "Error at line %d:\n\tThe components of an edge must be vertices\n",
                currnode -> line_no );
      fprintf ( stderr,
                "\tNote: Provided types are %s and %s\n\n",
                getDataTypeName ( getFirstChild ( currnode ) -> result_type ),
                getDataTypeName ( getSecondChild ( currnode ) -> result_type ) );
      return 1;
    }

    currnode -> result_type = D_EDGE_TYPE;
  }

  return 0;
}

/**
 * Main function that performs complete semantic analysis
 * on the provided AST node, depending on the type of node
 * and auxiliary data fed from the symbol table
 *
 * @param currnode ANODE* The AST node being analysed
 * @param symboltable SYMBOLTABLE* The symboltable to use for aux data
 * @param infunction int* A flag specifying if node is in a fn subtree
 * @param loopcount int* A counter to maintain loop depth
 * @param bdftcount int* A counter for BFT/DFT occurrences
 */

void performSemanticChecks (
    ANODE *currnode, SYMBOLTABLE *symboltable, int *infunction,
    int *loopcount, int *bdftcount )
{
  if ( checkInvalidKeywordLocation ( currnode, infunction,
                                     loopcount, bdftcount ) == 1 )
  {
    semantic_error = 1;
    return;
  }
  else if ( checkInvalidAssignment ( currnode, symboltable ) == 1 )
  {
    semantic_error = 1;
    return;
  }
  else if ( checkInvalidDereference ( currnode, symboltable ) == 1 )
  {
    semantic_error = 1;
    return;
  }
  else if ( checkIOErrors ( currnode ) == 1 )
  {
    semantic_error = 1;
    return;
  }
  else if ( checkExpressionErrors ( currnode ) == 1 )
  {
    semantic_error = 1;
    return;
  }
  else if ( checkFunctionRelatedErrors ( currnode, symboltable ) == 1 )
  {
    semantic_error = 1;
    return;
  }
  else if ( checkLoopErrors ( currnode, symboltable ) == 1 )
  {
    semantic_error = 1;
    return;
  }
  else if ( checkEdgeCreation ( currnode ) == 1 )
  {
    semantic_error = 1;
    return;
  }
  else if ( currnode -> node_type == AST_LITERAL_NODE )
  {
    // The type of a literal is the type of its entry in the symbol table
    currnode -> result_type = getEntryByIndex (
        symboltable, currnode -> extra_data . symboltable_index ) -> data . lit_data . lit_type;
  }
  else if ( currnode -> node_type == AST_DEPTH_NODE )
    currnode -> result_type = D_INT_TYPE;
  else if ( currnode -> node_type == AST_DEST_NODE || currnode -> node_type == AST_SOURCE_NODE
            || currnode -> node_type == AST_ROOT_NODE )
    currnode -> result_type = D_VERTEX_TYPE;
  else if ( currnode -> node_type == AST_WEIGHT_NODE )
    currnode -> result_type = D_FLOAT_TYPE;
  else
    processIdentifier ( currnode, symboltable );

  // End of semantic analysis for different nodes
}

