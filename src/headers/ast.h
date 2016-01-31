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
#ifndef CONSTANTS_DEFINED
  #include "constants.h"
#endif

#define AST_DEFINED

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
  int global_or_local;
  int offsetcount;
  int offsetreg;
  int offset1;
  int offset2;
  int offset3;
  int line_no;
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
    int iflabel;
    int forlabel;
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

extern ANODE* getFirstChild ( ANODE * );

extern ANODE* getSecondChild ( ANODE * );

extern ANODE* getThirdChild ( ANODE * );

extern ANODE* getFourthChild ( ANODE * );

extern ANODE* getFifthChild ( ANODE * );

extern ANODE* rotateLeft ( ANODE * );

extern DATATYPE getDataType ( ANODE * );

extern const char* getNodeTypeName ( int type );

extern void preOrderDumpAst ( ANODE *subtree_root, FILE * );

