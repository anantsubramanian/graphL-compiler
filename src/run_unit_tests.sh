echo "Running Unit Tests:"

# Recompile to check against latest version
make

# Clean files from previous runs
rm unit-testing/program*_produced 2>/dev/null

# Decide number of sample programs
echo -n "Enter the number of test programs in the example folder: "
read numcases

# Run required number of tests
for (( i = 1; i <= $numcases; i++ )); do
  echo -n "Running test $i... "
  ./lexer < example/program${i}.G 2>/dev/null > unit-testing/program${i}_produced
  diffres=$(diff Tokens.txt unit-testing/program${i}_output)
  if [ "$diffres" != "" ]; then
    echo "FAILED!"
    echo "Differences listed below: "
    diff unit-testing/program${i}_produced unit-testing/program${i}_output
    exit
  fi
  echo "PASSED!"
  rm unit-testing/program${i}_produced
done

# Done with all unit tests
echo "All unit tests passed. Proceed to commit"
