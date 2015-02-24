#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <headers/trie.h>
#include <headers/linkedlist.h>

#define BUFFERLEN 200
#define CHARSIZE 8
#define NEWLINE '\n'
#define EOFCHAR 4
int main( int argc, char *argv[] )
{
	char c;
	
	int curbuff = -1;
	int charindx = -1;
	int lines = 0;
	int totalblocks = 0;
	int charsread = 0;

	FILE *inputfile;
	FILE *grammarfile;
	inputfile = fopen ( argv[1] , "r" );

	// Get the system block size
  struct stat fi;
  stat ( "/", &fi );
  int blocksize = fi.st_blksize;
  char buffers [2] [blocksize];
  
  while ( 1 )
  {
    // Get char from appropriate buffer
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], CHARSIZE, blocksize / CHARSIZE, inputfile ) ) == 0 )
        break;
      totalblocks++;
    }
    c = buffers [ curbuff ] [ charindx ];

    if ( c == EOF )
    {
      printf ( "EOF Found\n" );
      break;
    }
//		printf("%c",c);
    if ( c == NEWLINE )
      lines++;
  }
 	
 	
	LINKEDLIST* ll[lines] ;
 	char buff[ blocksize ] ;
	char s[2] = "\n";
	char *token , *nttoken , *ttoken;
 	char *rhs , *finalterms;
 	
 	int ruleno = -1;
 	int a = 1;
 	
 	grammarfile = fopen(argv[1],"r");
 	TRIE *gramrules , *nonterm , *terminals;
 	TNODE *temp;
 	
 	gramrules = getNewTrie();
 	gramrules = setTrieName( gramrules , "Grammar Rules" );
 	nonterm = getNewTrie();
 	nonterm = setTrieName( nonterm , "Non Terminals");
 	terminals = getNewTrie();
 	terminals = setTrieName( terminals , "Terminals" );
 	
 	while ( totalblocks > 1 )
  {
  	
    fread ( buff, CHARSIZE, blocksize / CHARSIZE, grammarfile );
    token = strtok( buff , s );
    
    do
    {
    	//Pushing a Rule to the Trie
    	ruleno++;
    	rhs = strdup(token);
    	temp = insertString ( gramrules , token );
    	temp = setValue ( temp , ruleno );
    	
    	//Inserting into Array of Linked lIst and updating Non-Terminals Trie
    	rhs = strdup(token);
    	nttoken = strtok(rhs , "-");
    	
    	temp = insertString ( nonterm , nttoken );
    	temp = setValue ( temp , ruleno );
    	nttoken = strtok(NULL, "-");
    	
    	ll[ruleno] = getLinkedList();
    	ll[ruleno] = insertSpaceSeparatedWords(ll[ruleno] , nttoken + 1);
    	
    	
    	// Constructing Terminals Trie
    	finalterms = strdup(nttoken + 1);
    	ttoken = strtok(finalterms , " ");
    	
    	while ( ttoken != NULL )
    	{
    		if ( ttoken[0] == 'T' )
    		{	
    			temp = insertString ( terminals , ttoken );
    			temp = setValue ( temp , ruleno );
    			printf("%s\n",ttoken);
    		}
    		ttoken = strtok(NULL , " ");
    	}
    	printf("%s\n",token);
    	token = strtok(buff + strlen(token) + 1, s);
//    	printf("%s\n",token);    	
   	}
   	while( token != NULL );
    totalblocks--;
  }
 	
	return 0;
}

