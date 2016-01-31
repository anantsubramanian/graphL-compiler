# TODO: Add unit tests for Code Generator

# If insufficient command line arguments are provided
# display arguments information
if [[ "$5" == "" ]]; then
  printf "\n\nTo avoid prompts during testing, considering running "
  printf "the script in the following way:\n\n"
  printf "./run_unit_tests.sh numlex generatept? numparse numpst numast\n"
  printf "\nwhere:\n\tnumlex = Number of lexer test programs\n"
  printf "\tgeneratept? = 'y' if parse table needs to be generated/tested\n"
  printf "\tnumparse = Number of parser test programs\n"
  printf "\tnumpst = Number of Parse Tree generator test programs\n"
  printf "\tnumast = Number of Abstract Syntax Tree generator test programs\n"
fi

# Recompile to check against latest version
printf "\n\n-------------------------------------------------------\n"
printf "Compiling all modules:"
printf "\n-------------------------------------------------------\n\n"
make all

# Start running unit tests
printf "\n\n-------------------------------------------------------\n"
printf "Running Unit Tests:"
printf "\n-------------------------------------------------------\n\n"

# Clean files from previous runs
rm TOKENS TOKENMAP ERRORS ASTOUTPUT PARSEERRORS PARSEOUTPUT 2>/dev/null

if [[ "$1" == "" ]]; then
  # Decide number of sample programs
  printf "\nEnter the number of lexer test programs in the testing folder: "
  read numcases
else
  # Argument 1 is the number of lexer test cases
  numcases=$1
fi

##############################################
#           Starting Lexer Tests             #
##############################################

printf "\n\n-------------------------------------------------------\n"
printf "Testing module lexer:"
printf "\n-------------------------------------------------------\n"

# Run required number of tests
for (( i = 1; i <= $numcases; i++ )); do

  printf "Running test $i... "
  ./lexer unit-testing/program${i}.G 2>/dev/null

  errors=$(cat ERRORS)
  filetouse="TOKENS"
  mapfile="TOKENMAP"
  if [ "$errors" != "" ]; then
    filetouse="ERRORS"
  fi

  diffres=$(diff ${filetouse} unit-testing/program${i}_output)
  if [ "$diffres" != "" ]; then
    printf "FAILED!\n\n"
    printf "Differences listed below:\n"
    diff ${filetouse} unit-testing/program${i}_output
    exit
  fi
  diffres2=$(diff ${mapfile} unit-testing/program${i}_map)
  if [ "$diffres2" != "" ]; then
    printf "FAILED!\n\n"
    printf "Differences in MAP file:\n"
    diff ${mapfile} unit-testing/program${i}_map
    exit
  fi
  printf "PASSED!\n"
  rm TOKENS ERRORS TOKENMAP
done

printf "All tests passed!"
printf "\n\n-------------------------------------------------------\n"
printf "Testing module Lexer complete."
printf "\n-------------------------------------------------------\n\n"

##############################################
#         Starting Parse Table Tests         #
##############################################

if [[ "$2" == "" ]]; then
  printf "Test Parse Table generator? (y = yes): "
  read yn
else
  # Argument 2 is y or n for parse table generator tests
  yn=$2
fi

if [[ "$yn" == "y" ]]; then

  printf "\n\n-------------------------------------------------------\n"
  printf "Testing module Parse Table generator:"
  printf "\n-------------------------------------------------------\n\n"

  rm 2>/dev/null config/nonterminals_index terminals_index parse_table
  ./parse_table_generator 2>/dev/null

  printf "Testing terminals index mapping... "
  tmapdiff=$(diff config/terminals_index unit-testing/terminals_index)

  if [[ "$tmapdiff" != "" ]]; then
    printf "FAILED!\n\n"
    printf "Differences listed below:\n"
    diff config/terminals_index unit-testing/terminals_index
    exit
  fi
  printf "PASSED!\n"

  printf "Testing non-terminals index mapping... "
  ntmapdiff=$(diff config/nonterminals_index unit-testing/nonterminals_index)

  if [[ "$ntmapdiff" != "" ]]; then
    printf "FAILED!\n\n"
    printf "Differences listed below:\n"
    diff config/nonterminals_index unit-testing/nonterminals_index
    exit
  fi
  printf "PASSED!\n"

  printf "Testing rules file... "
  rulediff=$(diff config/rules_file unit-testing/rules_file)

  if [[ "$rulediff" != "" ]]; then
    printf "FAILED!\n\n"
    printf "Differences listed below:\n"
    diff config/rules_file unit-testing/rules_file
    exit
  fi
  printf "PASSED!\n"

  printf "Testing parse table... "
  ptdiff=$(diff config/parse_table unit-testing/parse_table)

  if [[ "$ptdiff" != "" ]]; then
    printf "FAILED!\n\n"
    printf "Differences listed below:\n"
    diff config/parse_table unit-testing/parse_table
    exit
  fi
  printf "PASSED!\n"

  printf "All tests passed!"
  printf "\n\n-------------------------------------------------------\n"
  printf "Testing module Parse Table generator complete."
  printf "\n-------------------------------------------------------\n\n"

