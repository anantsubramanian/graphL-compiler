if [ "$1" == "" ]; then
  printf "Provide the input program as an argument\n"
  exit
fi

make
./lexer $1 2>/dev/null
./parser 2>/dev/null $1
./ast_generator

printf "Done!\n"
