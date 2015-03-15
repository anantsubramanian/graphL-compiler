#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "headers/trie.h"
#include "headers/linkedlist.h"

#define BUFFERLEN 200
#define NEWLINE '\n'
#define MAXRULE 160
#define FIRST_SETS_FILE "config/first_sets"
#define GRAMMAR_FILE "config/grammar_rules"
#define PARSE_TABLE_FILE "config/parse_table"
#define NT_INDEX_FILE "config/nonterminals_index"
#define T_INDEX_FILE "config/terminals_index"
#define RULES_FILE "config/rules_file"
#define RULE_TRIE_NAME "Grammar Rules"
#define NT_TRIE_NAME "Non Terminals"
#define T_TRIE_NAME "Terminals"
#define TNT_TRIE_NAME "Terminals and Non Terminals"

int getLineCount ( FILE *inputfile, int blocksize )
{
  char c;

  int curbuff = -1;
  int charindx = -1;
  int lines = 0;
  int charsread = 0;
  char buffers [2] [blocksize];

  while ( TRUE )
  {
    // Get char from appropriate buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, inputfile ) ) == 0 )
        break;
    }
    c = buffers [ curbuff ] [ charindx ];

    if ( charsread < blocksize && charindx >= charsread )
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == NEWLINE )
      lines++;
  }

  return lines;
}

