// Authors: Anant Subramanian <anant.subramanian15@gmail.com>
//          Aditya Bansal <adityabansal_adi@yahoo.co.in>
//
// BITS PILANI ID NOs: 2012A7TS010P
//                     2012A7PS122P
//
// Project Team Num: 1
// Project Group No. 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symboltable.h"

#define SYMBOLTABLE_ENV_STACK_NAME "Symbol table environments stack"
#define TO_APPEND_LENGTH 13
#define TO_APPEND "'s STB Stack"
#define STRINGLITLEN 400
#define VARFUNCLEN 400
#define NEWLINECHAR '\n'

/**
 * Function that allocates memory for and returns a new symbol table
 *
 * @return SYMBOLTABLE* The pointer to the allocated symbol table
 *
 */

SYMBOLTABLE* getSymbolTable ()
{
  SYMBOLTABLE *symboltable;
  symboltable = NULL;

  symboltable = malloc ( sizeof ( SYMBOLTABLE ) );
  if ( symboltable == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for symbol table\n" );
    return NULL;
  }

  symboltable -> name = NULL;
  symboltable -> indexmap = getNewTrie ( TRIE_INT_TYPE );

  // Set symbol table entries to sentinel value
  symboltable -> num_entries = 0;
  symboltable -> cur_scope = -1;
  symboltable -> cur_subscope = -1;
  symboltable -> size = -1;

  symboltable -> entries = NULL;

  symboltable -> environments = getStack ( STACK_GENERIC_TYPE );
  symboltable -> environments = setStackName ( symboltable -> environments,
                                               SYMBOLTABLE_ENV_STACK_NAME );
  symboltable -> environments = setStackGenericSize ( symboltable -> environments,
                                                      sizeof ( LINKEDLIST ) );

  return symboltable;
}

/**
 * Function that sets/clears the name of a symbol table
 *
 * @param  symboltable SYMBOLTABLE* The target symbol table
 * @param  name char* The string containing the name
 *
 * @return SYMBOLTABLE* The pointer to the altered symbol table
 *
 */

SYMBOLTABLE* setSymbolTableName ( SYMBOLTABLE *symboltable, char * name )
{
  if ( symboltable == NULL )
  {
    fprintf ( stderr, "Cannot set name for non-existent symbol table\n" );
    return NULL;
  }

  if ( symboltable -> name != NULL )
    free ( symboltable -> name );

  symboltable -> name = NULL;

  if ( name != NULL )
  {
    int len = strlen ( name );
    symboltable -> name = malloc ( (len+1) * sizeof ( char ) );

    if ( symboltable -> name == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for symbol table name\n" );
      return NULL;
    }

    strcpy ( symboltable -> name, name );
  }

  return symboltable;
}

/**
 * Function that sets the maximum number of entries in the symbol table
 *
 * @param  symboltable SYMBOLTABLE* The target symbol table
 * @param  num_entries uint The number of entries
 *
 * @return SYMBOLTABLE* The pointer to the altered symbol table
 *
 */

SYMBOLTABLE* setNumEntries ( SYMBOLTABLE *symboltable, unsigned int num_entries )
{
  if ( symboltable == NULL )
  {
    fprintf ( stderr, "Cannot set number of entries of non-existent symbol table\n" );
    return NULL;
  }

  if ( symboltable -> num_entries != 0 )
  {
    fprintf ( stderr, "Symbol table already has entries. Cannot reset number of entries\n" );
    return NULL;
  }

  symboltable -> num_entries = num_entries;
  symboltable -> size = 0;

  // Allocate memory for entries
  symboltable -> entries = malloc ( num_entries * sizeof ( STACK * ) );

  if ( symboltable -> entries == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for symbol table entries\n" );
    return NULL;
  }

  unsigned int i;
  for ( i = 0; i < num_entries; i++ )
    symboltable -> entries [i] = NULL;

  return symboltable;
}

/**
 * Function that opens a new environment in the symbol table
 *
 * @param  symboltable SYMBOLTABLE* The target symbol table
 *
 * @return SYMBOLTABLE* The pointer to the altered symbol table
 *
 */

