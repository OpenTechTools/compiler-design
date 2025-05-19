CC = gcc

all: compiler

compiler: lexer.o parser.o main.o
	$(CC) -o compiler lexer.o parser.o main.o

# Bison generates both files
parser.tab.h parser.c: parser.y
	bison -d -o parser.c parser.y

# Compile parser.c (after parser.c exists)
parser.o: parser.c
	$(CC) -c parser.c

# Flex should only run AFTER parser.tab.h exists
lexer.c: lexer.l parser.tab.h
	flex -o lexer.c lexer.l

# Compile lexer.c (after lexer.c exists)
lexer.o: lexer.c
	$(CC) -c lexer.c

main.o: main.c
	$(CC) -c main.c

clean:
	rm -f *.o *.c *.h compiler