void populateTries ( FILE *grammarfile, int blocksize, LINKEDLIST* ruleLists [], TRIE* gramrules,
                     TRIE* nonterm, TRIE* terminals, TRIE* mixedbag, int *tokencounts )
{
  char c;

  char buffers [2] [blocksize];
  char token [ MAXRULE ];
  char *nttoken = NULL, *ttoken = NULL;
  char *rhs = NULL, *finalterms = NULL;

  FILE *rulesfile = NULL;
  rulesfile = fopen ( RULES_FILE, "w+" );

  if ( rulesfile == NULL )
  {
    fprintf ( stderr, "Failed to open rules output file\n" );
    exit (-1);
  }

  int curbuff = -1;
  int charindx = -1;
  int charsread = 0;

  int ruleno = -1;
  int tokencounter = 0;
  int mixcounter = 0;
  int marker = 0;
  int terminalcount = 0;
  int nonterminalcount = 0;

  TNODE *temp = NULL;

  FILE *tmapfile = NULL, *ntmapfile = NULL;
  tmapfile = fopen ( T_INDEX_FILE, "w+" );
  ntmapfile = fopen ( NT_INDEX_FILE, "w+" );

  if ( tmapfile == NULL )
  {
    fprintf ( stderr, "Failed to open terminals index file\n" );
    exit (-1);
  }

  if ( ntmapfile == NULL )
  {
    fprintf ( stderr, "Failed to open non-terminals index file\n" );
    exit (-1);
  }

  while ( TRUE )
  {
    // Read next character from the buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, grammarfile ) ) == 0 )
        break;
    }
    c = buffers [ curbuff ] [ charindx ];

    if ( charsread < blocksize && charindx >= charsread )
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == '-' )
      marker = tokencounter;

    if ( c == NEWLINE )
      token [ tokencounter ] = '\0';
    else
      token [ tokencounter ] = c;

    tokencounter++;

    // tokencounter == 1 indicates a blank line (only NULL character)
    if ( c == NEWLINE && tokencounter != 1 )
    {
      // Pushing a Rule to the Trie
      ruleno++;
      temp = insertString ( gramrules , token );
      temp = setValue ( gramrules , temp , &(ruleno) );
      tokencounter = 0;

      // Inserting into Non-Terminals Trie
      rhs = token;
      nttoken = strtok(rhs , "-");
      nttoken [ marker - 1 ] = '\0';

      temp = findString ( nonterm , nttoken );
      if ( temp == NULL )
      {
        temp = insertString ( nonterm , nttoken );

        // Write non terminal to index file
        fprintf ( ntmapfile, "%d %s\n", nonterminalcount, nttoken );

        temp = setValue ( nonterm , temp , &(nonterminalcount) );
        nonterminalcount++;
      }

      // Inserting Non-Terminals into the MixedBag Trie
      temp = findString ( mixedbag , nttoken );
      if ( temp == NULL )
      {
        temp = insertString ( mixedbag , nttoken );
        temp = setValue ( mixedbag , temp , &(mixcounter) );
        mixcounter++;
      }

      nttoken = strtok ( NULL, "-" );

       // Inserting into Array of Linked List
      ruleLists [ruleno] = getLinkedList ( LL_STRING_TYPE );
      ruleLists [ruleno] = insertSpaceSeparatedWords ( ruleLists[ruleno] , nttoken + 2 );

      fprintf ( rulesfile, "%d %s\n", ruleno, nttoken + 2 );

      // Inserting into Terminals Trie and MixedBag Trie
      finalterms = strdup ( nttoken + 1 );
      if ( finalterms == NULL )
      {
        fprintf ( stderr, "Failed to duplicate non terminal tokens\n" );
        exit (-1);
      }

      ttoken = strtok ( finalterms , " " );

      while ( ttoken != NULL )
      {
        if ( ttoken [0] == 'T' )
        {
          temp = findString ( terminals , ttoken );
          if ( temp == NULL )
          {
            temp = insertString ( terminals , ttoken );

            // Write terminal to index file
            fprintf ( tmapfile, "%d %s\n", terminalcount, ttoken );

            temp = setValue ( terminals , temp , &(terminalcount) );
            terminalcount++;
          }
          temp = findString ( mixedbag , ttoken );
          if ( temp == NULL )
          {
            temp = insertString ( mixedbag , ttoken );
            temp = setValue ( mixedbag , temp , &(mixcounter) );
            mixcounter++;
          }
        }

        ttoken = strtok ( NULL , " " );
      }

      free ( finalterms );
      finalterms = NULL;
    }
  }

  temp = insertString ( terminals , "e" );

  // Write non terminal to index file
  fprintf ( tmapfile, "%d e\n", terminalcount );

  temp = setValue ( terminals , temp , &(terminalcount) );
  terminalcount++;

  // Assign values to be returned
  tokencounts [0] = terminalcount;
  tokencounts [1] = nonterminalcount;
  tokencounts [2] = mixcounter;

  if ( fclose ( tmapfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close terminals index file\n" );
    exit (-1);
  }

  if ( fclose ( ntmapfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close non-terminals index file\n" );
    exit (-1);
  }

  if ( fclose ( rulesfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close rules file\n" );
    exit (-1);
  }
}

void populateFirstSets ( FILE *firstsfile, int blocksize, LINKEDLIST* firsts [], TRIE* mixedbag )
{
  char c;

  char buffers [2] [blocksize];
  char token [ MAXRULE ];
  char *finalterms = NULL;

  int charindx = -1;
  int curbuff = -1;
  int charsread = 0;
  int tokencounter = 0;

  TNODE *temp = NULL;

  while ( TRUE )
  {
    // Read the next character from the buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, firstsfile ) ) == 0 )
        break;
    }
    c = buffers [ curbuff ] [ charindx ];

    if ( charsread < blocksize && charindx >= charsread )
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == NEWLINE )
      token [ tokencounter ] = '\0';
    else
      token [ tokencounter ] = c;

    tokencounter++;

    // tokencounter == 1 indicates a blank line (only NULL character)
    if ( c == NEWLINE && tokencounter != 1 )
    {
      tokencounter = 0;
      finalterms = strtok (token , " " );

      temp = findString ( mixedbag , finalterms );
      if ( temp == NULL )
        fprintf ( stderr , "Token not Found.\n" );
      else
      {
        firsts [ temp -> data.int_val ] = getLinkedList ( LL_STRING_TYPE );
        finalterms = strtok ( NULL , " " );

        while ( finalterms != NULL)
        {
          firsts [ temp -> data.int_val ] = insertAtBack ( firsts [ temp -> data.int_val ] , finalterms );
          finalterms = strtok ( NULL , " " );
        }
      }
    }
  }
}

