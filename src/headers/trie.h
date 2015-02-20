#define TRUE 1
#define FALSE 0

typedef struct trie_node
{
  char *name;
  int value;
  int is_final;
  int count;
  struct trie_node *next [128];
} TNODE;

typedef struct trie_struct
{
  TNODE *root;
  char *name;
} TRIE;

extern TRIE* getNewTrie ();

extern TRIE* setName ( TRIE * , const char * );

extern TNODE* initializeNode ( TNODE * );

extern TNODE* insertString ( TRIE * , const char * );

extern TNODE* findString ( TRIE * , const char * );

extern TNODE* setValue ( TNODE * , int );

extern TNODE* setNodeName ( TNODE * , const char * );

extern TRIE* deleteString ( TRIE * , const char * );

extern int isFinal ( TNODE * );

