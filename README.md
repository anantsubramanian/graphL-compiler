# graphL-compiler
A compiler for the GraphL Language

A brief documentation of language constructs and features:

1. The language is statically typed, and type binding is done using the Define statement: 

        "Define *datatype* *identifier*"

    1. Statements are terminated by new line characters, i.e. each line is a statement.
    2. A valid identifier starts with a character or an underscore, followed by any combination of characters, underscores, and numbers. The maximum length for an identifier is 200 characters ( this can be modified by changing the #defines in the driver files ).
    3. The valid data types in the language ( as of now ) are the *Primitive types* -- Int, Float and String
      and the *Complex types* -- Vertex, Edge, Graph and Tree.
    4.  Valid arithemetic operations on *Int* and *Float* are addition, subtraction, multiplication, division. Int additionally supports the remainder operation ( or modulo ). No operations have been defined for *String* as of now.
    5.  Every *Vertex* and *Edge* object has three members, and Int, a Float and a String. An *Edge* object additionally has the members source (of type *Vertex*), dest (of type *Vertex*) and Weight (of type *Float*) as its members.
    6.  Members of an object are accessed using the "'s" membership operator:
          Define Vertex v
          Let v's Int be 5
    7. A *Tree* and *Graph* objects have a list of *Vertex* and *Edge* as their members. These cannot be explicitly accessed, but can be iterated over and manipulated using operations. A *Tree* type object has an additional root (of type *Vertex*) member that can be optionally set for rooted trees.
    8. *Tree* and *Graph* objects support the addition and removal of vertices and edges using the overloaded '+' and '-' operators.

2. Control constructs:

    The basic branching constructs is the If [ Else If ] [ Else ] constructs. The syntax is as follows:

        If <boolexp> [ and/or [ not ] *boolexp* ... ] :
        Begin
          <statements>
        End
        Else If <boolexp> :
        Begin
          <statements>
        End
        Else :
        Begin
            <statements>
        End
        

    Where *Begin* and *End* are used to mark the beginning and end of a block. Optional precedence for Boolean expressions can be specified using curly parenthesis, differentiating it from the normal parenthesis for arithemetic precedence, as follows:

        If <boolexp> and { <boolexp> or <boolexp> } :
  
3. Iteration constructs:

    The following are valid loop constructs:

      1. For n iterations:
      2. For vertex in graph:
      3. For edge in graph:
      4. For vertex in BFT (graph, vertex):
      5. For vertex in DFT (graph, vertex):
    
    Where vertex, graph and edge are variables of type *Vertex*, *Graph* and *Edge* respectively.

4. Assignment Statements:

    An assignment statement has the following syntax (refer to grammar for details):

        Let <assignable> be <equatable>
  
5. I/O Operations:

    To read from standard input, the syntax is:

        Read ( <assignable> )

    To write to standard output, the syntax is:

        Print ( <printable> )

6. Functions:

    Functions are defined as follows (the function name is a regular identifier):

        Function <identifier> ( <datatype> <identifier>, ... , <datatype> <identifier> ) -> <returntype> :
        Begin
          <statements>
          return <returnable>
        End

    1. All functions must be defined before the main program statements.
    2. A function that doesn't return any data must specify the return type “Nothing”. The last statement of a function body must be the return statement.
    3. Nested function declarations are not allowed. 
    4. Functions have their own scope (block-level scoping). 
    5. All complex data types are passed by reference while primitive types are passed by value (in order to support recursion).
    6. The syntax for a function call is:

            <identifier> ( <parameter>, ... , <parameter> )
  
    7. Recursive function calls are supported by the language.

7. Literals:

    Three types of literals can be used in the language:
      1. Int literals 	– 	[0-9]+
      2. Float literals 	– 	[0-9]+\.[0-9]+
      3. String literals	– 	“ * ”
