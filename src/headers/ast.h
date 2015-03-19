#ifndef STACK_DEFINED
  #include "stack.h"
#endif
#ifndef TRIE_DEFINED
  #include "trie.h"
#endif

#define CREATE 16   // 10000b
#define GOTOCH 8    // 01000b
#define PARENT 4    // 00100b
#define CONDRD 2    // 00010b
#define READ 1      // 00001b

typedef struct ast_node
{
  char *name;
  int node_type;
  int num_of_children;
  struct ast_node *parent;
  LINKEDLIST *children;
  union
  {
    int data_type;              // For DATATYPE nodes
    int symboltable_index;      // For VARIABLE and LITERAL nodes
  } extra_data;
} ANODE;

typedef struct ast_struct
{
  ANODE *root;
  int num_types;
  TRIE *node_typemap;
  char *name;
} AST;

extern AST* getNewAst ();

extern AST* setAstName ( AST * , const char * );

extern AST* addTypeMap ( AST * , const char * , int );

extern int getMapValue ( AST * , const char * );

extern ANODE* initializeAstNode ( ANODE * toInitialize , ANODE * parent );

extern ANODE* setNodeType ( ANODE * , int );

extern ANODE* setAstNodeName ( ANODE * , const char * );

extern ANODE* getParent ( ANODE * );

extern ANODE* addChild ( ANODE * , int nodeType, int action );

extern int createProperty ( char * );