int **populateParseTable ( FILE *grammarfile, int blocksize, LINKEDLIST* ruleLists [],
                          LINKEDLIST* firsts [], TRIE* gramrules, TRIE* nonterm,
                          TRIE* terminals, TRIE* mixedbag, int nonterminalcount,
                          int terminalcount )
{
  int **parseTable;

  parseTable = malloc ( nonterminalcount * sizeof ( int ** ) );

  if ( parseTable == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for parse table\n" );
    return NULL;
  }

  int i;
  for ( i = 0; i < nonterminalcount; i++ )
  {
    parseTable [i] = NULL;
    parseTable [i] = malloc ( terminalcount * sizeof ( int ) );

    if ( parseTable [i] == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for row of parse table\n" );
      return NULL;
    }
  }

  for ( i = 0; i < nonterminalcount; i++)
  {
    int j;
    for ( j = 0; j < terminalcount; j++ )
      parseTable [i][j] = -1;
  }

  char c;

  char buffers [2] [blocksize];
  char token [ MAXRULE ];

  int nodevalue = -1;    // Index of Linked List
  int ntvalue = -1;      // Index of Parse Table row
  int tvalue = -1;       // Index of Parse Table col
  int ntindex = -1;      // Index of Non Terminal Trie
  int epsflag = 0;

  int ruleno = -1;
  int charindx = -1;
  int curbuff = -1;
  int charsread = 0;
  int tokencounter = 0;
  int marker = 0;

  char *val = NULL;
  char *nttoken = NULL;

  LNODE currnode;        // temporary node for Rules LL
  LNODE firstnode;       // temporary node for Firsts LL
  TNODE *temp = NULL;

  while ( TRUE )
  {
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, grammarfile ) ) == 0 )
        break;
    }

    c = buffers [ curbuff ] [ charindx ];


    if ( charsread < blocksize && charindx >= charsread )
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == '-' )
      marker = tokencounter;

    if ( c == NEWLINE )
      token [ tokencounter ] = '\0';
    else
      token [ tokencounter ] = c;

    tokencounter++;

    if ( token [0] == '\0' )
       break;

    if ( c == NEWLINE )
    {
      ruleno++;
      temp = findString ( gramrules , token );
      nodevalue = temp -> data.int_val;
      tokencounter = 0;

      //Finding Non-Terminals index
      nttoken = strtok ( token , "-" );
      nttoken [ marker - 1 ] = '\0';

      temp = findString ( nonterm , nttoken );
      ntvalue = temp -> data.int_val;

      //Getting the RHS of the Non-Terminal and populating Parse Table
      if ( getIterator ( ruleLists [ nodevalue ], &currnode ) == NULL )
      {
        fprintf ( stderr, "Failed to get iterator for current node\n" );
        exit (-1);
      }

      while ( hasNext ( &currnode ) )
      {
        getNext ( ruleLists [ nodevalue ], &currnode );
        val = currnode.data.string_val;

        if ( val[0] == 'e' )
        {
          temp = findString ( terminals , "e" );
          tvalue = temp -> data.int_val;
          parseTable [ ntvalue ] [ tvalue ] = ruleno;
          continue;
        }

        if ( val[0] == 'T' )
        {
          temp = findString ( terminals , val );
          tvalue = temp -> data.int_val;
          parseTable [ ntvalue ] [ tvalue ] = ruleno;
          break;
        }

        temp = findString ( mixedbag , val );
        ntindex = temp -> data.int_val;

        //Getting the Firsts of the respective non-terminal
        if ( getIterator ( firsts [ntindex], &firstnode ) == NULL )
        {
          fprintf ( stderr, "Failed to get iterator for first node\n" );
          exit (-1);
        }

        while ( hasNext ( &firstnode ) )
        {
          getNext ( firsts [ ntindex ], &firstnode );
          val = firstnode.data.string_val;

          if ( val[0] == 'e' )
          {
            epsflag = 1;
            break;
          }

          temp = findString ( terminals , val );
          tvalue = temp -> data.int_val;

          parseTable [ ntvalue ] [ tvalue ] = ruleno;
        }

        //Adding entry for Parse Table for 'e'
        if ( epsflag == 1 )
        {
          if ( ! hasNext ( &currnode ) )
          {
            temp = findString ( terminals , "e" );
            tvalue = temp -> data.int_val;
            parseTable [ ntvalue ] [ tvalue ] = ruleno;
          }

          epsflag = 0;
        }
        else
          break;
      }
    }
  }

  return parseTable;
}

