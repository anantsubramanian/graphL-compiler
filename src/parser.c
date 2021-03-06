#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#ifndef TRIE_DEFINED
  #include "headers/trie.h"
#endif

#ifndef STACK_DEFINED
  #include "headers/stack.h"
#endif

#ifndef CONSTANTS_DEFINED
  #include "headers/constants.h"
#endif

#ifndef PARSE_UTILS_DEFINED
  #include "headers/parse_utils.h"
#endif

#define PARSER_STACK_NAME "Parser Stack"
#define MAXRULE 200
#define BUFFERLEN 200
#define MAX_ERRORS 15
#define NO_TRANSITION -1
#define NEWLINE '\n'
#define COMMA ','

void populateTerminalNames ( FILE *tnamemapfile, int blocksize, char **terminalnames, int maxvalue )
{
  char buffers [2] [ blocksize ];
  int curbuff = -1;
  int charindx = -1;
  int charsread = 0;
  int tokenindex = 0;
  int torval = 0;

  char c;
  char token [ BUFFERLEN ];
  int value = 0;

  while ( TRUE )
  {
    // Get char from appropriate buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( (charsread = fread ( buffers [ curbuff ], sizeof ( char ),
                                blocksize, tnamemapfile ) ) == 0 )
        break;
    }
    c = buffers [ curbuff ] [ charindx ];

    if ( charsread < blocksize && charindx >= charsread )
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == ' ' && torval == 0 )
    {
      torval = 1;
      tokenindex = 0;
    }
    else if ( c == NEWLINE )
    {
      if ( value > maxvalue )
      {
        fprintf ( stderr, "Mapping a terminal name to a wrong value\n" );
        exit (-1);
      }

      token [ tokenindex ] = '\0';
      terminalnames [ value ] = malloc ( ( tokenindex + 1 ) * sizeof ( char ) );
      strcpy ( terminalnames [ value ], token );

      value = 0;
      torval = 0;
    }
    else if ( torval == 1 )
      token [ tokenindex++ ] = c;
    else
      value = value * 10 + c - 48;
  }
}

void populateGrammarRules ( FILE *rulesfile, int blocksize, LINKEDLIST* ruleLists [] )
{
  char c;

  char buffers [2] [blocksize];
  char token [ MAXRULE ];

  int curbuff = -1;
  int charindx = -1;
  int charsread = 0;
  int tokenindex = 0;

  int value = 0;
  int torval = 1;

  while ( TRUE )
  {
    // Read next character from the buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, rulesfile ) ) == 0 )
        break;
    }
    c = buffers [ curbuff ] [ charindx ];

    if ( charsread < blocksize && charindx >= charsread )
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == NEWLINE )
    {
      token [ tokenindex ] = '\0';
      ruleLists [value] = getLinkedList ( LL_STRING_TYPE );
      ruleLists [value] = insertSpaceSeparatedWords ( ruleLists [value], token );
      value = 0;
      torval = 1;
    }
    else if ( c == ' ' && torval == 1 )
    {
      torval = 0;
      tokenindex = 0;
    }
    else if ( torval == 1 )
      value = value * 10 + c - 48;
    else
      token [ tokenindex++ ] = c;
  }
}

void populateParseTable ( FILE *ptablefile, int blocksize, int **parseTable )
{
  char c;

  char buffers [2] [blocksize];

  int curbuff = -1;
  int charindx = -1;
  int charsread = 0;
  int currow = 0;
  int curcol = 0;

  int value = 0;
  int negative = 0;

  while ( TRUE )
  {
    // Read next character from the buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, ptablefile ) ) == 0 )
        break;
    }
    c = buffers [ curbuff ] [ charindx ];

    if ( charsread < blocksize && charindx >= charsread )
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == NEWLINE )
    {
      currow++;
      curcol = 0;
      value = 0;
      negative = 0;
    }
    else if ( c == ' ' )
    {
      if ( negative == TRUE )
        value = -value;
      parseTable [currow] [curcol] = value;

      curcol++;
      value = 0;
      negative = 0;
    }
    else if ( c == '-' )
      negative = TRUE;
    else
      value = value * 10 + c - 48;
  }
}