SYMBOLTABLE* openEnv ( SYMBOLTABLE *symboltable )
{
  if ( symboltable == NULL )
  {
    fprintf ( stderr, "Cannot open a new environment in a non-existent symbol table\n" );
    return NULL;
  }

  symboltable -> cur_scope ++;
  symboltable -> cur_subscope = 0;

  LINKEDLIST *newenv = NULL;
  newenv = getLinkedList ( LL_INT_TYPE );

  symboltable -> environments = push ( symboltable -> environments, newenv );

  return symboltable;
}

/**
 * Function that closes the last opened environment of the symbol table
 *
 * @param  symboltable SYMBOLTABLE* The target symbol table
 *
 * @return SYMBOLTABLE* The pointer to the altered symbol table
 *
 */

SYMBOLTABLE* closeEnv ( SYMBOLTABLE *symboltable )
{
  if ( symboltable == NULL )
  {
    fprintf ( stderr, "Cannot close env for non existent symbol table\n" );
    return NULL;
  }

  if ( symboltable -> cur_scope == -1 )
  {
    fprintf ( stderr, "No environments to close in given symbol table\n" );
    return NULL;
  }

  // Iterate over the current environment (top of the stack)
  // linked list, of integers representing indices, and pop
  // from the stack corresponding to each integer in the LL
  LINKEDLIST *curenv = ( LINKEDLIST * ) top ( symboltable -> environments );
  LNODE iterator;

  getIterator ( curenv, &iterator );
  while ( hasNext ( &iterator ) )
  {
    getNext ( curenv, &iterator );
    if ( isEmpty ( symboltable -> entries [ iterator.data.int_val ] ) )
    {
      fprintf ( stderr, "Failed to close environment, one of the current env variables doesn't exist\n" );
      return NULL;
    }
    symboltable -> entries [ iterator.data.int_val ] = pop ( symboltable -> entries
                                                             [ iterator.data.int_val ] );
  }

  // Before popping the current environment linked list from the stack of environments,
  // first delete the environment linked list, as the onus to free the memory allocated
  // for the linked list is at the user level in a generic type stack
  deleteLinkedList ( curenv );

  symboltable -> environments = pop ( symboltable -> environments );
  symboltable -> cur_scope --;

  return symboltable;
}

/**
 * Function that dumps the data in the entry currently in the symbol table
 * at the index specified by index to the mentioned file.
 *
 * @param  symboltable SYMBOLTABLE* The target symbol table
 * @param  dumpfile FILE* The open file pointer to the dump file
 * @param  index uint The index of the entry to be dumped
 *
 * @return SYMBOLTABLE* The symbol table, intact if the dump was successful
 *
 */

SYMBOLTABLE* dumpEntry ( SYMBOLTABLE *symboltable, FILE *dumpfile, unsigned int index, char dumptype )
{
  if ( symboltable == NULL )
  {
    fprintf ( stderr, "Cannot dump data from a non-existent symbol table\n" );
    return NULL;
  }

  if ( dumpfile == NULL )
  {
    fprintf ( stderr, "Cannot dump data from symbol table in a non existent file\n" );
    return NULL;
  }

  if ( index >= symboltable -> num_entries )
  {
    fprintf ( stderr, "Requesting dump of out of bounds STB index\n" );
    return NULL;
  }

  if ( isEmpty ( symboltable -> entries [ index ] ) )
  {
    fprintf ( stderr, "Cannot dump non-existent entry of symbol table\n" );
    return NULL;
  }

  STBENTRY *entry = ( STBENTRY * ) top ( symboltable -> entries [ index ] );

  if ( entry -> entry_type == ENTRY_LIT_TYPE )
  {
    fprintf ( dumpfile, "L " );
    fprintf ( dumpfile, "%d ", entry -> index );
    LITERAL *litentry = & ( entry -> data . lit_data );
    fprintf ( dumpfile, "%d\n", litentry -> lit_type );
    fprintf ( dumpfile, "%s\n", litentry -> value );
  }
  else if ( entry -> entry_type == ENTRY_VAR_TYPE )
  {
    fprintf ( dumpfile, "V " );
    fprintf ( dumpfile, "%d\n", entry -> index );

    // Dump extra data only for definitions, not for references
    if ( dumptype == 'd' )
    {
      VARIABLE *varentry = & ( entry -> data . var_data );
      fprintf ( dumpfile, "%s\n", varentry -> name );
      fprintf ( dumpfile, "%d ", varentry -> data_type );
      fprintf ( dumpfile, "%d ", varentry -> var_type );
      fprintf ( dumpfile, "%d ", varentry -> scope_level );
      fprintf ( dumpfile, "%d ", varentry -> scope_sublevel );
      fprintf ( dumpfile, "%d\n", varentry -> decl_line );
    }
  }
  else if ( entry -> entry_type == ENTRY_FUNC_TYPE )
  {
    fprintf ( dumpfile, "F " );
    fprintf ( dumpfile, "%d\n", entry -> index );

    // Dump extra data only for definitions, not for references
    if ( dumptype == 'd' )
    {
      FUNCTION *funcentry = & ( entry -> data . func_data );
      fprintf ( dumpfile, "%s\n", funcentry -> name );
      fprintf ( dumpfile, "%d ", funcentry -> num_params );
      fprintf ( dumpfile, "%d ", funcentry -> ret_type );
      fprintf ( dumpfile, "%d ", funcentry -> returndata_stbindex );
      fprintf ( dumpfile, "%d\n", funcentry -> decl_line );
    }
  }

  return symboltable;
}