int main ( )
{

  /***************************************************
    *
    * PHASE 1 : Count the number of lines in the file
    *
    **************************************************
  **/

  FILE *grammarfile = NULL;
  grammarfile = fopen ( GRAMMAR_FILE , "rb" );

  if ( grammarfile == NULL )
  {
    fprintf ( stderr, "Failed to open grammar file for reading\n" );
    return -1;
  }

  // Get the system block size
  struct stat fi;
  stat ( "/", &fi );
  int blocksize = fi.st_blksize;

  int lines = getLineCount ( grammarfile, blocksize );

  if ( fclose ( grammarfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close the grammar file\n" );
    return -1;
  }
  grammarfile = NULL;

  fprintf ( stderr, "PHASE 1 complete\n" );

  /***************************************************
    *
    * PHASE 2 : Populate all the required TRIEs
    *
    **************************************************
  **/


  LINKEDLIST* ruleLists [lines];

  // Initialize each Linked List to null
  int i;
  for ( i = 0; i < lines; i++ )
    ruleLists [ i ] = NULL;

  TRIE *gramrules = NULL, *nonterm = NULL, *terminals = NULL, *mixedbag = NULL;

  gramrules = getNewTrie( TR_INT_TYPE );
  gramrules = setTrieName( gramrules , RULE_TRIE_NAME );

  nonterm = getNewTrie( TR_INT_TYPE );
  nonterm = setTrieName( nonterm , NT_TRIE_NAME );

  terminals = getNewTrie( TR_INT_TYPE );
  terminals = setTrieName( terminals , T_TRIE_NAME );

  mixedbag = getNewTrie( TR_INT_TYPE );
  mixedbag = setTrieName( mixedbag , TNT_TRIE_NAME );

  int tokencounts [ 3 ];

  grammarfile = fopen ( GRAMMAR_FILE, "rb" );

  if ( grammarfile == NULL )
  {
    fprintf ( stderr, "Failed to open grammar file to populate tries\n" );
    return -1;
  }

  populateTries ( grammarfile, blocksize, ruleLists, gramrules,
                  nonterm, terminals, mixedbag, tokencounts );

  if ( fclose ( grammarfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close grammar file\n" );
    return -1;
  }
  grammarfile = NULL;

  // Reassign count variables
  int terminalcount = tokencounts [0];
  int nonterminalcount = tokencounts [1];
  int mixcounter = tokencounts [2];

  fprintf ( stderr, "PHASE 2 complete\n" );

  /***************************************************
    *
    * PHASE 3 : Populate the first sets for all
    *           terminals and non-terminals
    *
    **************************************************
  **/


  FILE *firstsfile = NULL;
  firstsfile = fopen ( FIRST_SETS_FILE, "rb" );

  if ( firstsfile == NULL )
  {
    fprintf ( stderr, "Failed to open first sets file\n" );
    return -1;
  }

  LINKEDLIST *firsts [ mixcounter ];
  for ( i = 0; i < mixcounter; i++ )
    firsts [i] = NULL;

  populateFirstSets ( firstsfile, blocksize, firsts, mixedbag );

  if ( fclose ( firstsfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close the firsts file\n" );
    return -1;
  }


  fprintf ( stderr, "PHASE 3 complete\n" );

  /***************************************************
    *
    * PHASE 4 : Generate the final parse table
    *
    **************************************************
  **/


  grammarfile = fopen ( GRAMMAR_FILE , "r" );

  if ( grammarfile == NULL )
  {
    fprintf ( stderr, "Failed to open grammar file to generate parse table\n" );
    return -1;
  }

  int **parseTable = NULL;

  parseTable = populateParseTable ( grammarfile, blocksize, ruleLists, firsts,
                                    gramrules, nonterm, terminals, mixedbag,
                                    nonterminalcount, terminalcount );

  if ( parseTable == NULL )
  {
    fprintf ( stderr, "Failed to populate parse table\n" );
    return -1;
  }

  if ( fclose ( grammarfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close grammar file after populating parse table\n" );
    return -1;
  }

  fprintf ( stderr, "Parse Table generated\n" );

  FILE *parsetablefile = NULL;
  parsetablefile = fopen ( PARSE_TABLE_FILE, "w+" );

  if ( parsetablefile == NULL )
  {
    fprintf ( stderr, "Failed to open parse table file\n" );
    return -1;
  }

  int rindx = 0, cindx = 0;
  for ( rindx = 0; rindx < nonterminalcount; rindx++ )
  {
    for ( cindx = 0; cindx < terminalcount; cindx++ )
      fprintf ( parsetablefile, "%d ", parseTable [rindx] [cindx] );
    fprintf ( parsetablefile, "\n" );
  }

  if ( fclose ( parsetablefile ) != 0 )
  {
    fprintf ( stderr, "Failed to close parse table file\n" );
    return -1;
  }

  return 0;
}

