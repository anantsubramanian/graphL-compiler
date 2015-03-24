// Authors: Anant Subramanian <anant.subramanian15@gmail.com>
//          Aditya Bansal <adityabansal_adi@yahoo.co.in>
//
// BITS PILANI ID NOs: 2012A7TS010P
//                     2012A7PS122P
//
// Project Team Num: 1
// Project Group No. 1

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

extern PTNODE* insertSpaceSeparatedWords ( PTNODE * , char * );

extern PTNODE* getLeftMostDesc ( PTNODE * );

extern PTNODE* getNextPreOrder ( PTNODE * );

