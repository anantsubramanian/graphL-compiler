// Authors: Anant Subramanian <anant.subramanian15@gmail.com>
//          Aditya Bansal <adityabansal_adi@yahoo.co.in>
//
// BITS PILANI ID NOs: 2012A7TS010P
//                     2012A7PS122P
//
// Project Team Num: 1
// Project Group No. 1

#include "ast_construction.h"

static int construction_error_occurred = 0;

/**
 * Given an AST instruction string (refer to config folder for
 * format), construct and return the corresponding integer encoding
 *
 * @param instr char* the character instruction
 * 
 * @return the encoded instruction
 */

int makeTrieProperty ( char *instr )
{
  if ( instr == NULL )
  {
    fprintf ( stderr, "Cannot make property from non-existent instruction\n" );
    construction_error_occurred = 1;
    return -1;
  }

  if ( strlen ( instr ) < 2 )
  {
    fprintf ( stderr, "Instruction should be 2 characters in length\n" );
    construction_error_occurred = 1;
    return -1;
  }

  int result = 0;
  if ( instr [0] == 'P' )
    result |= PROPERTY_PARENT;
  else if ( instr [0] == 'C' )
    result |= PROPERTY_CREATE;

  if ( instr [1] == 'R' )
    result |= PROPERTY_READ;
  else if ( instr [1] == 'A' )
    result |= PROPERTY_ADD;

  return result;
}

/**
 * Extract the line number from the parser output of a given line
 *
 * @param input char* A line of parser output
 * 
 * @return int the line number extracted from it
 */

int extractLineNum ( char *input )
{
  int n = strlen ( input );
  char *p = input + n - 1;

  while ( *p != ' ' ) p--;
  p++;

  int lno = atoi ( p );
  *p = '\0';

  return lno;
}

/**
 * Extract all auxiliary data associated with an identifier or a
 * literal and store them in the corresponding variables
 *
 * @param inputtoken char* The token to extract data from
 * @param token char** The output token name
 * @param name char** The identifier name/literal value
 * @param linenumber int* The line number the token was found on
 */