else

  printf "Not testing module Parse Table generator\n\n"

fi

##############################################
#           Starting Parser Tests            #
##############################################

if [[ "$3" == "" ]]; then
  # Decide number of sample programs
  printf "\nEnter the number of parser test programs in the testing folder: "
  read numcases
else
  # Argument 3 is the number of parser test programs
  numcases=$3
fi

printf "\n\n-------------------------------------------------------\n"
printf "Testing module parser:"
printf "\n-------------------------------------------------------\n\n"

# Run required number of tests
for (( i = 1; i <= $numcases; i++ )); do

  printf "Running test $i... "
  cp unit-testing/parser_test${i}_tokens TOKENS
  cp unit-testing/parser_test${i}_map TOKENMAP

  ./parser 2>/dev/null unit-testing/parser_program${i}.G

  errors=$(cat PARSEERRORS)
  filetouse="PARSEOUTPUT"
  if [ "$errors" != "" ]; then
    filetouse="PARSEERRORS"
  fi

  comp=$(diff $filetouse unit-testing/parser_test${i}_output)

  if [[ "$comp" != "" ]]; then
    printf "FAILED!\n\n"
    printf "Errors reported:\n\n"
    diff unit-testing/parser_test${i}_output $filetouse
    printf "\n\n"
    exit
  fi

  printf "PASSED!\n"

  rm TOKENS TOKENMAP PARSEOUTPUT PARSEERRORS
done

printf "All tests passed!"
printf "\n\n-------------------------------------------------------\n"
printf "Testing module Parser complete."
printf "\n-------------------------------------------------------\n\n"


##############################################
#    Starting Parse Tree Generator Tests     #
##############################################


if [[ "$4" == "" ]]; then
  # Decide number of sample programs
  printf "\nEnter the number of error-free parser tests in the testing folder: "
  read numcases
else
  # Argument 4 is the number of pst generator test programs
  numcases=$4
fi

printf "\n\n-------------------------------------------------------\n"
printf "Testing module Parse Tree generator:"
printf "\n-------------------------------------------------------\n\n"

# Run required number of tests
for (( i = 1; i <= $numcases; i++ )); do

  printf "Running test $i... "
  cp unit-testing/parser_test${i}_output PARSEOUTPUT

  result=$(./parsetree_generator 2>/dev/null)

  if [[ "$result" != "PARSETREE successfully built" ]]; then
    printf "FAILED!\n\n"
    printf "Errors reported:\n\n"
    ./parsetree_generator
    printf "\n\n"
    exit
  fi

  printf "PASSED!\n"

  rm PARSEOUTPUT
done

printf "All tests passed!"
printf "\n\n-------------------------------------------------------\n"
printf "Testing module Parse Tree Generator complete."
printf "\n-------------------------------------------------------\n\n"

##############################################
#        Starting AST Generator Tests        #
##############################################


if [[ "$5" == "" ]]; then
  # Decide number of sample programs
  printf "\nEnter the number of error-free parser tests in the testing folder (for AST): "
  read numcases
else
  # Argument 5 is the number of ast generator test programs
  numcases=$5
fi

printf "\n\n-------------------------------------------------------\n"
printf "Testing module AST generator:"
printf "\n-------------------------------------------------------\n\n"

# Run required number of tests
for (( i = 1; i <= $numcases; i++ )); do

  printf "Running test $i... "
  cp unit-testing/parser_test${i}_output PARSEOUTPUT
  cp unit-testing/parser_test${i}_map TOKENMAP

  ./ast_generator 2>/dev/null

  diffres=$(diff ASTOUTPUT unit-testing/ast_test${i}_output)
  diffres1=$(diff STBDUMP unit-testing/ast_test${i}_stbdump)

  if [[ "$diffres" != "" ]]; then
    printf "FAILED!\n\n"
    printf "Errors reported:\n\n"
    diff ASTOUTPUT unit-testing/ast_test${i}_output
    printf "\n\n"
    exit
  fi

  if [[ "$diffres1" != "" ]]; then
    printf "FAILED!\n\n"
    printf "Errors reported:\n\n"
    diff STBDUMP unit-testing/ast_test${i}_stbdump
    printf "\n\n"
    exit
  fi

  printf "PASSED!\n"

  rm PARSEOUTPUT ASTOUTPUT TOKENMAP STBDUMP ASTDUMP
done

printf "All tests passed!"
printf "\n\n-------------------------------------------------------\n"
printf "Testing module AST Generator complete."
printf "\n-------------------------------------------------------\n\n"

# Done with all unit tests
printf "\nAll unit tests passed. Proceed to commit.\n\n"

# Cleanup
printf "Cleaning up temporary files..\n"
make clean

printf "\n\n"

