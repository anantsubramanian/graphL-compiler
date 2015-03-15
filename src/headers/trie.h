#define TRUE 1
#define FALSE 0
#define TR_INT_TYPE 1
#define TR_DOUBLE_TYPE 2
#define TR_STRING_TYPE 3
#define TR_GENERIC_TYPE 4

typedef struct trie_node
{
  struct trie_node *next [128];
  int is_final;
  int count;
  char *name;
  union
  {
    int int_val;
    double double_val;
    char *string_val;
    void *generic_val;
  } data;
} TNODE;

typedef struct trie_struct
{
  TNODE *root;
  int data_type;
  int generic_size;
  char *name;
} TRIE;

extern TRIE* getNewTrie ( int );

extern TRIE* setGenericSize ( TRIE * , unsigned int );

extern TRIE* setTrieName ( TRIE * , const char * );

extern TNODE* initializeNode ( TNODE * );

extern TNODE* insertString ( TRIE * , const char * );

extern TNODE* findString ( TRIE * , const char * );

extern TNODE* setValue ( TRIE * , TNODE * , void * );

extern TNODE* setNodeName ( TNODE * , const char * );

extern TRIE* deleteString ( TRIE * , const char * );

extern int isFinalTNode ( TNODE * );

