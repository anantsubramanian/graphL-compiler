#include "linkedlist.h"

#define TRUE 1
#define FALSE 0

typedef enum stack_type
{
  STACK_TYPE_FIRST,
  STACK_INT_TYPE,
  STACK_DOUBLE_TYPE,
  STACK_STRING_TYPE,
  STACK_GENERIC_TYPE,
  STACK_TYPE_LAST
} STACK_TYPE;

typedef struct stack_struct
{
  LINKEDLIST *stack;
  STACK_TYPE data_type;
  int generic_size;
  int is_empty;
} STACK;

extern STACK* getStack ( STACK_TYPE );

extern STACK* setStackGenericSize ( STACK * , unsigned int );

extern int isEmpty ( STACK * );

extern STACK* push ( STACK * , void * );

extern STACK* pop ( STACK * );

extern void* top ( STACK * );

extern STACK* insertFromLinkedList ( STACK * , LINKEDLIST * );

extern STACK* pushReverseSpaceSeparatedWords ( STACK * , const char * );

