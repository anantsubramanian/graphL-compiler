if [ "$1" == "" ]; then
  printf "Provide the input program as an argument\n"
  exit
fi

make
./lexer $1 2>/dev/null
e=$(cat ERRORS)
if [ "$e" != "" ]; then
  echo "Lexing failed!"
  exit
fi
./parser 2>/dev/null $1
e=$(cat PARSEERRORS)
if [ "$e" != "" ]; then
  echo "Parsing failed!"
  exit
fi
./ast_generator >/dev/null

printf "Done!\n"