void populateAttributes ( FILE *ptablefile, int blocksize, char attributes [] [MAXRULE] )
{
  char c;

  char buffers [2] [blocksize];
  char token [ MAXRULE ];

  int curbuff = -1;
  int charindx = -1;
  int charsread = 0;
  int tokenindex = 0;

  int torval = 1;
  int value = 0;

  while ( TRUE )
  {
    // Read next character from the buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, ptablefile ) ) == 0 )
        break;
    }
    c = buffers [ curbuff ] [ charindx ];

    if ( charsread < blocksize && charindx >= charsread )
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == NEWLINE )
    {
      token [ tokenindex ] = '\0';
      strcpy ( attributes [ value ], token );

      torval = 1;
      value = 0;
    }
    else if ( c == ' ' && torval == 1 )
    {
      tokenindex = 0;
      torval = 0;
    }
    else if ( torval == 0 )
      token [ tokenindex++ ] = c;
    else
      value = value * 10 + c - 48;
  }
}

void parseInputProgram ( FILE *inputfile, int blocksize, int **parseTable,
                         TRIE* terminals, TRIE* nonterminals, LINKEDLIST* ruleLists [],
                         char **terminalnames, char attributes [] [ MAXRULE ], char *inputprogram )
{
  FILE *parseout = NULL;
  parseout = fopen ( PARSE_OUTPUT, "w+" );

  FILE *parseerr = NULL;
  parseerr = fopen ( PARSE_ERRORS, "w+" );

  if ( parseout == NULL || parseerr == NULL )
  {
    fprintf ( stderr, "Failed to open parser output/error file\n" );
    exit (-1);
  }

  STACK *stack = NULL;
  stack = getStack ( STACK_STRING_TYPE );
  stack = setStackName ( stack, PARSER_STACK_NAME );

  stack = push ( stack, START_SYMBOL );
  fprintf ( parseout, "%s 1\n", START_SYMBOL );

  char c;

  char buffers [2] [blocksize];
  char token [ MAXRULE ];

  int curbuff = -1;
  int charindx = -1;
  int charsread = 0;
  int tokenindex = 0;

  int epscolumn = findString ( terminals, "e" ) -> data.int_val;

  int value = 0;
  int linenum = 0;
  int torvalorlno = 0;

  int in_error_state = FALSE;
  int errorcount = 0;
  TNODE* tempnode = NULL;

  TNODE* newlinenode = NULL;
  if ( ( newlinenode = findString ( terminals, NEWLINE_SYMBOL ) ) == NULL )
  {
    fprintf ( stderr, "Failed to find newline terminal\n" );
    exit (-1);
  }

  int newlineterm = newlinenode -> data.int_val;

  TNODE* identifiernode = NULL;
  if ( ( identifiernode = findString ( terminals, IDENTIFIER_SYMBOL ) ) == NULL )
  {
    fprintf ( stderr, "Failed to find identifier terminal\n" );
    exit (-1);
  }

  int identifierterm = identifiernode -> data.int_val;

  TNODE* stringlitnode = NULL;
  if ( ( stringlitnode = findString ( terminals, STRINGLIT_SYMBOL ) ) == NULL )
  {
    fprintf ( stderr, "Failed to find string literal terminal\n" );
    exit (-1);
  }

  int stringlitterm = stringlitnode -> data.int_val;

  TNODE* floatlitnode = NULL;
  if ( ( floatlitnode = findString ( terminals, FLOATLIT_SYMBOL ) ) == NULL )
  {
    fprintf ( stderr, "Failed to find float literal terminal\n" );
    exit (-1);
  }

  int floatlitterm = floatlitnode -> data.int_val;

  TNODE* intlitnode = NULL;
  if ( ( intlitnode = findString ( terminals, INTLIT_SYMBOL ) ) == NULL )
  {
    fprintf ( stderr, "Failed to find int literal terminal\n" );
    exit (-1);
  }

  int intlitterm = intlitnode -> data.int_val;

  while ( TRUE )
  {
    // Read next character from the buffer
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
      // Input token stream has ended, stack should be empty / should be nullable
      if ( ! isEmpty ( stack ) )
      {
        while ( ! isEmpty ( stack ) )
        {
          char *topval = strdup ( (char *) top ( stack ) );
          if ( topval == NULL )
          {
            fprintf ( stderr, "Failed to allocate memory to duplicate top of stack\n" );
            exit (-1);
          }

          stack = pop ( stack );

          TNODE *nonterm = findString ( nonterminals, topval );
          if ( findString ( terminals, topval ) != NULL || nonterm == NULL )
          {
            fprintf ( parseerr, "Error at line %d:\n\tUnexpected end to input program.\n", linenum );
            if ( fclose ( parseerr ) != 0 )
              fprintf ( stderr, "Failed to close parse error file\n" );

            exit (-1);
          }

          // Free allocated memory for duplicate top of stack
          free ( topval );
          topval = NULL;

          int nontermval = nonterm -> data.int_val;
          if ( parseTable [ nontermval ] [ epscolumn ] == NO_TRANSITION )
          {
            fprintf ( parseerr, "Error at line %d:\n\tUnexpected end to input program.\n", linenum );
            if ( fclose ( parseerr ) != 0 )
              fprintf ( stderr, "Failed to close parse error file\n" );

            exit (-1);
          }
        }
      }
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == NEWLINE )
    {
      TNODE *tomatch = findString ( terminals, token + 1 );
      if ( tomatch == NULL )
      {
        fprintf ( parseerr, "FATAL ERROR: Unrecognized input token %s\nConsider ", token + 1 );
        fprintf ( parseerr, "re-compiling the lexer module\n" );
        if ( fclose ( parseerr ) != 0 )
          fprintf ( stderr, "Failed to close parse errors file\n" );

        exit (-1 );
      }

      int column = tomatch -> data.int_val;

      if ( in_error_state )
      {
        if ( column == newlineterm )
        {
          // Found synchronization newline, continue with next token
          in_error_state = FALSE;
        }

        tokenindex = 0;
        torvalorlno = 0;
        continue;
      }

      while ( TRUE )
      {
        if ( isEmpty ( stack ) )
        {
          FILE* programfile = NULL;
          programfile = fopen ( inputprogram, "rb" );

          if ( programfile == NULL )
          {
            fprintf ( stderr, "Failed to open input program to print errors\n" );
            exit (-1);
          }

          fprintf ( parseerr, "Error at line %d: %s\n\tTrailing characters at the end of ",
                    linenum, getLine ( programfile, blocksize, linenum ) );
          fprintf ( parseerr, "the program\n" );

          if ( fclose ( parseerr ) != 0 )
            fprintf ( stderr, "Failed to close parse errors file\n" );

          if ( fclose ( programfile ) != 0 )
            fprintf ( stderr, "Failed to close input program used to display errors\n" );

          exit (-1);
        }

        char *topval = strdup ( (char *) top ( stack ) );
        if ( topval == NULL )
        {
          fprintf ( stderr, "Failed to allocate memory to duplicate top of stack\n" );
          exit (-1);
        }

        stack = pop ( stack );

        TNODE *current = findString ( nonterminals, topval );

        if ( current != NULL )
        {
          // Top of stack is a non-terminal
          int nontermindex = current -> data.int_val;

          if ( parseTable [ nontermindex ] [ column ] != NO_TRANSITION )
          {
            // Print rules to parser output
            LNODE iterator;
            LINKEDLIST *requiredRule = ruleLists [ parseTable [ nontermindex ] [ column ] ];
            getIterator (  requiredRule, &iterator );
            while ( hasNext ( &iterator ) )
            {
              getNext ( requiredRule, &iterator );

              if ( ( tempnode = findString ( terminals, iterator.data.string_val ) )  != NULL )
              {
                if ( tempnode -> data.int_val == identifierterm
                     || tempnode -> data.int_val == stringlitterm
                     || tempnode -> data.int_val == intlitterm
                     || tempnode -> data.int_val == floatlitterm)
                  fprintf ( parseout, "<%s,%s,%d> ", iterator.data.string_val , attributes [ value ] , linenum );
                else
                  fprintf ( parseout, "%s " , iterator.data.string_val );
              }
              else
                fprintf ( parseout, "%s ", iterator.data.string_val );
            }

            fprintf ( parseout, "%d\n", linenum );

            stack = insertFromLinkedList ( stack, ruleLists [ parseTable [ nontermindex ] [ column ] ] );
          }
          else if ( parseTable [ nontermindex ] [ epscolumn ] != NO_TRANSITION )
          {
            fprintf ( parseout, "e %d\n", linenum );
            continue;
          }
          else
          {
            // In an error state, fall back till synchronization newline
            in_error_state = TRUE;

            FILE* programfile = NULL;
            programfile = fopen ( inputprogram, "rb" );

            if ( programfile == NULL )
            {
              fprintf ( stderr, "Failed to open input program to print errors\n" );
              exit (-1);
            }

            fprintf ( parseerr, "Error at line %d: %s\n\t%s ",
                      linenum, getLine ( programfile, blocksize, linenum ), terminalnames [ column ] );

            if ( column == identifierterm || column == stringlitterm ||
                 column == floatlitterm || column == intlitterm )
            {
              fprintf ( parseerr, "%s ", attributes [ value ] );
            }

            fprintf ( parseerr, "not expected at this point\n" );

            if ( fclose ( programfile ) != 0 )
              fprintf ( stderr, "Failed to close input program used to display errors\n" );

            while ( !isEmpty ( stack ) )
            {
              char *topval = (char *) top ( stack );
              TNODE *temp = findString ( terminals, topval );
              stack = pop ( stack );

              if ( temp == NULL )
                continue;

              // If synchronization newline is found, break
              if ( temp -> data.int_val == newlineterm )
                break;
            }

            errorcount++;
            if ( errorcount == MAX_ERRORS )
            {
              if ( fclose ( parseerr ) != 0 )
                fprintf ( stderr, "Failed to close errors file\n" );

              exit (-1);
            }

            // Token stream already at synchronization point
            // Can continue normal parsing
            if ( column == newlineterm )
              in_error_state = FALSE;

            break;
          }
        }
        else
        {
          // Top of the stack is a terminal
          current = findString ( terminals, topval );

          if ( current == NULL )
          {
            fprintf ( parseerr, "FATAL ERROR: Unrecognized terminal %s encountered ", topval + 1 );
            fprintf ( parseerr, "while parsing.\nConsider re-checking grammar rules.\n" );
            if ( fclose ( parseerr ) != 0 )
              fprintf ( stderr, "Failed to close parse errors file\n" );

            exit (-1);
          }
          int stackterminal = current -> data.int_val;

          if ( stackterminal != column )
          {
            // In error state, fall back till synchronization newline
            in_error_state = TRUE;

            FILE* programfile = NULL;
            programfile = fopen ( inputprogram, "rb" );

            if ( programfile == NULL )
            {
              fprintf ( stderr, "Failed to open input program to print errors\n" );
              exit (-1);
            }

            fprintf ( parseerr, "Error at line %d: %s\n\tExpected \"%s\" but got %s ",
                      linenum, getLine ( programfile, blocksize, linenum ),
                      terminalnames [ stackterminal ], terminalnames [ column ] );

            if ( column == identifierterm || column == stringlitterm || column == intlitterm
                 || column == floatlitterm )
              fprintf ( parseerr, "%s instead\n", attributes [ value ] );
            else
              fprintf ( parseerr, "instead\n" );

            if ( fclose ( programfile ) != 0 )
              fprintf ( stderr, "Failed to close input program used to display errors\n" );

            errorcount++;
            if ( errorcount == MAX_ERRORS )
            {
              if ( fclose ( parseerr ) != 0 )
                fprintf ( stderr, "Failed to close errors file\n" );

              exit (-1);
            }

            // Stack already at newline synchronization, only token stream
            // needs to be synchronized
            if ( stackterminal == newlineterm )
              break;

            while ( !isEmpty ( stack ) )
            {
              char *topval = (char *) top ( stack );
              TNODE *temp = findString ( terminals, topval );
              stack = pop ( stack );

              if ( temp == NULL )
                continue;

              // If synchronization newline is found, break
              if ( temp -> data.int_val == newlineterm )
                break;
            }

            if ( in_error_state )
              break;
          }
          else
          {
            free ( topval );
            topval = NULL;
            break;    // Found the required terminal, so exit the while loop
          }
        }

        free ( topval );
        topval = NULL;
      }

      tokenindex = 0;
      torvalorlno = 0;
    }
    else if ( c == COMMA )
    {
      token [ tokenindex ] = '\0';

      if ( torvalorlno == 0 )
        value = 0;
      else
        linenum = 0;

      torvalorlno++;
    }
    else if ( c == '>' )
    {
      if ( torvalorlno == 1 )
      {
        linenum = value;
        value = -1;
      }
    }
    else if ( torvalorlno == 0 )
      token [ tokenindex++ ] = c;
    else if ( torvalorlno == 1 )
      value = value * 10 + c - 48;
    else if ( c != '>' )
      linenum = linenum * 10 + c - 48;
  }

  if ( fclose ( parseout ) != 0 )
    fprintf ( stderr, "Failed to close parser output file\n" );
  if ( fclose ( parseerr ) != 0 )
    fprintf ( stderr, "Failed to close parser errors file\n" );
}

