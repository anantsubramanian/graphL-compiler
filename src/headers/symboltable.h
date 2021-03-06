#ifndef TRIE_DEFINED
  #include "trie.h"
#endif
#ifndef STACK_DEFINED
  #include "stack.h"
#endif
#ifndef CONSTANTS_DEFINED
  #include "constants.h"
#endif

#define SYMBOLTABLE_DEFINED

#define TRUE 1
#define FALSE 0

typedef enum variable_types
{
  V_TYPE_FIRST = 0,
  V_GLOBAL_TYPE,
  V_LOCAL_TYPE,
  V_PARAM_TYPE,
  V_TYPE_LAST
} VARIABLETYPE;

typedef enum st_entry_type
{
  ENTRY_TYPE_FIRST = 0,
  ENTRY_VAR_TYPE,
  ENTRY_FUNC_TYPE,
  ENTRY_LIT_TYPE,
  ENTRY_TYPE_LAST
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
  union
  {
    int int_value;
    char *string_value;
    double float_value;
    void *complex_value;
  } data;
  void *complexdata;
  LINKEDLIST *refr_lines;
} VARIABLE;

typedef struct function_entry
{
  char *name;
  int num_params;
  LINKEDLIST *parameters;
  LINKEDLIST *paramtypes;
  DATATYPE ret_type;
  int returndata_stbindex;
  int decl_line;
  LINKEDLIST *refr_lines;
} FUNCTION;

typedef struct literal_entry
{
  DATATYPE lit_type;
  char *value;
} LITERAL;

typedef struct symboltable_entry
{
  STB_ENTRYTYPE entry_type;
  int index;
  int offset;
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
  unsigned int num_entries;
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

extern SYMBOLTABLE* dumpEntry ( SYMBOLTABLE * , FILE * , unsigned int , char );

extern unsigned int readDumpEntry ( SYMBOLTABLE * , FILE * , char );

extern int addEntry ( SYMBOLTABLE * , char * , STB_ENTRYTYPE );

extern int checkNameExistence ( SYMBOLTABLE * , char * );

extern int checkIndexExistence ( SYMBOLTABLE * , unsigned int );

extern STBENTRY* getEntryByName ( SYMBOLTABLE * , char * );

extern STBENTRY* getEntryByIndex ( SYMBOLTABLE * , unsigned int );

extern const char* getDataTypeName ( DATATYPE );

extern const char* getAropName ( AROPTYPE );

