#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "headers/ast.h"
#include "headers/symboltable.h"

#ifndef TRIE_DEFINED
  #include "headers/trie.h"
#endif

#define BUFFERLEN 400
#define INSTRLEN 5
#define DIGSTART 48
#define DIGEND 57
#define NEWLINE '\n'
#define COMMENT_START '#'
#define PARSE_OUTPUT_FILE "PARSEOUTPUT"
#define ATTRIBUTES_FILE "TOKENMAP"
#define AST_OUTPUT_FILE "ASTOUTPUT"
#define AST_NODETYPES_FILE "config/ast_nodetypes"
#define AST_INSTRUCTIONS_FILE "config/ast_instructions"
#define T_INDEX_FILE "config/terminals_index"
#define NT_INDEX_FILE "config/nonterminals_index"

#define DEBUG_ALL 0
#define DEBUG_ONCREATE 0

#define PROPERTY_PARENT 1
#define PROPERTY_READ 2
#define PROPERTY_ADD 4

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

char nodeTypes[][30] = {

  "AST_PROGRAM_NODE",
  "AST_GLOBALDEFINES_NODE",
  "AST_GLOBALDEFINE_NODE",
  "AST_DEFINE_NODE",
  "AST_LET_NODE",
  "AST_ASSIGNABLE_NODE",
  "AST_DATATYPE_NODE",
  "AST_IDENTIFIER_NODE",
  "AST_FUNCTION_NODE",
  "AST_FUNCBODY_NODE",
  "AST_QUALIFIEDPARAMETERS_NODE",
  "AST_QUALIFIEDPARAMETER_NODE",
  "AST_RETURNTYPE_NODE",
  "AST_READ_NODE",
  "AST_PRINT_NODE",
  "AST_COMPARE_NODE",
  "AST_BOOLEXP_NODE",
  "AST_EXP_NODE",
  "AST_PASSEDPARAMS_NODE",
  "AST_RETURNSTMT_NODE",
  "AST_FUNCTIONCALL_NODE",
  "AST_IF_NODE",
  "AST_BLOCK_NODE",
  "AST_FOR_NODE",
  "AST_BDFT_NODE",
  "AST_EDGECREATE_NODE",
  "AST_BOOLOP_NODE",
  "AST_AROP_NODE",
  "AST_FORIN_NODE",
  "AST_ADJTO_NODE",
  "AST_LITERAL_NODE",
  "AST_DEPTH_NODE",
  "AST_DEST_NODE",
  "AST_SOURCE_NODE",
  "AST_WEIGHT_NODE",
  "AST_ASSIGNFUNC_NODE",
  "AST_MEMBEROP_NODE",
  "AST_ROOT_NODE",
  "AST_ENDASSIGN_NODE",
  "AST_BREAK_NODE",
  ""
};

typedef struct property_data
{
  int jumps;
  int instruction;
} PROPERTY;

int makeTrieProperty ( char *instr )
{
  if ( instr == NULL )
  {
    fprintf ( stderr, "Cannot make property from non-existent instruction\n" );
    return -1;
  }

  if ( strlen ( instr ) < 2 )
  {
    fprintf ( stderr, "Instruction should be 2 characters in length\n" );
    return -1;
  }

  int result = 0;
  if ( instr [0] == 'P' )
    result |= PROPERTY_PARENT;

  if ( instr [1] == 'R' )
    result |= PROPERTY_READ;
  else if ( instr [1] == 'A' )
    result |= PROPERTY_ADD;

  return result;
}

char* getNodeTypeName ( int type )
{
  return nodeTypes [type];
}

