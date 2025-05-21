CC = gcc
CFLAGS = -Wall -g -Iinclude

all: compiler

compiler: lexer.o parser.o main.o ast.o ir.o
	$(CC) $(CFLAGS) -o compiler lexer.o parser.o main.o ast.o ir.o

# Bison generates both files
parser.tab.h parser.c: parser.y
	bison -d -o parser.c parser.y

# Compile parser.c (after parser.c exists)
parser.o: parser.c
	$(CC) $(CFLAGS) -c parser.c

# Flex should only run AFTER parser.tab.h exists
lexer.c: lexer.l parser.tab.h
	flex -o lexer.c lexer.l

# Compile lexer.c (after lexer.c exists)
lexer.o: lexer.c
	$(CC) $(CFLAGS) -c lexer.c

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

ast.o: src/ast.c
	$(CC) $(CFLAGS) -c src/ast.c -o ast.o

ir.o: src/ir.c include/ir.h include/ast.h
	$(CC) $(CFLAGS) -c src/ir.c -o ir.o

clean:
	rm -f *.o parser.c parser.h lexer.c compiler
