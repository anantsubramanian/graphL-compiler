typedef struct ast_node
{
  char *name;
  int num_of_children;
  struct ast_node **next;
} ANODE;

typedef struct ast_struct
{
  ANODE *root;
  char *name;
} AST;

extern AST* getNewAst ();

extern AST* setAstName ( AST * , const char * );

extern ANODE* initializeNode ( ANODE * );

extern ANODE* setNodeName ( ANODE * , const char * );

extern ANODE* setNumChildren ( ANODE * , int );

extern ANODE* allocateChildren ( ANODE * );

extern ANODE* insertSpaceSeparatedWords ( ANODE * , char * );

