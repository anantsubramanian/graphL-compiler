CC=gcc
CFLAGS=-O3 -Wall -Wextra -o
OBJFLAG=-c -Wall -Wextra
LEXOUTPUT=lexer
PTOUTPUT=parse_table_generator
PRSOUTPUT=parser
PSTOUTPUT=parsetree_generator
RM=rm
ALLFILES=lexer parse_table_generator dfa_ops.o trie_ops.o \
				 lexer.o parse_table_generator.o linkedlist_ops.o \
				 parser parser.o stack_ops.o parsetree_generator.o \
         parsetree_generator parsetree_ops.o

all: lexer parse_table_generator parser parsetree_generator

lexer: lexer.o dfa_ops.o trie_ops.o
	$(CC) lexer.o dfa_ops.o trie_ops.o $(CFLAGS) $(LEXOUTPUT)

parse_table_generator: parse_table_generator.o trie_ops.o linkedlist_ops.o
	$(CC) parse_table_generator.o trie_ops.o linkedlist_ops.o $(CFLAGS) $(PTOUTPUT)

parser: parser.o trie_ops.o stack_ops.o linkedlist_ops.o
	$(CC) parser.o trie_ops.o stack_ops.o linkedlist_ops.o $(CFLAGS) $(PRSOUTPUT)

parsetree_generator: parsetree_generator.o trie_ops.o parsetree_ops.o
	$(CC) parsetree_generator.o trie_ops.o parsetree_ops.o $(CFLAGS) $(PSTOUTPUT)

lexer.o: lexer.c
	$(CC) $(OBJFLAG) lexer.c

parse_table_generator.o: parse_table_generator.c
	$(CC) $(OBJFLAG) parse_table_generator.c

parser.o: parser.c
	$(CC) $(OBJFLAG) parser.c

parsetree_generator.o: parsetree_generator.c
	$(CC) $(OBJFLAG) parsetree_generator.c

dfa_ops.o: headers/dfa_ops.c
	$(CC) $(OBJFLAG) headers/dfa_ops.c

trie_ops.o: headers/trie_ops.c
	$(CC) $(OBJFLAG) headers/trie_ops.c

linkedlist_ops.o:	headers/linkedlist_ops.c
	$(CC) $(OBJFLAG) headers/linkedlist_ops.c

stack_ops.o: headers/stack_ops.c
	$(CC) $(OBJFLAG) headers/stack_ops.c

parsetree_ops.o: headers/parsetree_ops.c
	$(CC) $(OBJFLAG) headers/parsetree_ops.c

clean:
	$(RM) $(ALLFILES)

