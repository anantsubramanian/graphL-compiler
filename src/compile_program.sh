# Script that takes as input either a Graph-L language program
# or a NASM assembly program, and converts it into an executable
# file.
# Argument 1 = <program_file>
# Argument 2 = [Option]
#              -l   - Stop after lexer
#              -p   - Stop after parser
#              -ast - Stop after AST generator
#              -asm - Stop after Assembly code generation
#
# If no argument 2 is provided, the input is compiled in full to
# an executable

if [ "$1" == "" ]; then
  printf "Provide the input program as an argument\n"
  exit
fi

printf "\n"

make >/dev/null

name=$(echo $1 | cut -d '.' -f 1)
ext=$(echo $1 | cut -d '.' -f 2)
asmfile="ASMFILE"

if [ "$ext" == "G" ]; then
  ./lexer $1 2>/dev/null
  e=$(cat ERRORS)
  if [ "$e" != "" ]; then
    printf "Errors found while compiling:\n\n"
    cat ERRORS
    exit
  fi

  # Stop at lexer
  if [ "$2" == "-l" ]; then
    exit
  fi

  ./parser 2>/dev/null $1
  e=$(cat PARSEERRORS)
  if [ "$e" != "" ]; then
    printf "Errors found while compiling:\n\n"
    cat PARSEERRORS
    exit
  fi

  # Stop at parser
  if [ "$2" == "-p" ]; then
    exit
  fi

  ./ast_generator >/dev/null
  if [ "$?" != "0" ]; then
    exit
  fi

  # Stop at AST generator
  if [ "$2" == "-ast" ]; then
    exit
  fi

  ./code_generator >/dev/null
  if [ "$?" != "0" ]; then
    exit
  fi

  # Stop at assembly code
  if [ "$2" == "-asm" ]; then
    mv ASMFILE.asm ${name}.asm
    make clean >/dev/null
    rm ASTDUMP ASTOUTPUT ERRORS PARSEERRORS PARSEOUTPUT STBDUMP TOKENMAP TOKENS ASMFILE.asm CODEFILE FUNCTIONFILE VARFILE 2>/dev/null
    exit
  fi
elif [ "$ext" == "asm" ]; then
  asmfile=$name
else
  echo "Unrecognized file extension"
fi

nasm -f elf ${asmfile}.asm
gcc -m32 ${asmfile}.o -o ${name} -nostartfiles
#ld -m elf_i386 ${asmfile}.o -o ${name} -lc
rm ${asmfile}.o

make clean >/dev/null
rm ASTDUMP ASTOUTPUT ERRORS PARSEERRORS PARSEOUTPUT STBDUMP TOKENMAP TOKENS ASMFILE.asm CODEFILE FUNCTIONFILE VARFILE 2>/dev/null

echo "Compiled to $name"

