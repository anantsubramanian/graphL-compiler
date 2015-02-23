echo "Running Unit Tests:"

# Recompile to check against latest version
make

# Clean files from previous runs
rm TOKENS TOKENMAP ERRORS 2>/dev/null

# Decide number of sample programs
echo -n "Enter the number of test programs in the example folder: "
read numcases

# Run required number of tests
for (( i = 1; i <= $numcases; i++ )); do

  echo -n "Running test $i... "
  ./lexer unit-testing/program${i}.G 2>/dev/null

  errors=$(cat ERRORS)
  filetouse="TOKENS"
  mapfile="TOKENMAP"
  if [ "$errors" != "" ]; then
    filetouse="ERRORS"
  fi

  diffres=$(diff ${filetouse} unit-testing/program${i}_output)
  if [ "$diffres" != "" ]; then
    echo "FAILED!"
    echo ""
    echo "Differences listed below: "
    diff ${filetouse} unit-testing/program${i}_output
    exit
  fi
  diffres2=$(diff ${mapfile} unit-testing/program${i}_map)
  if [ "$diffres2" != "" ]; then
    echo "FAILED!"
    echo ""
    echo "Differences in MAP file: "
    diff ${mapfile} unit-testing/program${i}_map
    exit
  fi
  echo "PASSED!"
  rm TOKENS ERRORS TOKENMAP
done

# Done with all unit tests
echo "All unit tests passed. Proceed to commit"

# Cleanup
echo "Cleaning up temporary files"
make clean