int main ( int argc, char *argv[] )
{

  if ( argc <= 1 )
  {
    fprintf ( stderr, "Please provide the input program as an argument\n" );
    return -1;
  }

  // Get the system block size
  struct stat fi;
  stat ( "/", &fi );
  int blocksize = fi.st_blksize;

  TRIE* terminals = NULL, *nonterminals = NULL;
  int terminalscount = 0, nonterminalscount = 0;
  char **terminalnames = NULL;

  terminals = getNewTrie ( TRIE_INT_TYPE );
  nonterminals = getNewTrie ( TRIE_INT_TYPE );

  FILE *tmapfile = NULL, *ntmapfile = NULL;
  FILE *tnamemapfile = NULL;



  /***********************************************************
    *                                                        *
    *   PHASE 1 : Populate terminal and non-terminal tries   *
    *             and terminals true names                   *
    *                                                        *
    **********************************************************
   */



  tmapfile = fopen ( T_INDEX_FILE, "rb" );
  ntmapfile = fopen ( NT_INDEX_FILE, "rb" );
  tnamemapfile = fopen ( T_NAMEMAP_FILE, "rb" );

  if ( tmapfile == NULL || ntmapfile == NULL || tnamemapfile == NULL )
  {
    fprintf ( stderr, "Failed to open (non)terminals index file " );
    fprintf ( stderr, "or terminals true names file\n" );
    return -1;
  }

  populateTrie ( tmapfile, blocksize, terminals, &terminalscount );
  populateTrie ( ntmapfile, blocksize, nonterminals, &nonterminalscount );

  // Convert indices to limits
  terminalscount++;
  nonterminalscount++;

  if ( fclose ( tmapfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close terminals map file\n" );
    return -1;
  }
  if ( fclose ( ntmapfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close non-terminals map file\n" );
    return -1;
  }

  tmapfile = ntmapfile = NULL;

  // Now set terminal names from file
  terminalnames = malloc ( terminalscount * sizeof ( char * ) );
  if ( terminalnames == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for terminal names array\n" );
    return -1;
  }

  int i;
  for ( i = 0; i < terminalscount; i++ )
    terminalnames [i] = NULL;

  populateTerminalNames ( tnamemapfile, blocksize, terminalnames, terminalscount );

  if ( fclose ( tnamemapfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close terminals name map file\n" );
    return -1;
  }

  tnamemapfile = NULL;


  fprintf ( stderr, "PHASE 1 complete\n" );


  /***********************************************************
    *                                                        *
    *       PHASE 2 : Populate linked list of rules          *
    *                                                        *
    **********************************************************
   */



  FILE *rulesfile = NULL;
  rulesfile = fopen ( RULES_FILE, "rb" );

  if ( rulesfile == NULL )
  {
    fprintf ( stderr, "Failed to open rules file\n" );
    return -1;
  }

  int linecount = getLineCount ( rulesfile, blocksize );

  if ( fclose ( rulesfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close rules file\n" );
    return -1;
  }
  rulesfile = NULL;

  LINKEDLIST* ruleLists [linecount];
  for ( i = 0; i < linecount; i++ )
    ruleLists [i] = NULL;

  rulesfile = fopen ( RULES_FILE, "rb" );

  if ( rulesfile == NULL )
  {
    fprintf ( stderr, "Failed to open rules file\n" );
    return -1;
  }

  populateGrammarRules ( rulesfile, blocksize, ruleLists );

  if ( fclose ( rulesfile ) != 0 )
  {
    fprintf ( stderr, "Failed to close rules file\n" );
    return -1;
  }


  fprintf ( stderr, "PHASE 2 complete\n" );


  /***********************************************************
    *                                                        *
    *            PHASE 3 : Populate Parse Table              *
    *                                                        *
    **********************************************************
   */


  int **parseTable = NULL;

  parseTable = malloc ( nonterminalscount * sizeof ( int * ) );
  if ( parseTable == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for parse table\n" );
    return -1;
  }

  for ( i = 0; i < nonterminalscount; i++ )
  {
    parseTable [i] = NULL;
    parseTable [i] = malloc ( terminalscount * sizeof ( int ) );

    if ( parseTable [i] == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for row of parse table\n" );
      return -1;
    }
  }

  FILE *parsetablefile = NULL;
  parsetablefile = fopen ( PTABLE_FILE, "rb" );

  if ( parsetablefile == NULL )
  {
    fprintf ( stderr, "Failed to open parse table file\n" );
    return -1;
  }

  populateParseTable ( parsetablefile, blocksize, parseTable );

  if ( fclose ( parsetablefile ) != 0 )
  {
    fprintf ( stderr, "Failed to close parse table file\n" );
    return -1;
  }


  fprintf ( stderr, "PHASE 3 complete\n" );


  /***********************************************************
    *                                                        *
    *               PHASE 4 : Parse Input File               *
    *                                                        *
    **********************************************************
   */



  FILE *inputfile = NULL, *attributefile = NULL;
  attributefile = fopen ( DICT_FILE, "rb" );

  if ( attributefile == NULL )
  {
    fprintf ( stderr, "Failed to open attribute map file\n" );
    return -1;
  }

  int attributelines = getLineCount ( attributefile, blocksize );

  if ( fclose ( attributefile ) != 0 )
  {
    fprintf ( stderr, "Failed to close attributes file\n" );
    return -1;
  }
  attributefile = NULL;

  char attributes [ attributelines ] [ MAXRULE ];

  attributefile = fopen ( DICT_FILE, "rb" );

  if ( attributefile == NULL )
  {
    fprintf ( stderr, "Failed to open attribute map file\n" );
    return -1;
  }

  populateAttributes ( attributefile, blocksize, attributes );

  if ( fclose ( attributefile ) != 0 )
  {
    fprintf ( stderr, "Failed to close attributes file\n" );
    return -1;
  }

  inputfile = fopen ( TOKENS_FILE, "rb" );
  if ( inputfile == NULL )
  {
    fprintf ( stderr, "Failed to open program to parse\n" );
    return -1;
  }

  parseInputProgram ( inputfile, blocksize, parseTable, terminals,
                      nonterminals, ruleLists, terminalnames,
                      attributes, argv [1] );

  return 0;
}

