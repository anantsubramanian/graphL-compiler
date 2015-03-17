typedef enum ast_datatype
{
  AST_DATATYPE_FIRST,
  AST_INT_TYPE,
  AST_FLOAT_TYPE,
  AST_STRING_TYPE,
  AST_VERTEX_TYPE,
  AST_EDGE_TYPE,
  AST_GRAPH_TYPE,
  AST_TREE_TYPE,
  AST_DATATYPE_LAST
} AST_DATATYPE;

typedef enum ast_type
{
  AST_NODETYPE_FIRST,
  AST_PROGRAM_NODE,
  AST_MAINBODY_NODE,
  AST_GLOBALDEFINES_NODE,
  AST_GLOBALDEFINE_NODE,
  AST_DATATYPE_NODE,
  AST_VARIABLE_NODE,
  AST_LET_NODE,
  AST_LITERAL_NODE,
  AST_EDGELITERAL_NODE,
  AST_FUNCTIONDEFINES_NODE,
  AST_FUNCTION_NODE,
  AST_QUALIFIEDPARAMETERS_NODE,
  AST_QUALIFIEDPARAMETER_NODE,
  AST_BLOCK_NODE,
  AST_RETURNSTMT_NODE,
  AST_DEFINE_NODE,
  AST_FOR_NODE,
  AST_READ_NODE,
  AST_PRINT_NODE,
  AST_FUNCTIONCALL_NODE,
  AST_PARAMETER_NODE,
  AST_IF_NODE,
  AST_ELSE_NODE,
  AST_ELSEIF_NODE,
  AST_BREAK_NODE,
  AST_OREXP_NODE,
  AST_ANDEXP_NODE,
  AST_NOTEXP_NODE,
  AST_LT_NODE,
  AST_GT_NODE,
  AST_LTE_NODE,
  AST_GTE_NODE,
  AST_EQ_NODE,
  AST_PLUS_NODE,
  AST_MINUS_NODE,
  AST_MUL_NODE,
  AST_DIV_NODE,
  AST_MOD_NODE,
  AST_DEPTH_NODE,
  AST_BDFT_NODE,
  AST_NODETYPE_LAST
} AST_NODETYPE;

typedef struct ast_node
{
  AST_NODETYPE node_type;
  char *name;
  int num_of_children;
  int child_id;
  struct ast_node *parent;
  struct ast_node **next;
  union
  {
    AST_DATATYPE data_type;     // For DATATYPE node
    int symboltable_index;      // For VARIABLE and LITERAL nodes
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

