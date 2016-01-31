typedef struct parsetree_node
{
  char *name;
  int num_of_children;
  int child_id;
  struct parsetree_node *parent;
  struct parsetree_node **next;
} PTNODE;

typedef struct parsetree_struct
{
  PTNODE *root;
  char *name;
} PARSETREE;

extern PARSETREE* getNewParseTree ();

extern PARSETREE* setParseTreeName ( PARSETREE * , const char * );

extern PTNODE* initializeParseTreeNode ( PTNODE * , PTNODE * );

extern PTNODE* setParseTreeNodeName ( PTNODE * , const char * );

extern PTNODE* setNumChildren ( PTNODE * , int );

extern void deleteParseTreeNode ( PTNODE * );

extern PTNODE* allocateChildren ( PTNODE * );

extern PTNODE* insertSpaceSeparatedWordsInPTree ( PTNODE * , char * );

extern PTNODE* getLeftMostDesc ( PTNODE * );

extern PTNODE* getNextPreOrder ( PTNODE * );

