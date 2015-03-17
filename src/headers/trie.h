#define TRUE 1
#define FALSE 0

typedef enum trie_type
{
  TRIE_TYPE_FIRST,
  TRIE_INT_TYPE,
  TRIE_DOUBLE_TYPE,
  TRIE_STRING_TYPE,
  TRIE_GENERIC_TYPE,
  TRIE_TYPE_LAST
} TRIE_TYPE;

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
  TRIE_TYPE data_type;
  int generic_size;
  char *name;
} TRIE;

extern TRIE* getNewTrie ( TRIE_TYPE );

extern TRIE* setTrieGenericSize ( TRIE * , unsigned int );

extern TRIE* setTrieName ( TRIE * , const char * );

extern TNODE* initializeNode ( TNODE * );

extern TNODE* insertString ( TRIE * , const char * );

extern TNODE* findString ( TRIE * , const char * );

extern TNODE* setValue ( TRIE * , TNODE * , void * );

extern TNODE* setNodeName ( TNODE * , const char * );

extern TRIE* deleteString ( TRIE * , const char * );

extern int isFinalTNode ( TNODE * );

