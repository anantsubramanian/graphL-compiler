#include "headers/dfa.h"
#include <stdio.h>
#define FILEPATH "config/dfa_lexer_description"
#define BUFFERLEN 200
#define ERRORS 5

// TODO (Aditya) : Only display tokens if number of errors == 0
// TODO (Aditya) : Read from file using two buffers

int main()
{
  DFA *dfa;
  dfa = getNewDFA();
  dfa = initializeFromFile ( dfa, FILEPATH );
  int shouldread = TRUE;
  char c;
  char stringliteral[ BUFFERLEN ];
  char identifier[ BUFFERLEN ];
  int idenindex = 0;
  int stringlitindex = 0;
  int intliteral = 0;
  float floatliteral = 0.0, floatliteral2 = 0.0;
  int decimalcount = 1;
  int started = FALSE;
  int linenumber=1;
  int errors = 0;
  FILE *f,*p;
  f = fopen ( "Errors.txt","w+");
  p = fopen ( "Tokens.txt","w+");
  while (1)
  {
    c = getchar();
    if ( c == EOF )
      break;

    if ( peek ( dfa, c ) == NULL || getSpecialProperty ( peek ( dfa, c ) ) == TRAP )
    {
      if ( isFinal ( getCurrentState (dfa) ) == TRUE )
      {
        if ( strcmp ( getCurrentState (dfa) -> name , "TK_NEWLINE" ) == 0 )
          linenumber++;
      
        if ( getSpecialProperty ( getCurrentState (dfa) ) == ERROR )
        {
          fprintf(f,"%d : %s\n",linenumber,getCurrentState (dfa) -> name);
          errors++;
          if ( errors >= 5 )
            break;
        }

        else if ( strcmp ( getCurrentState (dfa) -> name , "TK_INTLIT" ) == 0 )
          fprintf (p,"<TK_INTLIT,%d>\n", intliteral );
        else if ( strcmp ( getCurrentState (dfa) -> name , "TK_FLOATLIT" ) == 0 )
          fprintf (p,"<TK_FLOATLIT,%f>\n", floatliteral + floatliteral2 );
        else if ( strcmp ( getCurrentState (dfa) -> name , "TK_STRINGLIT" ) == 0 )
          fprintf (p,"<TK_STRINGLIT,%s>\n", stringliteral );
        else if ( strcmp ( getCurrentState (dfa) -> name , "TK_IDEN" ) == 0 )
          fprintf (p,"<TK_IDEN,%s>\n", identifier );
        else
          fprintf (p,"<%s>\n", getCurrentState (dfa) -> name );
      }
      
      if ( peek ( dfa, c ) == NULL )
        dfa = gotoInitialState ( dfa );
    }

    gotoNextState ( dfa, c );

    if ( getCurrentState (dfa) != NULL && getCurrentState (dfa) -> name != NULL )
    {
      if ( strcmp ( getCurrentState (dfa) -> name , "Decimal Point" ) == 0 )
      {
        floatliteral = (float) intliteral;
        decimalcount = 1;
        floatliteral2 = 0;
      }
      else if ( strcmp ( getCurrentState (dfa) -> name, "TK_FLOATLIT" ) == 0 )
      {
        decimalcount *= 10;
        floatliteral2 = floatliteral2 + ((c - 48) / (float) decimalcount);
      }
      else
        floatliteral = 0;

      if ( strcmp ( getCurrentState (dfa) -> name , "TK_INTLIT" ) == 0 )
        intliteral = intliteral * 10 + c - 48;
      else
        intliteral = 0;

      if ( strcmp ( getCurrentState (dfa) -> name , "Started Quote" ) == 0 )
      {
        stringliteral [ stringlitindex ++ ] = c;
        stringliteral [ stringlitindex ] = '\0';
      }
      else
      {
        stringliteral [ stringlitindex ] = '"';
        stringliteral [ stringlitindex + 1 ] = '\0';
        stringlitindex = 0;
      }

      if ( strcmp ( getCurrentState (dfa) -> name , "TK_IDEN" ) == 0 )
      {
        started = TRUE;
        identifier [ idenindex ++ ] = c;
        identifier [ idenindex ] = '\0';
      }
      else if ( ( isalpha( c ) || isdigit( c )  || c == '_' ) && started )
      {
        identifier [ idenindex ++ ] = c;
        identifier [ idenindex ] = '\0';
      }
      else
      {
        started = FALSE;
        idenindex = 0;
      }
    }
    else
    {
      started = FALSE;
      idenindex = 0;
    }

  }
  fclose(f);
  fclose(p);
  return 0;
}
