#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
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

#define NUMREG 6

#define ONE_OFFSET 1
#define TWO_OFFSETS 2
#define THREE_OFFSETS 3
#define DATA_IN_REG 4
#define OFFSET_IN_REG 5
#define IS_GLOBAL 0
#define IS_LOCAL 1
#define IS_LITERAL 2
#define NO_SPECIFIC_REG -1
#define OFFSET_ANY -1
#define NO_REGISTER -2

#define EAX_REG 0
#define EBX_REG 1
#define ECX_REG 2
#define EDX_REG 3

typedef struct register_data
{
  int flushed;
  int hasoffset;
  int stbindex;
  int offset1;
  int offset2;
  int offset3;
  int istemp;
  int isglobal;
} REGISTER;

extern REGISTER registers [ NUMREG ];
extern int roundrobinreg;

extern char registerNames [][4];

typedef struct literal_data
{
  char *name;
} LITDATA;

extern char* getRegisterName ( int regid );

extern void setRegisterProperties (
    int regid, int flushed, int isglobal, int istemp, int hasoffset,
    int stbindex, int offset1, int offset2, int offset3 );

extern void flushRegister ( int topick, FILE *codefile, SYMBOLTABLE *symboltable );

extern int getRegister (
    FILE *codefile, SYMBOLTABLE *symboltable, int symboltable_index, int offset1,
    int offset2, int offset3, int topick, int istemp, int donttouch1, int donttouch2 );

extern int getOffsetInReg ( ANODE *assignable, FILE *codefile, SYMBOLTABLE *symboltable );

extern int getLiteralInRegister (
    ANODE *literalnode, FILE *codefile, SYMBOLTABLE *symboltable,
    TRIE *literaltrie, LITDATA *literals );

