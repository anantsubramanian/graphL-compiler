printf "\nRunning Unit Tests:\n\n"

# Recompile to check against latest version
make

# Clean files from previous runs
rm TOKENS TOKENMAP ERRORS 2>/dev/null

# Decide number of sample programs
printf "\nEnter the number of test programs in the testing folder: "
read numcases

##############################################
#           Starting Lexer Tests             #
##############################################

printf "\n\n-------------------------------------------------------\n"
printf "Testing module lexer:"
printf "\n-------------------------------------------------------\n\n"

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

printf "Test Parse Table generator? (y = yes): "
read yn

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

# Decide number of sample programs
printf "\nEnter the number of error-free programs in the testing folder: "
read numcases

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
    ./parser unit-testing/parser_program${i}.G >/dev/null
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

# Done with all unit tests
printf "\nAll unit tests passed. Proceed to commit.\n\n"

# Cleanup
printf "Cleaning up temporary files..\n"
make clean

printf "\n\n"