void extractTokenData ( char *inputtoken, char **token, char **name, int *linenumber )
{
  if ( inputtoken == NULL )
  {
    fprintf ( stderr, "Cannot extract non-existent token data\n" );
    return;
  }

  int len = strlen ( inputtoken + 1 );

  *token = malloc ( len * sizeof ( char ) );
  *name = malloc ( len * sizeof ( char ) );
  int i = 0;

  // Get token type

  while ( i < len && inputtoken [i] != ',' ) i++;

  if ( i == len )
  {
    fprintf ( stderr, "No auxiliary data seems to be present?\n" );
    return;
  }

  inputtoken [i] = '\0';
  strcpy ( *token, inputtoken + 1 );
  i++;

  int namestart = i;

  // Get token name / value

  while ( i < len && inputtoken [i] != ',' ) i++;

  if ( i == len )
  {
    fprintf ( stderr, "Insufficient amount of auxiliary data\n" );
    return;
  }

  inputtoken [i] = '\0';
  strcpy ( *name, inputtoken + namestart );
  i++;

  // Get token line number
  *linenumber = 0;

  while ( i < len && inputtoken [i] >= DIGSTART && inputtoken [i] <= DIGEND )
  {
    *linenumber = ( (*linenumber) * 10 ) + ( inputtoken [i++] - DIGSTART );
  }

}

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

void populateTrie ( FILE *mapfile, int blocksize, TRIE* trie, int *count )
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
                                blocksize, mapfile ) ) == 0 )
        break;
    }
    c = buffers [ curbuff ] [ charindx ];

    if ( charsread < blocksize && charindx >= charsread )
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( c == ' ' )
    {
      torval = 1;
      tokenindex = 0;
    }
    else if ( c == NEWLINE )
    {
      token [ tokenindex ] = '\0';
      TNODE *temp = NULL;
      temp = insertString ( trie, token );
      temp -> data.int_val = value;
      *count = value;
      value = 0;
      torval = 0;
    }
    else if ( torval == 1 )
      token [ tokenindex++ ] = c;
    else
      value = value * 10 + c - 48;
  }
}

