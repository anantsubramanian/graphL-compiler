#include "linkedlist.h"

#define TRUE 1
#define FALSE 0
#define STACK_INT_TYPE 1
#define STACK_DOUBLE_TYPE 2
#define STACK_STRING_TYPE 3
#define STACK_GENERIC_TYPE 4

typedef struct stack_struct
{
  LINKEDLIST *stack;
  int data_type;
  int generic_size;
  int is_empty;
} STACK;

extern STACK* getStack ( int );

extern STACK* setStackGenericSize ( STACK * , unsigned int );

extern int isEmpty ( STACK * );

extern STACK* push ( STACK * , void * );

extern STACK* pop ( STACK * );

extern void* top ( STACK * );

extern STACK* insertFromLinkedList ( STACK * , LINKEDLIST * );

