typedef struct ast_node
{
  char *name;
  int num_of_children;
  int child_id;
  struct ast_node *parent;
  struct ast_node **next;
} ANODE;

typedef struct ast_struct
{
  ANODE *root;
  char *name;
} AST;

extern AST* getNewAst ();

extern AST* setAstName ( AST * , const char * );

extern ANODE* initializeAstNode ( ANODE * , ANODE * );

extern ANODE* setAstNodeName ( ANODE * , const char * );

extern ANODE* setNumChildren ( ANODE * , int );

extern ANODE* allocateChildren ( ANODE * );

extern ANODE* insertSpaceSeparatedWords ( ANODE * , char * );

extern ANODE* getLeftMostDesc ( ANODE * );

extern ANODE* getNextPreOrder ( ANODE * );