/**
 * Function that reads the next entry from a dump file generated by the dumpEntry
 * function, and populates the symbol table appropriately.
 * Note: the function has the side effect of modifying the file pointer
 *
 * @param  symboltable SYMBOLTABLE* The target symbol table
 * @param  dumpfile FILE* The open file pointer to the dump file
 *
 * @return int Index in the STB of the entry that was read
 *
 */

unsigned int readDumpEntry ( SYMBOLTABLE *symboltable, FILE *dumpfile, char dumptype )
{
  if ( symboltable == NULL )
  {
    fprintf ( stderr, "Cannot dump data from a non-existent symbol table\n" );
    return -1;
  }

  if ( dumpfile == NULL )
  {
    fprintf ( stderr, "Cannot dump data from symbol table in a non existent file\n" );
    return -1;
  }

  char entrytype;
  do
  {
    fscanf ( dumpfile, "%c", &entrytype );
  } while ( entrytype != 'L' && entrytype != 'V' && entrytype != 'F' );

  STBENTRY *newentry = NULL;
  newentry = malloc ( sizeof ( STBENTRY ) );
  if ( newentry == NULL )
  {
    fprintf ( stderr, "Failed to create entry while reading STB dump\n" );
    return -1;
  }

  if ( entrytype == 'L' )
  {
    newentry -> entry_type = ENTRY_LIT_TYPE;
    fscanf ( dumpfile, "%d", & ( newentry -> index ) );
    int littype = 0;
    fscanf ( dumpfile, "%d", & littype );
    newentry -> data . lit_data . lit_type = littype;

    // The processing being done below is necessary as string literals could have spaces
    char buffer [ STRINGLITLEN ];
    int index = 0;
    char c;

    do
    {
      fscanf ( dumpfile, "%c", &c );
    } while ( c <= 32 );

    do
    {
      buffer [ index ++ ] = c;
      fscanf ( dumpfile, "%c", &c );
    } while ( c != NEWLINECHAR );

    buffer [ index ] = '\0';

    int len = strlen ( buffer ) + 1;
    newentry -> data . lit_data . value = malloc ( len * sizeof ( char ) );

    strcpy ( newentry -> data . lit_data . value, buffer );
    // Emd literal value processing

    int entryindex = newentry -> index;

    if ( checkIndexExistence ( symboltable, newentry -> index ) )
    {
      // Entry already exists in symbol table, no additional work for literals
      free ( newentry );
    }
    else
    {
      int addindex = addEntry ( symboltable, newentry -> data . lit_data . value, ENTRY_LIT_TYPE );
      if ( entryindex != addindex )
      {
        fprintf ( stderr, "Insertion index and dump index don't match!\n" );
        return -1;
      }
    }

    return entryindex;
  }
  else if ( entrytype == 'V' )
  {
    newentry -> entry_type = ENTRY_VAR_TYPE;
    fscanf ( dumpfile, "%d", & ( newentry -> index ) );

    // Only perform extra reads and ops for define dumps
    if ( dumptype == 'd' )
    {
      VARIABLE *varentry = & ( newentry -> data . var_data );

      char buffer [ VARFUNCLEN ];
      fscanf ( dumpfile, "%s", buffer );

      int len = strlen ( buffer ) + 1;
      varentry -> name  = malloc ( len * sizeof ( char ) );

      if ( varentry -> name == NULL )
      {
        fprintf ( stderr, "Failed to allocate memory for entry name while reading STB dump\n" );
        return -1;
      }

      strcpy ( varentry -> name, buffer );

      int datatype = 0;
      fscanf ( dumpfile, "%d", & datatype );
      varentry -> data_type = datatype;

      int vartype = 0;
      fscanf ( dumpfile, "%d", & vartype );
      varentry -> var_type = vartype;

      fscanf ( dumpfile, "%d", & ( varentry -> scope_level ) );
      fscanf ( dumpfile, "%d", & ( varentry -> scope_sublevel ) );
      fscanf ( dumpfile, "%d", & ( varentry -> decl_line ) );

      int addindex = addEntry ( symboltable, varentry -> name, ENTRY_VAR_TYPE );

      if ( addindex != newentry -> index )
      {
        fprintf ( stderr, "Insertion index and dump index don't match!\n" );
        return -1;
      }
    }

    return newentry -> index;
  }
  else if ( entrytype == 'F' )
  {
    newentry -> entry_type = ENTRY_FUNC_TYPE;
    fscanf ( dumpfile, "%d", & ( newentry -> index ) );

    // Only perform extra reads and ops for define dumps
    if ( dumptype == 'd' )
    {
      FUNCTION *funcentry = & ( newentry -> data . func_data );

      char buffer [ VARFUNCLEN ];
      fscanf ( dumpfile, "%s", buffer );

      int len = strlen ( buffer ) + 1;
      funcentry -> name = malloc ( len * sizeof ( char ) );

      if ( funcentry -> name == NULL )
      {
        fprintf ( stderr, "Failed to allocate memory for name while reading STB dump\n" );
        return -1;
      }

      strcpy ( funcentry -> name, buffer );

      fscanf ( dumpfile, "%d", & ( funcentry -> num_params ) );

      int returntype = 0;
      fscanf ( dumpfile, "%d", & returntype );
      funcentry -> ret_type = returntype;

      fscanf ( dumpfile, "%d", & ( funcentry -> returndata_stbindex ) );
      fscanf ( dumpfile, "%d", & ( funcentry -> decl_line ) );

      // This function just adds the dump file entry. Onus is on the API user
      // to ensure that functions aren't added twice if this is not allowed
      // in the lang
      int addindex = addEntry ( symboltable, funcentry -> name, ENTRY_FUNC_TYPE );

      if ( addindex != newentry -> index )
      {
        fprintf ( stderr, "Insertion index and dump index don't match!\n" );
        return -1;
      }
    }

    return newentry -> index;
  }

  // Unknown entry read from dump
  return -1;
}

