#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#ifndef AST_DEFINED
  #include "ast.h"
#endif

#ifndef SYMBOLTABLE_DEFINED
  #include "symboltable.h"
#endif

#ifndef TRIE_DEFINED
  #include "trie.h"
#endif

#ifndef CONSTANTS_DEFINED
  #include "constants.h"
#endif

#ifndef PARSE_UTILS_DEFINED
  #include "parse_utils.h"
#endif

#ifndef DEBUG_FLAGS_DEFINED
  #include "debug.h"
#endif

#define INSTRLEN 5
#define DIGSTART 48
#define DIGEND 57
#define NEWLINE '\n'
#define COMMENT_START '#'
#define PROPERTIES_START '@'

#define PROPERTY_PARENT 1
#define PROPERTY_READ 2
#define PROPERTY_ADD 4
#define PROPERTY_CREATE 8

typedef struct property_data
{
  int jumps;
  int instruction;
  int node_type;
} PROPERTY;

typedef enum coditionalval_type
{
  CONDITIONAL_TYPE_FIRST = 0,
  CONDITIONAL_TERMINAL,
  CONDITIONAL_NONTERMINAL,
  CONDITIONAL_NODETYPE,
  CONDITIONAL_TYPE_LAST
} CONDTYPE;

typedef struct instruction_auxdata
{
  int conditional_value;
  CONDTYPE conditional_type;
} AUXDATA;

extern int makeTrieProperty ( char *instr );

extern int extractLineNum ( char *input );

extern void extractTokenData ( char *inputtoken, char **token, char **name, int *linenumber );

extern void getNodeInstructions (
    FILE *instructionsfile, int blocksize, TRIE *instructions,
    TRIE *auxdata, TRIE *nodetypemap, TRIE *nonterminals,
    TRIE *terminals, TRIE *properties );

extern void getNodeIntegers (
    int *beginint, int *endint, int *idenint, int *intlitint,
    int *stringlitint, int *floatlitint, int *intint,
    int *stringint, int *floatint, int *vertexint, int *edgeint,
    int *graphint, int *treeint, int *nothingint, int *andint,
    int *orint, int *notint, int *plusint, int *minusint,
    int *mulint, int *divint, int *moduloint, int *gtint,
    int *ltint, int *gteint, int *lteint, int *eqint,
    int *bftint, int *dftint, int *functionint, TRIE *terminals );

extern void handleAuxiliaryTerminalOperations (
    int beginint, int endint, int idenint, int intlitint,
    int stringlitint, int floatlitint, int intint,
    int stringint, int floatint, int vertexint, int edgeint,
    int graphint, int treeint, int nothingint, int andint,
    int orint, int notint, int plusint, int minusint,
    int mulint, int divint, int moduloint, int gtint,
    int ltint, int gteint, int lteint, int eqint,
    int bftint, int dftint, int functionint, int terminalvalue,
    ANODE *currnode, SYMBOLTABLE *symboltable, int *should_start_function,
    int *function_scope_started, char *tokenname, int linenumber,
    FILE *stbdumpfile );

extern void handleNodeProperty (
    ANODE **currnode, const char *topvalue, TNODE *jumps,
    STACK **stack, AST *ast, int *shouldAdd, int *shouldRead );

extern int handleNodeInstruction (
    FILE *astoutput, ANODE **currnode, TNODE *currentval, AST *ast,
    const char *topvalue, TRIE *auxdata, int *conditional_read,
    int *conditional_pop, int *conditional_value, CONDTYPE *conditional_type );

extern int constructionErrorOccurred ();

