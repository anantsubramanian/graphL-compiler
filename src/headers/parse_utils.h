#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#ifndef TRIE_DEFINED
  #include "trie.h"
#endif

#ifndef LINKEDLIST_DEFINED
  #include "linkedlist.h"
#endif

#ifndef CONSTANTS_DEFINED
  #include "constants.h"
#endif

#define PARSE_UTILS_DEFINED

#define BUFFERLEN 200
#define MAXLINE 400
#define NEWLINE '\n'

extern int getLineCount ( FILE *inputfile, int blocksize );

extern void populateTrie ( FILE *mapfile, int blocksize, TRIE* trie, int *count );

extern char* getLine ( FILE *inputfile, int blocksize, int linenumber );