/**
 * Function that adds an entry of the given type, to the symbol table
 * in the current scope and returns the index of the entry in the table
 *
 * @param  symboltable SYMBOLTABLE* The target symbol table
 * @param  toinsert char* The name of the entry being inserted
 * @param  value_type STB_ENTRYTYPE The type of the STB entry
 *
 * @return int The unique index used to identify the entry in the STB
 *
 */

int addEntry ( SYMBOLTABLE *symboltable, char *toinsert, STB_ENTRYTYPE value_type )
{
  if ( symboltable == NULL )
  {
    fprintf ( stderr, "Attempting to insert into a non-existent symbol table\n" );
    return -1;
  }

  if ( toinsert == NULL )
  {
    fprintf ( stderr, "Attempting to insert an empty string into symbol table\n" );
    return -1;
  }

  if ( value_type <= ENTRY_TYPE_FIRST || value_type >= ENTRY_TYPE_LAST )
  {
    fprintf ( stderr, "Invalid type of data object being inserted in symbol table\n" );
    return -1;
  }

  if ( isEmpty ( symboltable -> environments ) )
  {
    fprintf ( stderr, "Cannot insert into a symbol table with no environments\n" );
    return -1;
  }

  STBENTRY *newentry = NULL;
  newentry = malloc ( sizeof ( STBENTRY ) );

  if ( newentry == NULL )
  {
    fprintf ( stderr, "Failed to allocate memory for symbol table node during insertion\n" );
    return -1;
  }

  // Set the type of the new node
  newentry -> entry_type = value_type;

  if ( value_type == ENTRY_VAR_TYPE )
  {
    VARIABLE *varobj = & ( newentry -> data . var_data );
    varobj -> name = NULL;
    int len = strlen ( toinsert );

    varobj -> name = malloc ( (len+1) * sizeof ( char ) );
    if ( varobj == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for variable name while inserting in ST\n" );
      return -1;
    }
    strcpy ( varobj -> name, toinsert );

    varobj -> data_type = -1;
    varobj -> var_type = -1;
    varobj -> scope_level = symboltable -> cur_scope;
    varobj -> scope_sublevel = symboltable -> cur_subscope;
    varobj -> decl_line = -1;
    // LINKEDLIST of integers that stores the lines in which the variable is referenced
    varobj -> refr_lines = getLinkedList ( LL_INT_TYPE );
    varobj -> data . int_value = 0;
    varobj -> data . string_value = 0;
    varobj -> data . float_value = 0;
    varobj -> data . complex_value = 0;
  }
  else if ( value_type == ENTRY_FUNC_TYPE )
  {
    FUNCTION *funcobj = & ( newentry -> data . func_data );
    funcobj -> name = NULL;
    int len = strlen ( toinsert );

    funcobj -> name = malloc ( (len+1) * sizeof ( char ) );
    if ( funcobj == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for function name while inserting in ST\n" );
      return -1;
    }
    strcpy ( funcobj -> name, toinsert );

    funcobj -> num_params = -1;
    // LINKEDLIST of integers that stores the indices of the paramters in the Symbol Table
    funcobj -> parameters = getLinkedList ( LL_INT_TYPE );
    funcobj -> paramtypes = getLinkedList ( LL_INT_TYPE );
    funcobj -> ret_type = -1;
    funcobj -> returndata_stbindex = -1;
    funcobj -> decl_line = -1;
    funcobj -> refr_lines = getLinkedList ( LL_INT_TYPE );
  }
  else if ( value_type == ENTRY_LIT_TYPE )
  {
    // The onus of converting the value and assigning it is on the API
    // user for integer and float literals
    LITERAL *litobj = & ( newentry -> data . lit_data );
    litobj -> lit_type = -1;
    litobj -> value = NULL;
  }

  TNODE *indexlocator = NULL;
  indexlocator = findString ( symboltable -> indexmap , toinsert );

  if ( indexlocator == NULL )
  {
    indexlocator = insertString ( symboltable -> indexmap, toinsert );
    indexlocator -> data . int_val = symboltable -> size++;
    int foundval = indexlocator -> data . int_val;

    int len = strlen ( toinsert );

    char *stackname = malloc ( (len + TO_APPEND_LENGTH) * sizeof ( char ) );

    strcpy ( stackname, toinsert );
    strcat ( stackname, TO_APPEND );

    symboltable -> entries [ foundval ] = getStack ( STACK_GENERIC_TYPE );
    symboltable -> entries [ foundval ] = setStackName ( symboltable -> entries [ foundval ],
                                                         stackname );
    symboltable -> entries [ foundval ] = setStackGenericSize ( symboltable -> entries
                                                                [ foundval ], sizeof ( STBENTRY ) );
  }

  newentry -> index = indexlocator -> data . int_val;

  symboltable -> entries [ indexlocator -> data . int_val ] =
    push ( symboltable -> entries [ indexlocator -> data . int_val ], newentry );

  // Push this index into the linked list for the current environment,
  // so that it will be popped on end
  LINKEDLIST *curenv = ( LINKEDLIST * ) top ( symboltable -> environments );

  insertAtBack ( curenv, & ( indexlocator -> data . int_val ) );

  // Return the index of the inserting value in the array of stacks
  // 'entries' in the symbol table, so it can be manipulated outside
  // the function at the user end
  return indexlocator -> data . int_val;
}

