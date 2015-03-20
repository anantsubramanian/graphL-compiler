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
  symboltable -> num_entries = -1;
  symboltable -> cur_scope = -1;
  symboltable -> cur_subscope = -1;
  symboltable -> size = -1;

  symboltable -> entries = NULL;

  symboltable -> environments = getStack ( STACK_GENERIC_TYPE );
  symboltable -> environments = setStackGenericSize ( symboltable -> environments,
                                                      sizeof ( LINKEDLIST ) );

  return symboltable;
}

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

SYMBOLTABLE* setNumEntries ( SYMBOLTABLE *symboltable, unsigned int num_entries )
{
  if ( symboltable == NULL )
  {
    fprintf ( stderr, "Cannot set number of entries of non-existent symbol table\n" );
    return NULL;
  }

  if ( symboltable -> num_entries != -1 )
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
  {
    symboltable -> entries [i] = NULL;
    symboltable -> entries [i] = malloc ( sizeof ( STACK ) );

    if ( symboltable -> entries [i] == NULL )
    {
      fprintf ( stderr, "Failed to allocate memory for one of the symbol table entries\n" );
      return NULL;
    }
  }

  return symboltable;
}

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

  symboltable -> environments = pop ( symboltable -> environments );
  symboltable -> cur_scope --;

  return symboltable;
}

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

  if ( value_type < ENTRY_VAR_TYPE || value_type > ENTRY_LIT_TYPE )
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

    symboltable -> entries [ foundval ] = getStack ( STACK_GENERIC_TYPE );
    symboltable -> entries [ foundval ] = setStackGenericSize ( symboltable -> entries
                                                                [ foundval ], sizeof ( STBENTRY ) );
  }

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

  return ( STBENTRY * ) top ( symboltable -> entries [ indexlocator -> data . int_val ] );
}

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

  return ( STBENTRY * ) top ( symboltable -> entries [ index ] );
}

