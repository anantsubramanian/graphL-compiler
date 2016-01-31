#define CONSTANTS_DEFINED

// Required language-specific tokens for semantic analysis
#define TK_BEGIN "TK_BEGIN"
#define TK_END "TK_END"
#define TK_IDEN "TK_IDEN"
#define TK_INTLIT "TK_INTLIT"
#define TK_STRINGLIT "TK_STRINGLIT"
#define TK_FLOATLIT "TK_FLOATLIT"
#define TK_INT "TK_INT"
#define TK_STRING "TK_STRING"
#define TK_FLOAT "TK_FLOAT"
#define TK_VERTEX "TK_VERTEX"
#define TK_EDGE "TK_EDGE"
#define TK_GRAPH "TK_GRAPH"
#define TK_TREE "TK_TREE"
#define TK_NOTHING "TK_NOTHING"
#define TK_AND "TK_AND"
#define TK_OR "TK_OR"
#define TK_NOT "TK_NOT"
#define TK_PLUS "TK_PLUS"
#define TK_MINUS "TK_MINUS"
#define TK_MUL "TK_MUL"
#define TK_DIV "TK_DIV"
#define TK_MODULO "TK_MODULO"
#define TK_GT "TK_GT"
#define TK_LT "TK_LT"
#define TK_GTE "TK_GTE"
#define TK_LTE "TK_LTE"
#define TK_EQ "TK_EQ"
#define TK_BFT "TK_BFT"
#define TK_DFT "TK_DFT"
#define TK_FUNCTION "TK_FUNCTION"
// End language-specific tokens

// A consistent mapping between these #defines and the AST_NODETYPES_FILE
// must be maintained.

#define AST_PROGRAM_NODE 0
#define AST_GLOBALDEFINES_NODE 1
#define AST_GLOBALDEFINE_NODE 2
#define AST_DEFINE_NODE 3
#define AST_LET_NODE 4
#define AST_ASSIGNABLE_NODE 5
#define AST_DATATYPE_NODE 6
#define AST_IDENTIFIER_NODE 7
#define AST_FUNCTION_NODE 8
#define AST_FUNCBODY_NODE 9
#define AST_QUALIFIEDPARAMETERS_NODE 10
#define AST_QUALIFIEDPARAMETER_NODE 11
#define AST_RETURNTYPE_NODE 12
#define AST_READ_NODE 13
#define AST_PRINT_NODE 14
#define AST_COMPARE_NODE 15
#define AST_BOOLEXP_NODE 16
#define AST_EXP_NODE 17
#define AST_PASSEDPARAMS_NODE 18
#define AST_RETURNSTMT_NODE 19
#define AST_FUNCTIONCALL_NODE 20
#define AST_IF_NODE 21
#define AST_BLOCK_NODE 22
#define AST_FOR_NODE 23
#define AST_BDFT_NODE 24
#define AST_EDGECREATE_NODE 25
#define AST_BOOLOP_NODE 26
#define AST_AROP_NODE 27
#define AST_FORIN_NODE 28
#define AST_ADJTO_NODE 29
#define AST_LITERAL_NODE 30
#define AST_DEPTH_NODE 31
#define AST_DEST_NODE 32
#define AST_SOURCE_NODE 33
#define AST_WEIGHT_NODE 34
#define AST_ASSIGNFUNC_NODE 35
#define AST_MEMBEROP_NODE 36
#define AST_ROOT_NODE 37
#define AST_ENDASSIGN_NODE 38
#define AST_BREAK_NODE 39

// End AST node types #defines

// Begin offsets
#define INTOFFSET 0
#define FLOATOFFSET 4
#define STRINGOFFSET 8
#define SOURCEOFFSET 12
#define DESTOFFSET 16
#define WEIGHTOFFSET 20
#define VLISTOFFSET 0
#define ELISTOFFSET 4
#define ROOTOFFSET 8
// End offsets

// Begin globally used enums
typedef enum data_types
{
  D_TYPE_FIRST = 0,
  D_INT_TYPE,
  D_FLOAT_TYPE,
  D_STRING_TYPE,
  D_VERTEX_TYPE,
  D_EDGE_TYPE,
  D_TREE_TYPE,
  D_GRAPH_TYPE,
  D_NOTHING_TYPE,
  D_TYPE_LAST
} DATATYPE;

typedef enum arop_types
{
  A_TYPE_FIRST = 0,
  A_PLUS_TYPE,
  A_MINUS_TYPE,
  A_MUL_TYPE,
  A_DIV_TYPE,
  A_MODULO_TYPE,
  A_TYPE_LAST
} AROPTYPE;

typedef enum compareop_type
{
  C_TYPE_FIRST = 0,
  C_LT_TYPE,
  C_GT_TYPE,
  C_LTE_TYPE,
  C_GTE_TYPE,
  C_EQ_TYPE,
  C_TYPE_LAST
} COMPOPTYPE;

typedef enum boolop_type
{
  B_TYPE_FIRST = 0,
  B_AND_TYPE,
  B_OR_TYPE,
  B_NOT_TYPE,
  B_TYPE_LAST
} BOOLOPTYPE;

typedef enum bdft_type
{
  BDFT_TYPE_FIRST = 0,
  BDFT_BFT_TYPE,
  BDFT_DFT_TYPE,
  BDFT_TYPE_LAST
} BDFTTYPE;
// End globally defined enums

// Global pipeline file names
#define PARSE_OUTPUT_FILE "PARSEOUTPUT"
#define ATTRIBUTES_FILE "TOKENMAP"
#define AST_OUTPUT_FILE "ASTOUTPUT"
#define STB_DUMP_FILE "STBDUMP"
#define AST_DUMP_FILE "ASTDUMP"
#define TOKENS_FILE "TOKENS"
#define ERRORS_FILE "ERRORS"
#define DICT_FILE "TOKENMAP"
#define PARSE_OUTPUT "PARSEOUTPUT"
#define PARSE_ERRORS "PARSEERRORS"
// End global file names

// Important symbol names
#define ROOTNODE_NAME "AST_ROOT_NODE"
#define START_SYMBOL "<program>"
#define NEWLINE_SYMBOL "TK_NEWLINE"
#define IDENTIFIER_SYMBOL "TK_IDEN"
#define STRINGLIT_SYMBOL "TK_STRINGLIT"
#define INTLIT_SYMBOL "TK_INTLIT"
#define FLOATLIT_SYMBOL "TK_FLOATLIT"
// End important symbols

#define LOWER16MASK 0b00000000000000001111111111111111
#define UPPER16MASK 0b11111111111111110000000000000000

#define AST_NODETYPES_FILE "config/ast_nodetypes"
#define AST_INSTRUCTIONS_FILE "config/ast_instructions"
#define T_INDEX_FILE "config/terminals_index"
#define NT_INDEX_FILE "config/nonterminals_index"
#define DFA_PATH "config/dfa_lexer_description"
#define PTABLE_FILE "config/parse_table"
#define RULES_FILE "config/rules_file"
#define T_NAMEMAP_FILE "config/terminals_namemap"
#define FIRST_SETS_FILE "config/first_sets"
#define GRAMMAR_FILE "config/grammar_rules"
#define PARSE_TABLE_FILE "config/parse_table"

// Constants use to print descriptive names
extern const char nodeTypes[][30];
extern const char dataTypes[][10];
extern const char aropTypes[][10];

