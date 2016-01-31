CC=gcc
CFLAGS=-O3 -Wall -Wextra -o
OBJFLAG=-c -Wall -Wextra
LEXOUTPUT=lexer
PTOUTPUT=parse_table_generator
PRSOUTPUT=parser
PSTOUTPUT=parsetree_generator
ASTOUTPUT=ast_generator
CGROUTPUT=code_generator
RM=rm
ALLFILES=lexer parse_table_generator dfa_ops.o trie_ops.o \
         lexer.o parse_table_generator.o linkedlist_ops.o \
         parser parser.o stack_ops.o parsetree_generator.o \
         parsetree_generator parsetree_ops.o ast_ops.o \
         ast_generator ast_generator.o symboltable_ops.o \
         code_generator.o code_generator constants.o \
         parse_utils.o ast_construction_ops.o register_ops.o \
         semantic_analyzer.o

all: lexer parse_table_generator parser parsetree_generator \
     ast_generator code_generator

lexer: lexer.o dfa_ops.o trie_ops.o
	$(CC) lexer.o dfa_ops.o trie_ops.o $(CFLAGS) $(LEXOUTPUT)

parse_table_generator: parse_table_generator.o trie_ops.o \
                       linkedlist_ops.o parse_utils.o
	$(CC) parse_table_generator.o trie_ops.o linkedlist_ops.o \
        parse_utils.o $(CFLAGS) $(PTOUTPUT)

parser: parser.o trie_ops.o stack_ops.o linkedlist_ops.o \
        parse_utils.o
	$(CC) parser.o trie_ops.o stack_ops.o linkedlist_ops.o \
        parse_utils.o $(CFLAGS) $(PRSOUTPUT)

parsetree_generator: parsetree_generator.o trie_ops.o parsetree_ops.o \
                     parse_utils.o
	$(CC) parsetree_generator.o trie_ops.o parsetree_ops.o parse_utils.o \
        $(CFLAGS) $(PSTOUTPUT)

ast_generator: ast_generator.o trie_ops.o stack_ops.o linkedlist_ops.o \
               symboltable_ops.o ast_ops.o constants.o parse_utils.o \
               ast_construction_ops.o
	$(CC) ast_generator.o trie_ops.o stack_ops.o linkedlist_ops.o symboltable_ops.o \
        ast_ops.o constants.o parse_utils.o ast_construction_ops.o \
        $(CFLAGS) $(ASTOUTPUT)

code_generator: code_generator.o ast_ops.o stack_ops.o symboltable_ops.o \
                trie_ops.o linkedlist_ops.o constants.o parse_utils.o \
                register_ops.o semantic_analyzer.o
	$(CC) code_generator.o ast_ops.o stack_ops.o symboltable_ops.o trie_ops.o \
        linkedlist_ops.o constants.o parse_utils.o register_ops.o \
        semantic_analyzer.o $(CFLAGS) $(CGROUTPUT)

constants.o: headers/constants.h headers/constants.c
	$(CC) $(OBJFLAG) headers/constants.c

parse_utils.o: headers/parse_utils.h headers/parse_utils.c
	$(CC) $(OBJFLAG) headers/parse_utils.c

lexer.o: lexer.c
	$(CC) $(OBJFLAG) lexer.c

parse_table_generator.o: parse_table_generator.c
	$(CC) $(OBJFLAG) parse_table_generator.c

parser.o: parser.c
	$(CC) $(OBJFLAG) parser.c

parsetree_generator.o: parsetree_generator.c
	$(CC) $(OBJFLAG) parsetree_generator.c

ast_generator.o: ast_generator.c
	$(CC) $(OBJFLAG) ast_generator.c

code_generator.o: code_generator.c
	$(CC) $(OBJFLAG) code_generator.c

dfa_ops.o: headers/dfa_ops.c headers/dfa.h
	$(CC) $(OBJFLAG) headers/dfa_ops.c

trie_ops.o: headers/trie_ops.c headers/trie.h
	$(CC) $(OBJFLAG) headers/trie_ops.c

linkedlist_ops.o: headers/linkedlist_ops.c headers/linkedlist.h
	$(CC) $(OBJFLAG) headers/linkedlist_ops.c

stack_ops.o: headers/stack_ops.c headers/stack.h
	$(CC) $(OBJFLAG) headers/stack_ops.c

parsetree_ops.o: headers/parsetree_ops.c headers/parsetree.h
	$(CC) $(OBJFLAG) headers/parsetree_ops.c

symboltable_ops.o: headers/symboltable_ops.c headers/symboltable.h
	$(CC) $(OBJFLAG) headers/symboltable_ops.c

ast_ops.o: headers/ast_ops.c headers/ast.h
	$(CC) $(OBJFLAG) headers/ast_ops.c

ast_construction_ops.o: headers/ast_construction.h headers/ast_construction_ops.c
	$(CC) $(OBJFLAG) headers/ast_construction_ops.c

register_ops.o: headers/register.h headers/register_ops.c
	$(CC) $(OBJFLAG) headers/register_ops.c

semantic_analyzer.o: semantic_analyzer.c
	$(CC) $(OBJFLAG) semantic_analyzer.c

clean:
	$(RM) $(ALLFILES)