void getNodeInstructions ( FILE *instructionsfile, int blocksize, TRIE *instructions,
                           TRIE *auxdata, TRIE *nodetypemap, TRIE *nonterminals,
                           TRIE *terminals, TRIE *properties )
{
  char c;

  char buffers [2] [ blocksize ];
  char token [ BUFFERLEN ];
  char instr [ INSTRLEN ];
  char extradata [ BUFFERLEN ];
  int num_jumps = 0;

  int charindx = -1;
  int curbuff = -1;
  int charsread = 0;
  int toriore = 0;

  int tokencounter = 0;
  int instrcounter = 0;
  int extracounter = 0;

  int isfirst = 1;
  int incomment = 0;
  int started_properties = 0;
  int prevspace = 0;

  while ( TRUE )
  {
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, instructionsfile ) ) == 0 )
        break;
    }

    c = buffers [ curbuff ] [ charindx ];

    if ( charsread < blocksize && charindx >= charsread )
    {
      fprintf ( stderr, "EOF Found\n" );
      break;
    }

    if ( isfirst == 1 && c == COMMENT_START )
      incomment = 1;

    if ( c == NEWLINE )
    {
      if ( token [0] == '@' )
      {
        started_properties = 1;
        incomment = 0;
        isfirst = 1;
        toriore = 0;
        tokencounter = 0;
        continue;
      }
      if ( incomment != 1 )
      {
        if ( started_properties == 0 )
        {
          // Parsing instructions and not properties
          // Is a valid line to parse, and not a comment
          if ( toriore == 1 )
          {
            // No auxiliary data
            instr [ instrcounter ] = '\0';

            TNODE *temp = insertString ( instructions, token );
            temp -> data . int_val = createProperty ( instr );
          }
          else
          {
            // Auxiliary data is there
            extradata [ extracounter ] = '\0';

            TNODE *temp = insertString ( instructions, token );
            temp -> data . int_val = createProperty ( instr );

            temp = insertString ( auxdata, token );

            // Is the aux data a node type, a terminal or a non terminal?
            TNODE *istype = findString ( nodetypemap, extradata );
            TNODE *isterm = findString ( terminals, extradata );
            TNODE *isntrm = findString ( nonterminals, extradata );

            if ( istype != NULL )
            {
              // Was a node type
              temp -> data . int_val = istype -> data . int_val;
            }
            else if ( isterm != NULL )
            {
              // Was a terminal
              temp -> data . int_val = isterm -> data . int_val;
            }
            else if ( isntrm != NULL )
            {
              // Was a non terminal
              temp -> data . int_val = isntrm -> data . int_val;
            }
            else
            {
              fprintf ( stderr, "Invalid auxiliary data in the instructions file\n" );
              exit (-1);
            }
          }
          if ( DEBUG_ALL ) printf ( "%s %s %d %s\n", token, instr, createProperty ( instr ), extradata );
        }
        else
        {
          extradata [ extracounter ] = '\0';
          if ( DEBUG_ALL ) printf ( "Property %s %d %s %s\n", token, num_jumps, instr, extradata );
          // Parsing properties now
          TNODE* nodeexists = findString ( properties, token );
          TRIE* level2trie = NULL;
          if ( nodeexists == NULL )
          {
            nodeexists = insertString ( properties, token );
            nodeexists -> data . generic_val = getNewTrie ( TRIE_GENERIC_TYPE );
            nodeexists -> data . generic_val = setTrieGenericSize ( (TRIE *) nodeexists -> data . generic_val, sizeof ( PROPERTY ) );
          }

          level2trie = (TRIE*) ( nodeexists -> data . generic_val );

          TNODE* propertyexists = findString ( level2trie, extradata );
          if ( propertyexists == NULL )
            propertyexists = insertString ( level2trie, extradata );

          PROPERTY temp;
          temp . jumps = num_jumps;
          temp . instruction = makeTrieProperty ( instr );

          propertyexists = setValue ( level2trie, propertyexists, & temp );
        }
      }

      incomment = 0;
      isfirst = 1;
      toriore = 0;
      tokencounter = 0;
    }
    else if ( incomment == 1 )
    {
      isfirst = 0;
      continue;
    }
    else if ( c == ' ' )
    {
      isfirst = 0;
      if ( prevspace == 1 ) continue;
      prevspace = 1;
      if ( toriore == 0 )
      {
        token [ tokencounter ] = '\0';
        instrcounter = 0;
        num_jumps = 0;
        toriore = 1;
      }
      else if ( toriore == 1 && started_properties == 0 )
      {
        instr [ instrcounter ] = '\0';
        extracounter = 0;
        toriore = 2;
      }
      else if ( toriore == 1 && started_properties == 1 )
      {
        instrcounter = 0;
        toriore = 2;
      }
      else if ( toriore == 2 && started_properties == 1 )
      {
        instr [ instrcounter ] = '\0';
        extracounter = 0;
        toriore = 3;
      }
    }
    else if ( toriore == 0 )
      token [ tokencounter++ ] = c, prevspace = 0, isfirst = 0;
    else if ( toriore == 1 && started_properties == 0 )
      instr [ instrcounter++ ] = c, prevspace = 0, isfirst = 0;
    else if ( toriore == 1 && started_properties == 1 )
    {
      num_jumps = num_jumps * 10 + c - 48;
      prevspace = 0, isfirst = 0;
    }
    else if ( toriore == 2 && started_properties == 0 )
      extradata [ extracounter++ ] = c, prevspace = 0, isfirst = 0;
    else if ( toriore == 2 && started_properties == 1 )
      instr [ instrcounter++ ] = c, prevspace = 0, isfirst = 0;
    else if ( toriore == 3 )
      extradata [ extracounter++ ] = c, prevspace = 0, isfirst = 0;
  }

}

