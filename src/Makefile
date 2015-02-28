CC=gcc
CFLAGS=-O3 -o
OBJFLAG=-c
LEXOUTPUT=lexer
PTOUTPUT=parse_table_generator
RM=rm
ALLFILES=lexer parse_table_generator dfa_ops.o trie_ops.o \
				 lexer.o parse_table_generator.o linkedlist_ops.o

all: lexer parse_table_generator

lexer: lexer.o dfa_ops.o trie_ops.o
	$(CC) lexer.o dfa_ops.o trie_ops.o $(CFLAGS) $(LEXOUTPUT)

parse_table_generator: parse_table_generator.o trie_ops.o linkedlist_ops.o
	$(CC) parse_table_generator.o trie_ops.o linkedlist_ops.o $(CFLAGS) $(PTOUTPUT)

lexer.o: lexer.c
	$(CC) $(OBJFLAG) lexer.c

parse_table_generator.o: parse_table_generator.c
	$(CC) $(OBJFLAG) parse_table_generator.c

dfa_ops.o: headers/dfa_ops.c
	$(CC) $(OBJFLAG) headers/dfa_ops.c

trie_ops.o: headers/trie_ops.c
	$(CC) $(OBJFLAG) headers/trie_ops.c

linkedlist_ops.o:	headers/linkedlist_ops.c
	$(CC) $(OBJFLAG) headers/linkedlist_ops.c

clean:
	$(RM) $(ALLFILES)

