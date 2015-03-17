#include "trie.h"
#include "stack.h"

#define TRUE 1
#define FALSE 0

typedef enum data_types
{
  D_INT_TYPE,
  D_FLOAT_TYPE,
  D_STRING_TYPE,
  D_VERTEX_TYPE,
  D_EDGE_TYPE,
  D_TREE_TYPE,
  D_GRAPH_TYPE,
  D_NOTHING_TYPE
} DATATYPE;

typedef enum variable_types
{
  V_GLOBAL_TYPE,
  V_LOCAL_TYPE,
  V_PARAM_TYPE
} VARIABLETYPE;

typedef enum st_entry_type
{
  ENTRY_VAR_TYPE,
  ENTRY_FUNC_TYPE,
  ENTRY_LIT_TYPE
} STB_ENTRYTYPE;

// Begin type definitions

typedef struct variable_entry
{
  char *name;
  DATATYPE data_type;
  VARIABLETYPE var_type;
  int scope_level;
  int scope_sublevel;
  int decl_line;
  int refr_line;
  int value;
  void *complexdata;
} VARIABLE;

typedef struct function_entry
{
  char *name;
  int num_params;
  int *params;
  DATATYPE ret_type;
  int decl_line;
  int refr_line;
} FUNCTION;

typedef struct literal_entry
{
  DATATYPE lit_type;
  union
  {
    int int_value;
    double double_val;
    char *string_val;
  } data;
} LITERAL;

typedef struct symboltable_entry
{
  STB_ENTRYTYPE entry_type;
  union
  {
    VARIABLE var_data;
    FUNCTION func_data;
    LITERAL lit_data;
  } data;
} STBENTRY;

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

typedef struct vertex_entry
{
  int int_val;
  double float_val;
  char *string_val;
  LINKEDLIST *adjacent;
} VERTEX;

typedef struct edge_entry
{
  int int_val;
  double float_val;
  char *string_val;
  double weight;
  VARIABLE *source;
  VARIABLE *dest;
} EDGE;

typedef struct tree_entry
{
  VARIABLE *root;
  LINKEDLIST *vertexlist;
  LINKEDLIST *edgelist;
} TREE;

typedef struct graph_entry
{
  LINKEDLIST *vertexlist;
  LINKEDLIST *edgelist;
} GRAPH;

// Begin function prototypes

extern SYMBOLTABLE* getSymbolTable ();

extern SYMBOLTABLE* setSymbolTableName ( SYMBOLTABLE * , char * );

extern SYMBOLTABLE* setNumEntries ( SYMBOLTABLE * , unsigned int );

extern SYMBOLTABLE* openEnv ( SYMBOLTABLE * );

extern SYMBOLTABLE* closeEnv ( SYMBOLTABLE * );

extern int addEntry ( SYMBOLTABLE * , char * , STB_ENTRYTYPE );

extern int checkNameExistence ( SYMBOLTABLE * , char * );

extern int checkIndexExistence ( SYMBOLTABLE * , unsigned int );

extern STBENTRY* getEntryByName ( SYMBOLTABLE * , char * );

extern STBENTRY* getEntryByIndex ( SYMBOLTABLE * , unsigned int );
