CC=gcc
CFLAGS=-O3 -o
OBJFLAG=-c
OUTPUT=lexer
RM=rm
ALLFILES=lexer dfa_ops.o trie_ops.o lexer.o

lexer: lexer.o dfa_ops.o trie_ops.o
	$(CC) lexer.o dfa_ops.o trie_ops.o $(CFLAGS) $(OUTPUT)

lexer.o: lexer.c
	$(CC) $(OBJFLAG) lexer.c

dfa_ops.o: headers/dfa_ops.c
	$(CC) $(OBJFLAG) headers/dfa_ops.c

trie_ops.o: headers/trie_ops.c
	$(CC) $(OBJFLAG) headers/trie_ops.c

clean:
	$(RM) $(ALLFILES)

