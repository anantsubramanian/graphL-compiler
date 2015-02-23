#include "linkedlist.h"

#define TRUE 1
#define FALSE 0

typedef struct stack_struct
{
  LINKEDLIST *stack;
  int is_empty;
} STACK;

extern STACK* getStack ();

extern int isEmpty ( STACK * );

extern STACK* push ( STACK * , char * );

extern STACK* pop ( STACK * );

extern char* top ( STACK * );

