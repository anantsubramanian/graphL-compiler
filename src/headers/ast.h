#define AST_INT_TYPE 1
#define AST_FLOAT_TYPE 2
#define AST_STRING_TYPE 3
#define AST_VERTEX_TYPE 4
#define AST_EDGE_TYPE 5
#define AST_GRAPH_TYPE 6
#define AST_TREE_TYPE 7

typedef enum ast_type
{
  AST_PROGRAM_NODE             = 1,
  AST_GLOBALDEFINES_NODE       = 2,
  AST_GLOBALDEFINE_NODE        = 3,
  AST_DATATYPE_NODE            = 4,
  AST_VARIABLE_NODE            = 5,
  AST_LET_NODE                 = 6,
  AST_LITERAL_NODE             = 7,
  AST_EDGELITERAL_NODE         = 8,
  AST_FUNCTIONDEFINES_NODE     = 9,
  AST_FUNCTION_NODE            = 10,
  AST_QUALIFIEDPARAMETERS_NODE = 11,
  AST_QUALIFIEDPARAMETER_NODE  = 12,
  AST_BLOCK_NODE               = 13,
  AST_RETURNSTMT_NODE          = 14,
  AST_DEFINE_NODE              = 15,
  AST_FOR_NODE                 = 16,
  AST_READ_NODE                = 17,
  AST_PRINT_NODE               = 18,
  AST_FUNCTIONCALL_NODE        = 19,
  AST_PARAMETER_NODE           = 20,
  AST_IF_NODE                  = 21,
  AST_ELSE_NODE                = 22,
  AST_ELSEIF_NODE              = 23,
  AST_BREAK_NODE               = 24,
  AST_OREXP_NODE               = 25,
  AST_ANDEXP_NODE              = 26,
  AST_NOTEXP_NODE              = 27,
  AST_LT_NODE                  = 28,
  AST_GT_NODE                  = 29,
  AST_LTE_NODE                 = 30,
  AST_GTE_NODE                 = 31,
  AST_EQ_NODE                  = 32,
  AST_PLUS_NODE                = 33,
  AST_MINUS_NODE               = 34,
  AST_MUL_NODE                 = 35,
  AST_DIV_NODE                 = 36,
  AST_MOD_NODE                 = 37,
  AST_DEPTH_NODE               = 38,
  AST_BDFT_NODE                = 39
} ASTNODETYPE;

typedef struct ast_node
{
  ASTNODETYPE node_type;
  char *name;
  int num_of_children;
  int child_id;
  struct ast_node *parent;
  struct ast_node **next;
  union
  {
    int data_type;          // For DATATYPE node
    int symboltable_index;  // For VARIABLE and LITERAL nodes
  } extra_data;
} ANODE;

typedef struct ast_struct
{
  ANODE *root;
  char *name;
} AST;

extern AST* getNewAst ();

extern AST* setAstName ( AST * , const char * );

extern ANODE* initializeAstNode ( ANODE * , ANODE * );

extern ANODE* setAstNodeName ( ANODE * , const char * );

extern ANODE* setNumChildren ( ANODE * , int );

extern ANODE* allocateChildren ( ANODE * );

extern ANODE* insertSpaceSeparatedWords ( ANODE * , char * );

extern ANODE* getLeftMostDesc ( ANODE * );

extern ANODE* getNextPreOrder ( ANODE * );