AST* createAST ( FILE * parseroutput, int blocksize, AST *ast, TRIE *instructions,
                 TRIE *auxdata, TRIE *nonterminals, TRIE *terminals, TRIE *properties,
                 SYMBOLTABLE *symboltable, FILE *astoutput )
{
  // We start processing from the root node
  ANODE *currnode = ast -> root;

  STACK *stack = NULL;
  stack = getStack ( STACK_STRING_TYPE );

  // Open a global environment in the symbol table
  symboltable = openEnv ( symboltable );

  char c;

  char buffers [2] [blocksize];
  char token [ BUFFERLEN ];

  int charindx = -1;
  int curbuff = -1;
  int charsread = 0;
  int tokencounter = 0;

  int conditional_read = 0;
  int conditional_pop = 0;
  int conditional_value = -1;

  TNODE *beginnode = findString ( terminals, TK_BEGIN );
  TNODE *endnode = findString ( terminals, TK_END );
  TNODE *idennode = findString ( terminals, TK_IDEN );
  TNODE *intlitnode = findString ( terminals, TK_INTLIT );
  TNODE *stringlitnode = findString ( terminals, TK_STRINGLIT );
  TNODE *floatlitnode = findString ( terminals, TK_FLOATLIT );
  TNODE *intnode = findString ( terminals, TK_INT );
  TNODE *stringnode = findString ( terminals, TK_STRING );
  TNODE *floatnode = findString ( terminals, TK_FLOAT );
  TNODE *vertexnode = findString ( terminals, TK_VERTEX );
  TNODE *edgenode = findString ( terminals, TK_EDGE );
  TNODE *graphnode = findString ( terminals, TK_GRAPH );
  TNODE *treenode = findString ( terminals, TK_TREE );
  TNODE *nothingnode = findString ( terminals, TK_NOTHING );
  TNODE *andnode = findString ( terminals, TK_AND );
  TNODE *ornode = findString ( terminals, TK_OR );
  TNODE *notnode = findString ( terminals, TK_NOT );
  TNODE *plusnode = findString ( terminals, TK_PLUS );
  TNODE *minusnode = findString ( terminals, TK_MINUS );
  TNODE *mulnode = findString ( terminals, TK_MUL );
  TNODE *divnode = findString ( terminals, TK_DIV );
  TNODE *modulonode = findString ( terminals, TK_MODULO );
  TNODE *gtnode = findString ( terminals, TK_GT );
  TNODE *ltnode = findString ( terminals, TK_LT );
  TNODE *gtenode = findString ( terminals, TK_GTE );
  TNODE *ltenode = findString ( terminals, TK_LTE );
  TNODE *eqnode = findString ( terminals, TK_EQ );
  TNODE *bftnode = findString ( terminals, TK_BFT );
  TNODE *dftnode = findString ( terminals, TK_DFT );

  if ( beginnode == NULL
       || endnode == NULL
       || idennode == NULL
       || intlitnode == NULL
       || stringlitnode == NULL
       || floatlitnode == NULL
       || intnode == NULL
       || stringnode == NULL
       || floatnode == NULL
       || vertexnode == NULL
       || edgenode == NULL
       || graphnode == NULL
       || treenode == NULL
       || nothingnode == NULL
       || andnode == NULL
       || ornode == NULL
       || notnode == NULL
       || plusnode == NULL
       || minusnode == NULL
       || mulnode == NULL
       || divnode == NULL
       || modulonode == NULL
       || gtnode == NULL
       || ltnode == NULL
       || gtenode == NULL
       || ltenode == NULL
       || eqnode == NULL
       || bftnode == NULL
       || dftnode == NULL ) fprintf ( stderr, "Failed to find required terminal for semantic analysis\n" ), exit (0);

  int beginint = beginnode -> data . int_val;
  int endint = endnode -> data . int_val;
  int idenint = idennode -> data . int_val;
  int intlitint = intlitnode -> data . int_val;
  int stringlitint = stringlitnode -> data . int_val;
  int floatlitint = floatlitnode -> data . int_val;
  int intint = intnode -> data . int_val;
  int stringint = stringnode -> data . int_val;
  int floatint = floatnode -> data . int_val;
  int vertexint = vertexnode -> data . int_val;
  int edgeint = edgenode -> data . int_val;
  int graphint = graphnode -> data . int_val;
  int treeint = treenode -> data . int_val;
  int nothingint = nothingnode -> data . int_val;
  int andint = andnode -> data . int_val;
  int orint = ornode -> data . int_val;
  int notint = notnode -> data . int_val;
  int plusint = plusnode -> data . int_val;
  int minusint = minusnode -> data . int_val;
  int mulint = mulnode -> data . int_val;
  int divint = divnode -> data . int_val;
  int moduloint = modulonode -> data . int_val;
  int gtint = gtnode -> data . int_val;
  int ltint = ltnode -> data . int_val;
  int gteint = gtenode -> data . int_val;
  int lteint = ltenode -> data . int_val;
  int eqint = eqnode -> data . int_val;
  int bftint = bftnode -> data . int_val;
  int dftint = dftnode -> data . int_val;

  while ( TRUE )
  {
    charindx = ( charindx + 1 ) % blocksize;
    if ( charindx == 0 )
    {
      curbuff = ( curbuff + 1 ) & 1;
      if ( ( charsread = fread ( buffers [ curbuff ], sizeof ( char ), blocksize, parseroutput ) ) == 0 )
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
      token [ tokencounter ] = '\0';

      // A whole line of expansion has been read. Need to process
      stack = pushReverseSpaceSeparatedWords ( stack, token );

      while ( ! isEmpty ( stack ) )
      {
        // Examine the next value on top of the stack, and perform the corresponding operation
        char *topvalue = strdup ( ( char * ) top ( stack ) );
        stack = pop ( stack );

        char *temptoken = NULL, *tokenname = NULL;
        int linenumber = -1;

        // TODO: (unimportant) -- Anyway to make this jugaad cleaner?
        if ( topvalue [0] == '<' && topvalue [1] == 'T' )
        {
          extractTokenData ( topvalue, &temptoken, &tokenname, &linenumber );
          free ( topvalue );
          topvalue = temptoken;
        }


        TNODE *istopterminal = findString ( terminals, topvalue );
        if ( istopterminal != NULL )
        {
          if ( istopterminal -> data . int_val == beginint )
            symboltable = openEnv ( symboltable );
          else if ( istopterminal -> data . int_val == endint )
            symboltable = closeEnv ( symboltable );
        }


        // TODO: If 'topvalue' is a variable/literal, add entry of appropriate type in
        //       the symbol table
        // TODO: If 'topvalue' is == TK_PLUS / TK_MINUS / TK_MUL / TK_DIV / TK_MODULO,
        //       assign the appropriate type to the AST_AROP node that will be created
        //       in the creation condition below.
        // TODO: If 'topvalue' is == TK_AND / TK_OR / TK_NOT then assign the appropriate
        //       type to the AST_BOOLOP node that will be created in the creation cond.
        //       below
        // TODO: If 'topvalue' is DFT / BFT then assign the appropriate type to the BDFT
        //       node that will be created below
        // TODO: If 'topvalue' is == TK_INT/FLOAT/STRING/VERTEX/EDGE/GRAPH/TREE assign
        //       the appropriate type to the datatype node, which **should** and will be
        //       the currnode
        // TODO: If 'topvalue' is TK_LT/GT/LTE/GTE/EQ then assign the appropriate type
        //       to the compare node, which **should** and will be the currnode
        //
        // where 'topvalue' = the string that was popped from the stack
        //       'token' = the next line that was read from the input
        //       'tokenname' = name of the variable / value of the literal
        //       linenumber = the linenumber for variables / literals


        // The current node properties override all of the other conditions so it must be tested
        // first.
        TNODE *hasproperties = findString ( properties, getNodeTypeName ( currnode -> node_type ) );
        if ( hasproperties != NULL )
        {
          TRIE* trieToSearch = (TRIE *) hasproperties -> data . generic_val;
          TNODE* jumps = findString ( trieToSearch, topvalue );

          if ( jumps != NULL )
          {
            int numberOfJumps = ( (PROPERTY *) jumps -> data . generic_val ) -> jumps;
            int shouldRead = ( ( ( ( (PROPERTY *) jumps -> data . generic_val ) -> instruction ) & PROPERTY_READ ) == PROPERTY_READ );
            int shouldAdd = ( ( ( ( (PROPERTY *) jumps -> data . generic_val ) -> instruction ) & PROPERTY_ADD ) == PROPERTY_ADD );
            if ( DEBUG_ALL ) printf ( "At node %s got %s so jumping %d times\n", getNodeTypeName ( currnode -> node_type ),
                                                                topvalue, numberOfJumps );
            while ( numberOfJumps > 0 )
            {
              if ( getParent ( currnode ) == NULL || getParent ( currnode ) == ast -> root )
                break;
              currnode = getParent ( currnode );
              numberOfJumps --;
            }

            if ( shouldRead == 1 )
            {
              free ( topvalue );
              break;
            }
            else if ( shouldAdd == 1 )
            {
              stack = push ( stack, topvalue );
              if ( DEBUG_ALL ) printf ( "Pushing back %s\n", topvalue );
              free ( topvalue );
              continue;
            }
          }
        }


        if ( conditional_read == 1 || conditional_pop == 1 )
        {
          // If the previous value was a conditional read we may need to go to currnode's parent
          conditional_read = 0;
          conditional_pop = 0;

          // The conditional read may be on a terminal or on a non-terminal. Check both.
          // Simplifying assumption - the IDs for the non-terminals and terminals are unique.
          // TODO: Store extra data in the auxiliary Trie to differentiate
          // between terminals and non-terminals.
          TNODE *termval = findString ( terminals, topvalue );
          TNODE *nontermval = findString ( nonterminals, topvalue );

          if ( termval != NULL && termval -> data . int_val == conditional_value )
            currnode = getParent ( currnode );
          else if ( nontermval != NULL && nontermval -> data . int_val == conditional_value )
            currnode = getParent ( currnode );
          else
            if ( DEBUG_ALL ) printf ( "Conditional read unsuccessful\n" );

          // Don't break as the topvalue should be processed even on conditional read
          // but do reset the conditional_value
          conditional_value = -1;
        }

        TNODE *currentval = findString ( instructions, topvalue );

        // If there are no instructions for the current value, ignore it if terminal
        // Read on non-terminals.
        if ( currentval == NULL )
        {
          if ( DEBUG_ALL ) printf ( "*No rule for %s*\n", topvalue );
          TNODE *isnonterm = findString ( nonterminals, topvalue );
          free ( topvalue );

          // Is a non-terminal...
          if ( isnonterm != NULL )
            break;

          continue;
        }

        int instruction = currentval -> data . int_val;

        if ( (instruction & CREATE) == CREATE )
        {
          // If we have to create a node, examine the type of node to be created
          TNODE *typeofnode = findString ( auxdata, topvalue );

          if ( typeofnode == NULL )
          {
            fprintf ( stderr, "Node instruction is create, but type to create not found\n" );
            return NULL;
          }

          int type_to_create = typeofnode -> data . int_val;

          if ( DEBUG_ALL || DEBUG_ONCREATE ) printf ( "At node %s\n", getNodeTypeName ( currnode -> node_type ) );

          fprintf ( astoutput, "At node %s\n", getNodeTypeName ( currnode -> node_type ) );

          currnode = addChild ( currnode, type_to_create, instruction );

          if ( DEBUG_ALL ) printf ( "Got %s so ", topvalue );

          if ( DEBUG_ALL || DEBUG_ONCREATE ) printf ( "Creating: %s\n\n", getNodeTypeName ( type_to_create ) );

          fprintf ( astoutput, "Creating: %s\n\n", getNodeTypeName ( type_to_create ) );

          if ( DEBUG_ALL ) printf ( "\n\n" );

          // currnode is now either the same node, child, or parent depending on
          // the instruction. If the instruction says read, then we break.
          if ( (instruction & READ) == READ )
          {
            free ( topvalue );
            break;
          }
        }
        else if ( (instruction & GOTOCH) == GOTOCH )
        {
          fprintf ( stderr, "Instruction says goto child, but no child was created. So which child?\n" );
          return NULL;
        }
        else if ( (instruction & PARENT) == PARENT )
        {
          if ( DEBUG_ALL ) printf ( "At node %s\n", getNodeTypeName ( currnode -> node_type ) );
          if ( DEBUG_ALL ) printf ( "Got %s so ", topvalue );
          if ( DEBUG_ALL ) printf ( "Going to parent\n" );

          if ( getParent ( currnode ) == NULL || getParent ( currnode ) == ast -> root )
            fprintf ( stderr, "Parent is null, so remaining at same node\n" );
          else
            currnode = getParent ( currnode );

          // If the instruction says read, we should break
          if ( (instruction & READ) == READ )
          {
            free ( topvalue );
            break;
          }
        }
        else if ( (instruction & CONDRD) == CONDRD )
        {
          if ( DEBUG_ALL ) printf ( "At node %s\n", getNodeTypeName ( currnode -> node_type ) );
          if ( DEBUG_ALL ) printf ( "Got %s so ", topvalue );
          if ( DEBUG_ALL ) printf ( "conditionally reading/popping next value %d\n", instruction );
          // Instruction is a conditional read
          // Set the conditional read flag and value so that
          // currnode will be changed to parent on that value

          if ( (instruction & READ) == READ )
            conditional_read = 1;
          else
            conditional_pop = 1;

          TNODE *cond_value = findString ( auxdata, topvalue );
          if ( cond_value == NULL )
          {
            fprintf ( stderr, "Conditional read & goto parent has no auxiliary data\n" );
            return NULL;
          }

          conditional_value = cond_value -> data . int_val;

          if ( (instruction & READ) == READ )
          {
            // Break so a read will occur
            free ( topvalue );
            break;
          }
          if ( DEBUG_ALL ) printf ( "Not breaking\n" );
        }
        else if ( (instruction & READ) == READ )
        {
          if ( DEBUG_ALL ) printf ( "At node %s\n", getNodeTypeName ( currnode -> node_type ) );
          if ( DEBUG_ALL ) printf ( "Got %s so ", topvalue );
          if ( DEBUG_ALL ) printf ( "Reading next input...\n" );
          free ( topvalue );
          break;
        }
      }   // End of while

      tokencounter = 0;
    }
    else
      token [ tokencounter++ ] = c;
  }

  return ast;
}