/**
 * Function that checks if an entry of the given name already
 * exists in the table, in the current scope
 *
 * @param  symboltable SYMBOLTABLE* The target symbol table
 * @param  tocheck char* The name of the entry to check
 *
 * @return int 1 if the entry exists, 0 otherwise
 *
 */

int checkNameExistence ( SYMBOLTABLE *symboltable, char *tocheck )
{
  if ( symboltable == NULL )
  {
    fprintf ( stderr, "Searching in a non-existent symbol table\n" );
    return -1;
  }

  if ( tocheck == NULL )
  {
    fprintf ( stderr, "Searching symbol table for a non-existent string\n" );
    return -1;
  }

  TNODE *indexlocator = findString ( symboltable -> indexmap, tocheck );

  if ( indexlocator == NULL )
    return FALSE;
  else if ( isEmpty ( symboltable -> entries [ indexlocator -> data . int_val ] ) )
    return FALSE;

  return TRUE;
}

/**
 * Function that checks if an entry of the given index
 * exists in the table, in the current scope
 *
 * @param  symboltable SYMBOLTABLE* The target symbol table
 * @param  index uint The index to check
 *
 * @return int 1 if the entry exists, 0 otherwise
 *
 */

int checkIndexExistence ( SYMBOLTABLE *symboltable, unsigned int index )
{
  if ( symboltable == NULL )
  {
    fprintf ( stderr, "Searching in a non-existent symbol table\n" );
    return -1;
  }

  if ( index >= ( unsigned int ) symboltable -> size )
    return FALSE;
  else if ( isEmpty ( symboltable -> entries [ index ] ) )
    return FALSE;

  return TRUE;
}

