#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "headers/trie.h"
#include "headers/linkedlist.h"

#define BUFFERLEN 200
#define NEWLINE '\n'
#define MAXRULE 160
#define FIRST_SETS_FILE "config/first_sets"

int main ( int argc, char * argv [] )
{
  char c;

  int curbuff = -1;
  int charindx = -1;
  int lines = 0;
  int totalblocks = 0;
  int charsread = 0;

  FILE *inputfile = NULL;
  FILE *grammarfile = NULL;

  if ( argc <= 1 )
  {
    fprintf ( stderr, "Please provide the grammar file\n" );
    exit ( 1 );
  }

  inputfile = fopen ( argv [1] , "rb" );

  // Get the system block size
  struct stat fi;
  stat ( "/", &fi );
  int blocksize = fi.st_blksize;
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
      totalblocks++;
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

  LINKEDLIST* ruleLists [lines];

  int i;
  for ( i = 0; i < lines; i++ )
    ruleLists [ i ] = NULL;

  char buff [ blocksize ];
  char token [ MAXRULE ];
  char *nttoken = NULL, *ttoken = NULL;
  char *rhs = NULL, *finalterms = NULL;

  curbuff = -1;
  charindx = -1;
  charsread = 0;

  int ruleno = -1;
  int tokencounter = 0;
  int mixcounter = 0;
  int marker = 0;
  int terminalcount = 0;
  int nonterminalcount = 0;

  grammarfile = fopen ( argv[1], "r" );

  TRIE *gramrules = NULL, *nonterm = NULL, *terminals = NULL, *mixedbag = NULL;
  TNODE *temp = NULL;

  gramrules = getNewTrie();
  gramrules = setTrieName( gramrules , "Grammar Rules" );

  nonterm = getNewTrie();
  nonterm = setTrieName( nonterm , "Non Terminals");

  terminals = getNewTrie();
  terminals = setTrieName( terminals , "Terminals" );

  mixedbag = getNewTrie();
  mixedbag = setTrieName( mixedbag , "Terminals and Non Terminals" );

  while ( TRUE )
  {
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      totalblocks--;
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

    if ( totalblocks == 0 && (charindx == charsread - 1))
      break;

    if ( c == '-' )
      marker = tokencounter;

    if ( c == NEWLINE )
      token [ tokencounter ] = '\0';

    else
      token [ tokencounter ] = c;

    tokencounter++;

    if ( c == NEWLINE )
    {
      // Pushing a Rule to the Trie
      ruleno++;
      temp = insertString ( gramrules , token );
      temp = setValue ( temp , ruleno );
      tokencounter = 0;

      // Inserting into Non-Terminals Trie
      rhs = token;
      nttoken = strtok(rhs , "-");
      nttoken [ marker - 1 ] = '\0';

      temp = findString ( nonterm , nttoken );
      if ( temp == NULL )
      {
        temp = insertString ( nonterm , nttoken );
        temp = setValue ( temp , nonterminalcount++ );
      }

      // Inserting Non-Terminals into the MixedBag Trie
      temp = findString ( mixedbag , nttoken );
      if ( temp == NULL )
      {
        temp = insertString ( mixedbag , nttoken );
        temp = setValue ( temp , mixcounter++ );
      }

      nttoken = strtok ( NULL, "-" );

       // Inserting into Array of Linked List
      ruleLists [ruleno] = getLinkedList();
      ruleLists [ruleno] = insertSpaceSeparatedWords ( ruleLists[ruleno] , nttoken + 2 );

      // Inserting into Terminals Trie and MixedBag Trie
      finalterms = strdup ( nttoken + 1 );
      ttoken = strtok ( finalterms , " " );

      while ( ttoken != NULL )
      {
        if ( ttoken [0] == 'T' )
        {
          temp = findString ( terminals , ttoken );
          if ( temp == NULL )
          {
            temp = insertString ( terminals , ttoken );
            temp = setValue ( temp , terminalcount++ );
          }
          temp = findString ( mixedbag , ttoken );
          if ( temp == NULL )
          {
            temp = insertString ( mixedbag , ttoken );
            temp = setValue ( temp , mixcounter++ );
          }
        }

        ttoken = strtok ( NULL , " " );
      }
    }
  }

  temp = insertString ( terminals , "e" );
  temp = setValue ( temp , terminalcount++ );

  fclose ( inputfile );
  fclose ( grammarfile );

  FILE *firstsets = NULL;
  firstsets = fopen ( FIRST_SETS_FILE, "r" );

  LINKEDLIST *firsts [ mixcounter ];
  for ( i = 0; i < mixcounter; i++ )
    firsts [i] = NULL;

  charindx = -1;
  curbuff = -1;
  charsread = 0;
  tokencounter = 0;

  while ( TRUE )
  {
    charindx = ( charindx + 1 ) % blocksize;

    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, firstsets ) ) == 0 )
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

    if ( c == NEWLINE )
    {
      tokencounter = 0;
      finalterms = strtok (token , " " );

      temp = findString ( mixedbag , finalterms );
      if ( temp == NULL )
        fprintf ( stderr , "Token not Found.\n" );
      else
      {
        firsts [ temp -> value ] = getLinkedList();
        finalterms = strtok ( NULL , " " );

        while ( finalterms != NULL)
        {
          firsts [ temp -> value ] = insertAtBack ( firsts [ temp -> value ] , finalterms );
          finalterms = strtok ( NULL , " " );
        }
      }
    }
  }

  fclose ( firstsets );
  grammarfile = fopen ( argv[1] , "r" );

  int parseTable [ nonterminalcount ] [ terminalcount ];
  memset ( parseTable, -1, sizeof( parseTable ));

  int nodevalue; // INDEX OF LINKED LIST
  int ntvalue; //index of parse table row
  int tvalue; //index of parse table col
  int ntindex; //index of NT trie
  int epsflag = 0;

  ruleno = -1;
  char *val = NULL;
  charindx = -1;
  curbuff = -1;
  charsread = 0;
  tokencounter = 0;

  LNODE *currnode = NULL; //temporary node for Rules LL
  LNODE *firstnode = NULL; //temporary node for Firsts LL

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
      nodevalue = temp -> value;
      tokencounter = 0;

      //Finding Non-Terminals index
      nttoken = strtok ( token , "-" );
      nttoken [ marker - 1 ] = '\0';

      temp = findString ( nonterm , nttoken );
      ntvalue = temp -> value;

      //Getting the RHS of the Non-Terminal and populating Parse Table
      currnode = getIterator ( ruleLists[nodevalue] );
      while ( hasNext ( currnode ) )
      {
        currnode = getNext ( currnode );
        val = currnode -> value;

        if ( val[0] == 'e' )
        {
          temp = findString ( terminals , "e" );
          tvalue = temp -> value;
          parseTable [ ntvalue ] [ tvalue ] = ruleno;
          continue;  
        }
          
        if ( val[0] == 'T' )
        {
          temp = findString ( terminals , val );
          tvalue = temp -> value;
          parseTable [ ntvalue ] [ tvalue ] = ruleno;
          break;
        }

        temp = findString ( mixedbag , val );
        ntindex = temp -> value;

        //Getting the Firsts of the respective non-terminal
        firstnode = getIterator ( firsts [ntindex] );
        while ( hasNext (firstnode) )
        {
          firstnode = getNext ( firstnode );
          val = firstnode -> value;

          if ( val[0] == 'e' )
          {
            epsflag = 1;
            break;
          }
          temp = findString ( terminals , val );
          tvalue = temp -> value;

          parseTable [ ntvalue ] [ tvalue ] = ruleno;
        }

        //Adding entry for Parse Table for 'e'
        if ( epsflag == 1 )
        {
          if ( ! hasNext ( currnode ) )
          {
            temp = findString ( terminals , "e" );
            tvalue = temp -> value;
            parseTable [ ntvalue ] [ tvalue ] = ruleno;
          }

          epsflag = 0;
        }
        else
          break;
      }
    }
  }

  return 0;
}
