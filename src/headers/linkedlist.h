#define TRUE 1
#define FALSE 0
#define LL_INT_TYPE 1
#define LL_DOUBLE_TYPE 2
#define LL_STRING_TYPE 3
#define LL_GENERIC_TYPE 4

typedef struct linkedlist_node
{
  struct linkedlist_node *prev, *next;
  union
  {
    int int_val;
    double double_val;
    char *string_val;
    void *generic_val;
  } data;
} LNODE;

typedef struct linkedlist
{
  LNODE *head, *tail;
  int data_type;
  int generic_size;
  char *name;
} LINKEDLIST;

extern LINKEDLIST* getLinkedList ( int );

extern LINKEDLIST* setGenericSize ( LINKEDLIST * , unsigned int );

extern LINKEDLIST* insertAtBack ( LINKEDLIST * , void * );

extern LINKEDLIST* insertAtFront ( LINKEDLIST * , void * );

extern LINKEDLIST* insertSpaceSeparatedWords ( LINKEDLIST * , char * );

extern LINKEDLIST* deleteFromBack ( LINKEDLIST * );

extern LINKEDLIST* deleteFromFront ( LINKEDLIST * );

extern LNODE* copyNode ( LINKEDLIST * , LNODE * , LNODE * );

extern LNODE* getIterator ( LINKEDLIST * , LNODE * );

extern LNODE* getReverseIterator ( LINKEDLIST * , LNODE * );

extern int hasNext ( LNODE * );

extern int hasPrevious ( LNODE * );

extern LNODE* getNext ( LINKEDLIST * , LNODE * );

extern LNODE* getPrevious ( LINKEDLIST * , LNODE * );

extern LNODE* getFront ( LINKEDLIST * );

extern LNODE* getBack ( LINKEDLIST * );