int main ( )
{
  // Get the system block size
  struct stat fi;
  stat ( "/", &fi );
  int blocksize = fi.st_blksize;



  /*********************************************************
    *                                                      *
    *              PHASE 1 : Populate Tries                *
    *                                                      *
    ********************************************************
   */

  AST* ast = NULL;
  ast = getNewAst ();

  TRIE *terminals = NULL, *nonterminals = NULL;
  int terminalscount = 0, nonterminalscount = 0;
  int nodetypescount = 0;

  terminals = getNewTrie ( TRIE_INT_TYPE );
  nonterminals = getNewTrie ( TRIE_INT_TYPE );

  FILE *tmapfile = NULL, *ntmapfile = NULL;
  FILE *nodetypesfile = NULL;

  tmapfile = fopen ( T_INDEX_FILE, "rb" );
  ntmapfile = fopen ( NT_INDEX_FILE, "rb" );
  nodetypesfile = fopen ( AST_NODETYPES_FILE, "rb" );

  if ( tmapfile == NULL || ntmapfile == NULL || nodetypesfile == NULL )
  {
    fprintf ( stderr, "Failed to open (non) terminals index file / AST node types file\n" );
    return -1;
  }

  populateTrie ( tmapfile, blocksize, terminals, &terminalscount );
  populateTrie ( ntmapfile, blocksize, nonterminals, &nonterminalscount );
  populateTrie ( nodetypesfile, blocksize, ast -> node_typemap, &nodetypescount );

  if ( fclose ( tmapfile ) != 0 )
    fprintf ( stderr, "Failed to close terminals index file\n" );
  if ( fclose ( ntmapfile ) != 0 )
    fprintf ( stderr, "Failed to close non terminals index file\n" );
  if ( fclose ( nodetypesfile ) != 0 )
    fprintf ( stderr, "Failed to close AST node types file\n" );



  /*********************************************************
    *                                                      *
    *        PHASE 2 : Build Rule Tries and Mapping        *
    *                                                      *
    ********************************************************
   */

  TRIE *instructions = NULL, *auxdata = NULL;
  // This Trie of Tries stores the nodes with special properties
  // if findString ( properties, string ) exists then string has
  // a special property
  // if findString ( (TRIE*) findString ( properties, string ) -> data . generic_val, string2 )
  // exists, then then string node has a property for string2..
  TRIE *properties = NULL;

  instructions = getNewTrie ( TRIE_INT_TYPE );
  auxdata = getNewTrie ( TRIE_INT_TYPE );
  properties = getNewTrie ( TRIE_GENERIC_TYPE );
  properties = setTrieGenericSize ( properties, sizeof ( TRIE* ) );

  FILE *instructionsfile = NULL;

  instructionsfile = fopen ( AST_INSTRUCTIONS_FILE, "rb" );

  if ( instructionsfile == NULL )
  {
    fprintf ( stderr, "Failed to open AST nodes' instructions file\n" );
    return -1;
  }

  getNodeInstructions ( instructionsfile, blocksize, instructions, auxdata, ast -> node_typemap,
                        nonterminals, terminals, properties );



  /*********************************************************
    *                                                      *
    *    PHASE 3 : Build Final AST and use Symbol Table    *
    *                                                      *
    ********************************************************
   */

  FILE *attributesFile = NULL;
  attributesFile = fopen ( ATTRIBUTES_FILE, "rb" );

  if ( attributesFile == NULL )
  {
    fprintf ( stderr, "Failed to open attributes map file\n" );
    return -1;
  }

  int stbentries = getLineCount ( attributesFile, blocksize );

  SYMBOLTABLE *symboltable = getSymbolTable();

  symboltable = setNumEntries ( symboltable, stbentries );

  FILE *parseroutput = NULL;
  parseroutput = fopen ( PARSE_OUTPUT_FILE, "rb" );

  if ( parseroutput == NULL )
  {
    fprintf ( stderr, "Failed to open parser output file\n" );
    return -1;
  }

  FILE *astoutput = NULL;
  astoutput = fopen ( AST_OUTPUT_FILE, "w+" );

  if ( astoutput == NULL )
  {
    fprintf ( stderr, "Failed to open AST output file\n" );
    return -1;
  }

  ast = createAST ( parseroutput, blocksize, ast, instructions, auxdata,
                    nonterminals, terminals, properties, symboltable,
                    astoutput );

  if ( fclose ( parseroutput ) != 0 )
  {
    fprintf ( stderr, "Failed to close parser output file\n" );
    return -1;
  }

  if ( fclose ( astoutput ) != 0 )
  {
    fprintf ( stderr, "Failed to close AST output file\n" );
    return -1;
  }

  if ( DEBUG_ALL ) printf ( "AST successfully built\n" );

  return 0;
}