/**
 * Function that returns a Symbol Table entry using the name
 * of the entry to look it up
 *
 * @param  symboltable SYMBOLTABLE* The target symbol table
 * @param  toget char* The name of the entry to look up
 *
 * @return STBENTRY* The pointer to the entry in the STB
 *
 */

STBENTRY* getEntryByName ( SYMBOLTABLE *symboltable, char *toget )
{
  if ( symboltable == NULL )
  {
    fprintf ( stderr, "Cannot find value in a non-existent symbol table\n" );
    return NULL;
  }

  if ( toget == NULL )
  {
    fprintf ( stderr, "Cannot search for non-existent string in symbol table\n" );
    return NULL;
  }

  TNODE *indexlocator = NULL;
  indexlocator = findString ( symboltable -> indexmap, toget );

  // If entry not found, return NULL
  if ( indexlocator == NULL )
    return NULL;

  // If the stack is empty, the entry ran out of
  // scope, so even though there is an index value,
  // there is no entry
  if ( isEmpty ( symboltable -> entries [ indexlocator -> data . int_val ] ) )
    return NULL;

  return ( STBENTRY * ) top ( symboltable -> entries [ indexlocator -> data . int_val ] );
}

/**
 * Function that returns a Symbol Table entry using the index
 * of the entry to look it up
 *
 * @param  symboltable SYMBOLTABLE* The target symbol table
 * @param  index uint The index to look up
 *
 * @return STBENTRY* The pointer to the entry in the STB
 *
 */

STBENTRY* getEntryByIndex ( SYMBOLTABLE *symboltable, unsigned int index )
{
  if ( symboltable == NULL )
  {
    fprintf ( stderr, "Cannot index look-up a non-existent symbol table\n" );
    return NULL;
  }

  if ( index >= ( unsigned int ) symboltable -> size )
  {
    fprintf ( stderr, "Index out of bounds in symbol table\n" );
    return NULL;
  }

  // Similar to the getEntryByName case, if the entry
  // ran out of scope (i.e. the stack is empty), then
  // return NULL to indicate that there is no entry
  if ( isEmpty ( symboltable -> entries [ index ] ) )
    return NULL;

  return ( STBENTRY * ) top ( symboltable -> entries [ index ] );
}

