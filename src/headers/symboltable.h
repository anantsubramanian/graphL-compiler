#include "trie.h"
#include "stack.h"

#define TRUE 1
#define FALSE 0

// Begin data types

#define D_INT_TYPE 1
#define D_FLOAT_TYPE 2
#define D_STRING_TYPE 3
#define D_VERTEX_TYPE 4
#define D_EDGE_TYPE 5
#define D_TREE_TYPE 6
#define D_GRAPH_TYPE 7
#define D_NOTHING_TYPE 8

// Begin variable types

#define V_GLOBAL_TYPE 1
#define V_LOCAL_TYPE 2
#define V_PARAM_TYPE 3

// Begin stbnode types

#define N_VAR_TYPE 1
#define N_FUNC_TYPE 2
#define N_LIT_TYPE 3

// Begin type definitions

typedef struct variable_node
{
  char *name;
  int data_type;
  int var_type;
  int scope_level;
  int scope_sublevel;
  int decl_line;
  int refr_line;
  int value;
} VARIABLE;

typedef struct function_node
{
  char *name;
  int num_params;
  int *params;
  int ret_type;
  int decl_line;
  int refr_line;
} FUNCTION;

typedef struct literal_node
{
  int lit_type;
  union
  {
    int int_value;
    double double_val;
    char *string_val;
  } data;
} LITERAL;

typedef struct symbolt_node
{
  int node_type;
  union
  {
    VARIABLE var_data;
    FUNCTION func_data;
    LITERAL lit_data;
  } data;
} STBNODE;

typedef struct symbol_table
{
  char *name;
  TRIE *indexmap;
  int num_entries;
  int size;
  int cur_scope;
  int cur_subscope;
  STACK **entries;
  STACK *environments;
} SYMBOLTABLE;


// Begin function prototypes

extern SYMBOLTABLE* getSymbolTable ();

extern SYMBOLTABLE* setSymbolTableName ( SYMBOLTABLE * , char * );

extern SYMBOLTABLE* setNumEntries ( SYMBOLTABLE * , unsigned int );

extern SYMBOLTABLE* openEnv ( SYMBOLTABLE * );

extern SYMBOLTABLE* closeEnv ( SYMBOLTABLE * );

extern int addEntry ( SYMBOLTABLE * , char * , int );

extern int checkNameExistence ( SYMBOLTABLE * , char * );

extern int checkIndexExistence ( SYMBOLTABLE * , unsigned int );

extern STBNODE* getEntryByName ( SYMBOLTABLE * , char * );

extern STBNODE* getEntryByIndex ( SYMBOLTABLE * , unsigned int );

