#define TRUE 1
#define FALSE 0

typedef struct linkedlist_node
{
  struct linkedlist_node *prev, *next;
  char *value;
} LNODE;

typedef struct linkedlist
{
  LNODE *head, *tail;
  char *name;
} LINKEDLIST;

extern LINKEDLIST* getLinkedList ();

extern LINKEDLIST* insertAtBack ( LINKEDLIST * , char * );

extern LINKEDLIST* insertAtFront ( LINKEDLIST * , char * );

extern LINKEDLIST* insertSpaceSeparatedWords ( LINKEDLIST * , char * );

extern LINKEDLIST* deleteFromBack ( LINKEDLIST * );

extern LINKEDLIST* deleteFromFront ( LINKEDLIST * );

extern LNODE* copyNode ( LNODE * , LNODE * );

extern LNODE* getIterator ( LINKEDLIST * , LNODE * );

extern LNODE* getReverseIterator ( LINKEDLIST * , LNODE * );

extern int hasNext ( LNODE * );

extern int hasPrevious ( LNODE * );

extern LNODE* getNext ( LNODE * );

extern LNODE* getPrevious ( LNODE * );

extern LNODE* getFront ( LINKEDLIST * );

extern LNODE* getBack ( LINKEDLIST * );