void extractTokenData ( char *inputtoken, char **token, char **name, int *linenumber )
{
  if ( inputtoken == NULL )
  {
    fprintf ( stderr, "Cannot extract non-existent token data\n" );
    construction_error_occurred = 1;
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
    construction_error_occurred = 1;
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
    construction_error_occurred = 1;
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

/**
 * Function that reads an instructions file (format in the config folder)
 * and populates all Tries necessary for AST construction
 *
 * @param instructionsfile FILE* The file containing the construction instrs.
 * @param blocksize int The block size to use while reading the file
 * @param instructions TRIE* The basic instructions Trie
 * @param auxdata TRIE* Auxiliary data associated with instructions
 * @param nodetypemap TRIE* Maps node names onto integers
 * @param nonterminals TRIE* Maps non terminal names onto integers
 * @param terminals TRIE* Maps terminals onto ints
 * @param properties TRIE* Stores specific properties for nodes that override instrs
 */

void getNodeInstructions ( FILE *instructionsfile, int blocksize, TRIE *instructions,
                           TRIE *auxdata, TRIE *nodetypemap, TRIE *nonterminals,
                           TRIE *terminals, TRIE *properties )
{
  char c;

  char buffers [2] [ blocksize ];
  char token [ BUFFERLEN ];
  char instr [ INSTRLEN ];
  char extradata [ BUFFERLEN ];
  char nodetype [ BUFFERLEN ];
  int num_jumps = 0;

  int charindx = -1;
  int curbuff = -1;
  int charsread = 0;
  int toriore = 0;

  int tokencounter = 0;
  int instrcounter = 0;
  int extracounter = 0;
  int nodetcounter = 0;

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
      break;

    if ( isfirst == 1 && c == COMMENT_START )
      incomment = 1;

    if ( c == NEWLINE )
    {
      if ( incomment != 1 )
      {
        // If the token is the special properties start marker
        if ( token [0] == PROPERTIES_START )
        {
          token [ tokencounter ] = '\0';
          if ( DEBUG_ALL ) printf ( "Started properties on %s\n", token );
          started_properties = 1;
          incomment = 0;
          isfirst = 1;
          toriore = 0;
          tokencounter = 0;
          continue;
        }

        // Parsing instructions and not properties
        if ( started_properties == 0 )
        {
          // Is a valid line to parse, and not a comment
          if ( toriore == 1 )
          {
            // No auxiliary data
            instr [ instrcounter ] = '\0';

            TNODE *temp = insertString ( instructions, token );
            temp -> data . int_val = createProperty ( instr );

            if ( DEBUG_ALL )
              printf ( "Instruction %s %s %d\n", token, instr, createProperty ( instr ) );
          }
          else
          {
            // Auxiliary data is there
            extradata [ extracounter ] = '\0';
            nodetype [ nodetcounter ] = '\0';

            TNODE *temp = insertString ( instructions, token );
            temp -> data . int_val = createProperty ( instr );

            temp = insertString ( auxdata, token );

            // Is the aux data a node type, a terminal or a non terminal?
            TNODE *istype = findString ( nodetypemap, extradata );
            TNODE *isterm = findString ( terminals, extradata );
            TNODE *isntrm = findString ( nonterminals, extradata );

            AUXDATA tempdata;

            if ( istype != NULL )
            {
              // Was a node type
              tempdata . conditional_value = istype -> data . int_val;
              tempdata . conditional_type = CONDITIONAL_NODETYPE;
              setValue ( auxdata, temp, & tempdata );
            }
            else if ( isterm != NULL )
            {
              // Was a terminal
              tempdata . conditional_value = isterm -> data . int_val;
              tempdata . conditional_type = CONDITIONAL_TERMINAL;
              setValue ( auxdata, temp, & tempdata );
            }
            else if ( isntrm != NULL )
            {
              // Was a non terminal
              tempdata . conditional_value = isntrm -> data . int_val;
              tempdata . conditional_type = CONDITIONAL_NONTERMINAL;
              setValue ( auxdata, temp, & tempdata );
            }
            else
            {
              fprintf ( stderr, "Invalid auxiliary data in the instructions file\n" );
              construction_error_occurred = 1;
              exit (-1);
            }
            if ( DEBUG_ALL )
              printf ( "Instruction %s %s %d %s\n", token, instr, createProperty ( instr ), extradata );
          }
        }
        else
        {
          extradata [ extracounter ] = '\0';
          nodetype [ nodetcounter ] = '\0';
          if ( DEBUG_ALL )
            printf ( "Property %s %d %s %s %s\n", token, num_jumps, instr, extradata, nodetype );
          // Parsing properties now
          TNODE* nodeexists = findString ( properties, token );
          TRIE* level2trie = NULL;
          if ( nodeexists == NULL )
          {
            nodeexists = insertString ( properties, token );
            nodeexists -> data . generic_val = getNewTrie ( TRIE_GENERIC_TYPE );
            nodeexists -> data . generic_val =
              setTrieGenericSize ( (TRIE *) nodeexists -> data . generic_val, sizeof ( PROPERTY ) );
          }

          level2trie = (TRIE*) ( nodeexists -> data . generic_val );

          TNODE* propertyexists = findString ( level2trie, extradata );
          if ( propertyexists == NULL )
            propertyexists = insertString ( level2trie, extradata );

          PROPERTY temp;
          temp . jumps = num_jumps;
          temp . instruction = makeTrieProperty ( instr );
          if ( ( temp . instruction & PROPERTY_CREATE ) == PROPERTY_CREATE )
          {
            TNODE *findnodetype = findString ( nodetypemap, nodetype );
            if ( findnodetype == NULL )
            {
              fprintf ( stderr, "Invalid node type for create property in the instructions file\n" );
              construction_error_occurred = 1;
              exit (-1);
            }

            temp . node_type = findnodetype -> data . int_val;
          }

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
      else if ( toriore == 3 && started_properties == 1 )
      {
        extradata [ extracounter ] = '\0';
        nodetcounter = 0;
        toriore = 4;
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
    else if ( toriore == 4 )
      nodetype [ nodetcounter++ ] = c, prevspace = 0, isfirst = 0;
  }

}

/**
 * Convert token names to integers
 *
 * @paramlist ints The integers to be populated
 * @paramlast terminals TRIE* The Trie to use to retrieve the ints
 */

void getNodeIntegers (
    int *beginint, int *endint, int *idenint, int *intlitint,
    int *stringlitint, int *floatlitint, int *intint,
    int *stringint, int *floatint, int *vertexint, int *edgeint,
    int *graphint, int *treeint, int *nothingint, int *andint,
    int *orint, int *notint, int *plusint, int *minusint,
    int *mulint, int *divint, int *moduloint, int *gtint,
    int *ltint, int *gteint, int *lteint, int *eqint,
    int *bftint, int *dftint, int *functionint, TRIE *terminals )
{

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
  TNODE *functionnode = findString ( terminals, TK_FUNCTION );

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
       || functionnode == NULL
       || dftnode == NULL )
  {
    fprintf ( stderr, "Failed to find required terminal for semantic analysis\n" );
    construction_error_occurred = 1;
    exit (-1);
  }

  *beginint = beginnode -> data . int_val;
  *endint = endnode -> data . int_val;
  *idenint = idennode -> data . int_val;
  *intlitint = intlitnode -> data . int_val;
  *stringlitint = stringlitnode -> data . int_val;
  *floatlitint = floatlitnode -> data . int_val;
  *intint = intnode -> data . int_val;
  *stringint = stringnode -> data . int_val;
  *floatint = floatnode -> data . int_val;
  *vertexint = vertexnode -> data . int_val;
  *edgeint = edgenode -> data . int_val;
  *graphint = graphnode -> data . int_val;
  *treeint = treenode -> data . int_val;
  *nothingint = nothingnode -> data . int_val;
  *andint = andnode -> data . int_val;
  *orint = ornode -> data . int_val;
  *notint = notnode -> data . int_val;
  *plusint = plusnode -> data . int_val;
  *minusint = minusnode -> data . int_val;
  *mulint = mulnode -> data . int_val;
  *divint = divnode -> data . int_val;
  *moduloint = modulonode -> data . int_val;
  *gtint = gtnode -> data . int_val;
  *ltint = ltnode -> data . int_val;
  *gteint = gtenode -> data . int_val;
  *lteint = ltenode -> data . int_val;
  *eqint = eqnode -> data . int_val;
  *bftint = bftnode -> data . int_val;
  *dftint = dftnode -> data . int_val;
  *functionint = functionnode -> data . int_val;
}

/**
 * Handles auxiliary terminal operations for AST nodes
 * that do not represent identifiers
 * For parameter information, refer to the general
 * handleAuxiliaryOpsForTerminals function.
 *
 */

void handleAuxiliaryOpsForNonIdentifiers (
    int beginint, int endint, int intint,
    int stringint, int floatint, int vertexint, int edgeint,
    int graphint, int treeint, int nothingint, int andint,
    int orint, int notint, int plusint, int minusint,
    int mulint, int divint, int moduloint, int gtint,
    int ltint, int gteint, int lteint, int eqint,
    int bftint, int dftint, int functionint, int terminalvalue,
    ANODE *currnode, SYMBOLTABLE *symboltable, int *should_start_function,
    int *function_scope_started )
{
  if ( terminalvalue == beginint )
  {
    if ( *function_scope_started == 0 )
    {
      // If it is TK_BEGIN
      symboltable = openEnv ( symboltable );
      if ( DEBUG_AUXOPS || DEBUG_ALL )
        printf ( "Opening environment\n\n" );
    }
    else
      *function_scope_started = 0;
  }
  else if ( terminalvalue == endint )
  {
    // If it is TK_END
    symboltable = closeEnv ( symboltable );
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Closing environment\n\n" );
  }
  else if ( terminalvalue == ltint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning LT type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . compop_type = C_LT_TYPE;
  }
  else if ( terminalvalue == lteint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning LTE type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . compop_type = C_LTE_TYPE;
  }
  else if ( terminalvalue == gtint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning GT type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . compop_type = C_GT_TYPE;
  }
  else if ( terminalvalue == gteint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning GTE type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . compop_type = C_GTE_TYPE;
  }
  else if ( terminalvalue == eqint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning EQ type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . compop_type = C_EQ_TYPE;
  }
  else if ( terminalvalue == andint )
  {
    if ( currnode -> node_type == AST_BOOLEXP_NODE || currnode -> node_type == AST_BOOLOP_NODE )
    {
      if ( DEBUG_AUXOPS || DEBUG_ALL )
        printf ( "Assigning AND type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
      currnode -> extra_data . boolop_type = B_AND_TYPE;
    }
    else if ( DEBUG_ALL )
      printf ( "Got AND at non-boolop/boolexp node so ignoring...\n" );
  }
  else if ( terminalvalue == orint )
  {
    if ( currnode -> node_type == AST_BOOLEXP_NODE || currnode -> node_type == AST_BOOLOP_NODE )
    {
      if ( DEBUG_AUXOPS || DEBUG_ALL )
        printf ( "Assigning OR type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
      currnode -> extra_data . boolop_type = B_OR_TYPE;
    }
    else if ( DEBUG_ALL )
      printf ( "Got OR at non-boolop/boolexp node so ignoring...\n" );
  }
  else if ( terminalvalue == notint )
  {
    if ( currnode -> node_type == AST_BOOLEXP_NODE || currnode -> node_type == AST_BOOLOP_NODE )
    {
      if ( DEBUG_AUXOPS || DEBUG_ALL )
        printf ( "Assigning NOT type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
      currnode -> extra_data . boolop_type = B_NOT_TYPE;
    }
    else if ( DEBUG_ALL )
      printf ( "Got NOT at non-boolop/boolexp node so ignoring...\n" );
  }
  else if ( terminalvalue == bftint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning BFT type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . bdft_type = BDFT_BFT_TYPE;
  }
  else if ( terminalvalue == dftint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning DFT type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . bdft_type = BDFT_DFT_TYPE;
  }
  else if ( terminalvalue == plusint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning PLUS type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . arop_type = A_PLUS_TYPE;
  }
  else if ( terminalvalue == minusint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning MINUS type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . arop_type = A_MINUS_TYPE;
  }
  else if ( terminalvalue == mulint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning MUL type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . arop_type = A_MUL_TYPE;
  }
  else if ( terminalvalue == divint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning DIV type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . arop_type = A_DIV_TYPE;
  }
  else if ( terminalvalue == moduloint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning MODULO type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . arop_type = A_MODULO_TYPE;
  }
  else if ( terminalvalue == intint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning INT type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_INT_TYPE;
  }
  else if ( terminalvalue == floatint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning FLOAT type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_FLOAT_TYPE;
  }
  else if ( terminalvalue == stringint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning STRING type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_STRING_TYPE;
  }
  else if ( terminalvalue == vertexint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning VERTEX type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_VERTEX_TYPE;
  }
  else if ( terminalvalue == edgeint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning EDGE type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_EDGE_TYPE;
  }
  else if ( terminalvalue == graphint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning GRAPH type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_GRAPH_TYPE;
  }
  else if ( terminalvalue == treeint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning TREE type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_TREE_TYPE;
  }
  else if ( terminalvalue == nothingint )
  {
    if ( DEBUG_AUXOPS || DEBUG_ALL )
      printf ( "Assigning NOTHING type to node %s\n\n", getNodeTypeName ( currnode -> node_type ) );
    currnode -> extra_data . data_type = D_NOTHING_TYPE;
  }
  else if ( terminalvalue == functionint )
  {
    // If function keyword has been seen, then start the function scope after the next identifier
    // and ignore the next begin
    *should_start_function = 1;
  }
}

/**
 * Perform auxiliary STB and ANODE operations for nodes that
 * represent terminals
 *
 * @paramlist ints Integer values corresponding to terminals
 * @param currnode ANODE* The current AST node
 * @param symboltable SYMBOLTABLE* The symbol table pointer
 * @param should_start_function int* Output flag that a function
 *        should be started at the next node
 * @param function_scope_started int* Output flag that fn scope
 *        has started
 * @param tokenname char* The name of the token for this AST node
 * @param linenumber int The line number for this token
 * @param stbdumpfile FILE* The pointer to use for STB data dumps
 */

void handleAuxiliaryTerminalOperations (
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
    FILE *stbdumpfile )
{

  // Here  'topvalue' = the string that was popped from the stack
  //       'token' = the next line that was read from the input
  //       'tokenname' = name of the variable / value of the literal
  //       linenumber = the linenumber for variables / literals

  handleAuxiliaryOpsForNonIdentifiers (
    beginint, endint, intint,
    stringint, floatint, vertexint, edgeint,
    graphint, treeint, nothingint, andint,
    orint, notint, plusint, minusint,
    mulint, divint, moduloint, gtint,
    ltint, gteint, lteint, eqint,
    bftint, dftint, functionint, terminalvalue,
    currnode, symboltable, should_start_function,
    function_scope_started );

  if ( terminalvalue == idenint )
  {
    ANODE *parentnode = currnode -> parent;
    if ( parentnode -> node_type == AST_DEFINE_NODE
         || parentnode -> node_type == AST_FUNCTION_NODE
         || parentnode -> node_type == AST_GLOBALDEFINE_NODE
         || parentnode -> node_type == AST_QUALIFIEDPARAMETER_NODE )
    {
      // At a define node, so need to add an entry to the symbol table.
      // the data type for the node should be identifiable from the first child
      // of the current node.

      if ( getEntryByName ( symboltable, tokenname ) != NULL )
      {
        // Report an error if the current identifier is a variable and had been declared in the same scope before
        if ( parentnode -> node_type != AST_FUNCTION_NODE )
        {
          STBENTRY *previousentry = getEntryByName ( symboltable, tokenname );

          if ( previousentry -> entry_type == ENTRY_FUNC_TYPE )
          {
            // A variable is being declared now, but the previous entry by this name is a function
            // Throw an error and ignore this variable definition
            fprintf ( stderr, "Redeclaration of variable %s at line %d\n", tokenname, linenumber );
            fprintf ( stderr, "Note: Previous declaration of %s as a function on line %d.\n",
                      tokenname, previousentry -> data . func_data . decl_line );
            construction_error_occurred = 1;

            // Do not count this variable declaration
            // Return from the function to avoid further processing
            return;
          }

          if ( previousentry -> data . var_data . scope_level == symboltable -> cur_scope )
          {
            fprintf ( stderr, "Redeclaration of variable %s at line number %d\n", tokenname, linenumber );
            fprintf ( stderr, "Note: Previous declaration of %s as type %s on line %d.\n",
                      tokenname, getDataTypeName ( previousentry -> data . var_data . data_type ),
                      previousentry -> data . var_data . decl_line );
            construction_error_occurred = 1;

            // Do not count the redeclaration, process the rest of the input as
            // though this declaration didn't occur
            return;
          }
        }
        else
        {
          // Is a duplicate function definition
          STBENTRY *previousentry = getEntryByName ( symboltable, tokenname );
          fprintf ( stderr, "Redeclaration of function %s at line number %d\n", tokenname, linenumber );
          fprintf ( stderr, "Note: Previous declaration of %s at line number %d.\n", tokenname,
                    previousentry -> data . func_data . decl_line );
          construction_error_occurred = 1;

          // Update the function to the latest declaration, and let the
          // parameters be updated automatically.
          // Don't return at this point, this way the start of the func
          // scope is handled correctly after the else below
        }
      }
      else
      {
        unsigned insertedIndex = -1;

        if ( parentnode -> node_type == AST_FUNCTION_NODE )
        {
          insertedIndex = ( unsigned int ) addEntry ( symboltable, tokenname, ENTRY_FUNC_TYPE );
          if ( DEBUG_AUXOPS ) printf ( "Added function entry %s at scope %d\n", tokenname, symboltable -> cur_scope );
          STBENTRY *insertedEntry = getEntryByIndex ( symboltable, insertedIndex );

          FUNCTION *funcdata = & ( insertedEntry -> data . func_data );

          funcdata -> decl_line = linenumber;

          int len = strlen ( tokenname );
          funcdata -> name = malloc ( (len+1) * sizeof ( char ) );

          if ( funcdata -> name == NULL )
          {
            fprintf ( stderr, "Failed to allocate memory for function name\n" );
            construction_error_occurred = 1;
            exit (-1);
          }

          strcpy ( funcdata -> name, tokenname );

          currnode -> extra_data . symboltable_index = insertedIndex;

          dumpEntry ( symboltable, stbdumpfile, insertedIndex, 'd' );
        }
        else
        {
          // Not a function node, so it must be a variable declaration.
          // Add the corresponding entry in the symbol table.
          insertedIndex = addEntry ( symboltable, tokenname, ENTRY_VAR_TYPE );
          if ( DEBUG_AUXOPS ) printf ( "Added variable entry %s at scope %d\n", tokenname, symboltable -> cur_scope );

          STBENTRY *insertedEntry = getEntryByIndex ( symboltable, insertedIndex );

          VARIABLE *vardata = & ( insertedEntry -> data . var_data );

          if ( parentnode -> node_type == AST_GLOBALDEFINE_NODE )
            vardata -> var_type = V_GLOBAL_TYPE;
          else if ( parentnode -> node_type == AST_QUALIFIEDPARAMETER_NODE )
            vardata -> var_type = V_PARAM_TYPE;
          else if ( parentnode -> node_type == AST_DEFINE_NODE )
            vardata -> var_type = V_LOCAL_TYPE;
          else
          {
            fprintf ( stderr, "At node %s did not recognize variable type\n", getNodeTypeName ( currnode -> node_type ) );
            construction_error_occurred = 1;
          }

          // Set the data type using the data type of the first child
          vardata -> data_type = getDataType ( parentnode );
          vardata -> decl_line = linenumber;

          if ( DEBUG_AUXOPS ) printf ( "Set data type of %s as %s in Symbol Table\n", tokenname, getDataTypeName ( vardata -> data_type ) );

          currnode -> extra_data . symboltable_index = insertedIndex;

          dumpEntry ( symboltable, stbdumpfile, insertedIndex, 'd' );
        }
      }
    }
    else
    {
      // Variable is being referenced, not declared, here so push it into the linkedlist of references
      STBENTRY *foundEntry = getEntryByName ( symboltable, tokenname );

      // If the variable is not found in the symbol table, it hasn't been declared yet in this scope.
      // so report an error
      if ( foundEntry == NULL )
      {
        fprintf ( stderr, "Variable %s used but has not been declared in this scope\n", tokenname );
        fprintf ( stderr, "Note: Error at line %d.\n", linenumber );

        construction_error_occurred = 1;

        // Let all the rest of the errors associated with the undeclared variable
        // show up as well, for ease of debugging
        return;
      }
      else
      {
        if ( foundEntry -> entry_type == ENTRY_VAR_TYPE )
          foundEntry -> data . var_data . refr_lines =
            insertAtBack ( foundEntry -> data . var_data . refr_lines, &linenumber );
        else if ( foundEntry -> entry_type == ENTRY_FUNC_TYPE )
          foundEntry -> data . func_data . refr_lines =
            insertAtBack ( foundEntry -> data . func_data . refr_lines, &linenumber );

      }

      currnode -> extra_data . symboltable_index = foundEntry -> index;

      dumpEntry ( symboltable, stbdumpfile, foundEntry -> index, 'r' );
    }

    // Finished processing the identifier, if this identifier was a function name,
    // then we need to start a new scop here itself, so that parameters belong to
    // the inner scope.
    // We set the function_scope_started parameter so that the TK_BEGIN for the
    // function body doesn't start another scope, again.

    if ( *should_start_function == 1 )
    {
      *should_start_function = 0;
      symboltable = openEnv ( symboltable );
      *function_scope_started = 1;
    }
  }
  else if ( terminalvalue == intlitint || terminalvalue == floatlitint
            || terminalvalue == stringlitint )
  {
    // Add entry for the literal in the symbol table if it doesn't already exist

    STBENTRY *entry = getEntryByName ( symboltable, tokenname );

    // If entry doesn't exist, the entry needs to be added for the literal
    // If the entry already exists, nothing further needs to be done.
    if ( entry == NULL )
    {
      unsigned int entryIndex = addEntry ( symboltable, tokenname, ENTRY_LIT_TYPE );
      entry = getEntryByIndex ( symboltable, entryIndex );

      // Entry now has the entry index. Set the value appropriately
      if ( terminalvalue == intlitint )
        entry -> data . lit_data . lit_type = D_INT_TYPE;
      else if ( terminalvalue == floatlitint )
        entry -> data . lit_data . lit_type = D_FLOAT_TYPE;
      else if ( terminalvalue == stringlitint )
        entry -> data . lit_data . lit_type = D_STRING_TYPE;

      int len = strlen ( tokenname );
      entry -> data . lit_data . value = malloc ( (len+1) * sizeof ( char ) );

      if ( entry -> data . lit_data . value == NULL )
      {
        fprintf ( stderr, "Failed to allocate memory for literal\n" );
        construction_error_occurred = 1;
        exit (-1);
      }

      strcpy ( entry -> data . lit_data . value, tokenname );
    }

    currnode -> extra_data . symboltable_index = entry -> index;

    // Dump literal entries every time, no difference b/w definition and reference
    dumpEntry ( symboltable, stbdumpfile, entry -> index, 'd' );
  }

  // End handling aux ops for terminals
  // Nothing more needs to be done
}

/**
 * Nodes that have associated properties need to be handled separately
 * and with higher priority than their corresponding instructions.
 * This function handles those properties (refer to config folder)
 *
 * @param currnode ANODE** Input/output parameter of the current node
 * @param topavlue char* The top of the stack during construction
 * @param jumps TNODE* Trie node with data about the number of jumps
 * @param stack STACK** The stack used for construction
 * @param ast AST* The AST being constructed
 * @param shouldAdd int* Output flag for whether the node should be added back
 *        to the stack
 * @param shouldRead int* Output flag for whether input should be read
 */

void handleNodeProperty (
    ANODE **currnode, const char *topvalue, TNODE *jumps,
    STACK **stack, AST *ast, int *shouldAdd, int *shouldRead )
{
  int numberOfJumps = ( (PROPERTY *) jumps -> data . generic_val ) -> jumps;
  int shouldCreate =
    ( ( ( ( (PROPERTY *) jumps -> data . generic_val ) -> instruction )
        & PROPERTY_CREATE
      ) == PROPERTY_CREATE );

  int nodetype_tocreate = ( (PROPERTY *) jumps -> data . generic_val ) -> node_type;

  *shouldRead =
    ( ( ( ( (PROPERTY *) jumps -> data . generic_val ) -> instruction )
        & PROPERTY_READ
      ) == PROPERTY_READ );

  *shouldAdd =
    ( ( ( ( (PROPERTY *) jumps -> data . generic_val ) -> instruction )
        & PROPERTY_ADD
      ) == PROPERTY_ADD );

  if ( DEBUG_ALL )
    printf ( "At node %s got %s so jumping %d times\n",
              getNodeTypeName ( (*currnode) -> node_type ),
              topvalue,
              numberOfJumps );

  if ( shouldCreate == 0 )
  {
    // Should jump
    while ( numberOfJumps > 0 )
    {
      if ( getParent ( *currnode ) == NULL || getParent ( *currnode ) == ast -> root )
        break;
      *currnode = getParent ( *currnode );
      numberOfJumps --;
    }
  }
  else if ( shouldCreate == 1 )
  {
    if ( DEBUG_ALL || DEBUG_ONCREATE )
      printf ( "At node %s got %s so creating and going to %s\n\n",
               getNodeTypeName ( (*currnode) -> node_type ), topvalue,
               getNodeTypeName ( nodetype_tocreate ) );

    *currnode = addChild ( *currnode, nodetype_tocreate, GOTOCH );

    // numberOfJumps is the number of elements to pop from the stack
    while ( numberOfJumps > 0 )
    {
      if ( isEmpty ( *stack ) )
      {
        fprintf ( stderr, "Instructions say pop elements, but stack is empty!\n" );
        construction_error_occurred = 1;
        exit (-1);
      }
      *stack = pop ( *stack );
      numberOfJumps --;
    }
  }

  return;
}

/**
 * Handle the instructions associated with a given AST node
 * (refer to config folder)
 *
 * @param astoutput FILE* The file to dump the AST output in
 * @param currnode ANODE** The current AST node input/output parameter
 * @param currentval TNODE* The Trie node possessing the current instruction
 * @param ast AST* The AST being constructed
 * @param topvalue char* The top of the stack
 * @param auxdata TRIE* The trie possessing auxiliary data for instructions
 * @param conditional_read int* Output flag to check conditional read
 * @param conditional_pop int* Output flag to conditional pop the top of the stack
 * @param conditional_value int* The value to check against for the condition
 * @param conditional_type CONDTYPE* The type of the condition
 *
 * @return int 1 if the next input should be read, else 0
 */

int handleNodeInstruction (
    FILE *astoutput, ANODE **currnode, TNODE *currentval, AST *ast,
    const char *topvalue, TRIE *auxdata, int *conditional_read,
    int *conditional_pop, int *conditional_value, CONDTYPE *conditional_type )
{
  // instruction stores the encoded instruction, the individual bits need to be
  // examined to decode the instruction
  int instruction = currentval -> data . int_val;

  if ( (instruction & CREATE) == CREATE )
  {
    // If we have to create a node, examine the type of node to be created
    TNODE *typeofnode = findString ( auxdata, topvalue );

    if ( typeofnode == NULL )
    {
      fprintf ( stderr, "Node instruction is create, but type to create not found\n" );
      construction_error_occurred = 1;
      exit (-1);
    }

    if ( ( (AUXDATA *) ( typeofnode -> data . generic_val ) ) -> conditional_type != CONDITIONAL_NODETYPE )
    {
      fprintf ( stderr, "Node instruction is create, but the auxiliary data provided is not a node type\n" );
      construction_error_occurred = 1;
      exit (-1);
    }

    int type_to_create = ( (AUXDATA *) ( typeofnode -> data . generic_val ) ) -> conditional_value;

    if ( DEBUG_ALL || DEBUG_ONCREATE ) printf ( "At node %s\n", getNodeTypeName ( (*currnode) -> node_type ) );

    if ( (*currnode) -> node_type < 0 )
      fprintf ( astoutput, "At node ROOT_NODE\n" );
    else
      fprintf ( astoutput, "At node %s\n", getNodeTypeName ( (*currnode) -> node_type ) );

    *currnode = addChild ( *currnode, type_to_create, instruction );

    if ( DEBUG_ALL ) printf ( "Got %s so ", topvalue );

    if ( DEBUG_ALL || DEBUG_ONCREATE ) printf ( "Creating: %s\n\n", getNodeTypeName ( type_to_create ) );

    fprintf ( astoutput, "Creating: %s\n\n", getNodeTypeName ( type_to_create ) );

    if ( DEBUG_ALL ) printf ( "\n\n" );

    // currnode is now either the same node, child, or parent depending on
    // the instruction.

    // If the instruction says read, then return the read status 1.
    if ( (instruction & READ) == READ )
      return 1;
  }
  else if ( (instruction & GOTOCH) == GOTOCH )
  {
    fprintf ( stderr, "Instruction says goto child, but no child was created. So which child?\n" );
    construction_error_occurred = 1;
    return -1;
  }
  else if ( (instruction & PARENT) == PARENT )
  {
    if ( DEBUG_ALL ) printf ( "At node %s\n", getNodeTypeName ( (*currnode) -> node_type ) );
    if ( DEBUG_ALL ) printf ( "Got %s so ", topvalue );
    if ( DEBUG_ALL ) printf ( "Going to parent\n" );

    if ( getParent ( *currnode ) == NULL || getParent ( *currnode ) == ast -> root )
      fprintf ( stderr, "Parent is null, so remaining at same node\n" );
    else
      *currnode = getParent ( *currnode );

    // If the instruction says read, we should return the read status 1
    if ( (instruction & READ) == READ )
      return 1;
  }
  else if ( (instruction & CONDRD) == CONDRD )
  {
    if ( DEBUG_ALL ) printf ( "At node %s\n", getNodeTypeName ( (*currnode) -> node_type ) );
    if ( DEBUG_ALL ) printf ( "Got %s so ", topvalue );
    if ( DEBUG_ALL ) printf ( "conditionally reading/popping next value %d\n", instruction );
    // Instruction is a conditional read
    // Set the conditional read flag and value so that
    // currnode will be changed to parent on that value

    if ( (instruction & READ) == READ )
      *conditional_read = 1;
    else
      *conditional_pop = 1;

    TNODE *cond_value = findString ( auxdata, topvalue );
    if ( cond_value == NULL )
    {
      fprintf ( stderr, "Conditional read & goto parent has no auxiliary data\n" );
      construction_error_occurred = 1;
      return -1;
    }

    *conditional_value = ( (AUXDATA *) ( cond_value -> data . generic_val ) ) -> conditional_value;
    *conditional_type = ( (AUXDATA *) ( cond_value -> data . generic_val ) ) -> conditional_type;

    if ( (instruction & READ) == READ )
      return 1;

    if ( DEBUG_ALL ) printf ( "Not reading as conditional read failed\n" );
  }
  else if ( (instruction & READ) == READ )
  {
    if ( DEBUG_ALL ) printf ( "At node %s\n", getNodeTypeName ( (*currnode) -> node_type ) );
    if ( DEBUG_ALL ) printf ( "Got %s so ", topvalue );
    if ( DEBUG_ALL ) printf ( "Reading next input...\n" );
    return 1;
  }

  // Should not read, so return 0
  return 0;
}

/**
 * Function that mentions whether an error occurred during
 * AST construction
 *
 * @return int 1 if an error occurred, 0 otherwise
 */

int constructionErrorOccurred ()
{
  return construction_error_occurred;
}
