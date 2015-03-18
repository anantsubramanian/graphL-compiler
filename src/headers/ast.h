#include "stack.h"
#include "trie.h"

#define CREATE 8    // 1000b
#define GOTOCH 4    // 0100b
#define PARENT 2    // 0010b
#define CONDRD 6    // 0110b
#define READ 1      // 0001b

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

