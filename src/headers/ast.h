// Authors: Anant Subramanian <anant.subramanian15@gmail.com>
//          Aditya Bansal <adityabansal_adi@yahoo.co.in>
//
// BITS PILANI ID NOs: 2012A7TS010P
//                     2012A7PS122P
//
// Project Team Num: 1
// Project Group No. 1

#ifndef STACK_DEFINED
  #include "stack.h"
#endif
#ifndef TRIE_DEFINED
  #include "trie.h"
#endif

#ifndef DATATYPE_DEFINED

#define DATATYPE_DEFINED 1

typedef enum data_types
{
  D_TYPE_FIRST = 0,
  D_INT_TYPE,
  D_FLOAT_TYPE,
  D_STRING_TYPE,
  D_VERTEX_TYPE,
  D_EDGE_TYPE,
  D_TREE_TYPE,
  D_GRAPH_TYPE,
  D_NOTHING_TYPE,
  D_TYPE_LAST
} DATATYPE;

#endif

#define CREATE 16   // 10000b
#define GOTOCH 8    // 01000b
#define PARENT 4    // 00100b
#define CONDRD 2    // 00010b
#define READ 1      // 00001b

typedef enum arop_types
{
  A_TYPE_FIRST = 0,
  A_PLUS_TYPE,
  A_MINUS_TYPE,
  A_MUL_TYPE,
  A_DIV_TYPE,
  A_MODULO_TYPE,
  A_TYPE_LAST
} AROPTYPE;

typedef enum compareop_type
{
  C_TYPE_FIRST = 0,
  C_LT_TYPE,
  C_GT_TYPE,
  C_LTE_TYPE,
  C_GTE_TYPE,
  C_EQ_TYPE,
  C_TYPE_LAST
} COMPOPTYPE;

typedef enum boolop_type
{
  B_TYPE_FIRST = 0,
  B_AND_TYPE,
  B_OR_TYPE,
  B_NOT_TYPE,
  B_TYPE_LAST
} BOOLOPTYPE;

typedef enum bdft_type
{
  BDFT_TYPE_FIRST = 0,
  BDFT_BFT_TYPE,
  BDFT_DFT_TYPE,
  BDFT_TYPE_LAST
} BDFTTYPE;

typedef struct ast_node
{
  char *name;
  int node_type;
  int num_of_children;
  struct ast_node *parent;
  int offset;
  DATATYPE result_type;
  LINKEDLIST *children;
  union
  {
    DATATYPE data_type;          // For DATATYPE nodes and ENDASSIGNABLE nodes
    int symboltable_index;       // For VARIABLE and LITERAL nodes
    AROPTYPE arop_type;
    COMPOPTYPE compop_type;
    BOOLOPTYPE boolop_type;
    BDFTTYPE bdft_type;
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

extern ANODE* dumpNode ( ANODE * , FILE * );

extern ANODE* readDumpNode ( ANODE * , FILE * );

extern int createProperty ( char * );

