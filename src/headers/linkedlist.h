#define LINKEDLIST_DEFINED

#define TRUE 1
#define FALSE 0

typedef enum linkedlist_type
{
  LL_TYPE_FIRST,
  LL_INT_TYPE,
  LL_DOUBLE_TYPE,
  LL_STRING_TYPE,
  LL_GENERIC_TYPE,
  LL_TYPE_LAST
} LINKEDLIST_TYPE;

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
  LINKEDLIST_TYPE data_type;
  int generic_size;
  char *name;
} LINKEDLIST;

extern LINKEDLIST* getLinkedList ( LINKEDLIST_TYPE );

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

extern void deleteLinkedList ( LINKEDLIST * );

