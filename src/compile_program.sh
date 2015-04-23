# Script that takes as input either a Graph-L language program
# or a NASM assembly program, and converts it into an executable
# file.
# Argument 1 = <program_file>
# Argument 2 = [Option]
#              l  - Stop after lexer
#              p  - Stop after parser
#              ag - Stop after AST generator
#              as - Stop after Assembly code generation
#
# If no argument 2 is provided, the input is compiled in full to
# an executable

if [ "$1" == "" ]; then
  printf "Provide the input program as an argument\n"
  exit
fi

make >/dev/null

name=$(echo $1 | cut -d '.' -f 1)
ext=$(echo $1 | cut -d '.' -f 2)
asmfile="ASMFILE"

if [ "$ext" == "G" ]; then
  ./lexer $1 2>/dev/null
  e=$(cat ERRORS)
  if [ "$e" != "" ]; then
    cat ERRORS
    exit
  fi

  # Stop at lexer
  if [ "$2" == "l" ]; then
    exit
  fi

  ./parser 2>/dev/null $1
  e=$(cat PARSEERRORS)
  if [ "$e" != "" ]; then
    cat PARSEERRORS
    exit
  fi

  # Stop at parser
  if [ "$2" == "p" ]; then
    exit
  fi

  ./ast_generator >/dev/null
  if [ "$?" != "0" ]; then
    exit
  fi

  # Stop at AST generator
  if [ "$2" == "ag" ]; then
    exit
  fi

  ./code_generator >/dev/null
  if [ "$?" != "0" ]; then
    exit
  fi

  # Stop at assembly code
  if [ "$2" == "as" ]; then
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
rm ASTDUMP ASTOUTPUT ERRORS PARSEERRORS PARSEOUTPUT STBDUMP TOKENMAP TOKENS ASMFILE.asm CODEFILE FUNCTIONFILE VARFILE

echo "Compiled to $name"

