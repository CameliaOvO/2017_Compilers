#
# makefile for TINY
# Borland C Version
# K. Louden 2/3/98
#

# Makefile modified for C minus
# Seonha Park

CC = gcc

CFLAGS = -Wall -g 

OBJS = y.tab.o lex.yy.o main.o util.o symtab.o analyze.o # code.o cgen.o

all: cminus

cminus: $(OBJS)
	$(CC) $(CFLAGS) -o cminus $(OBJS)

y.tab.o: cminus.y globals.h
	bison -d cminus.y --yacc
	$(CC) $(CFLAGS) -c y.tab.c

lex.yy.o: cminus.l scan.h util.h globals.h
	flex cminus.l
	$(CC) $(CFLAGS) -c lex.yy.c -lfl

main.o: main.c globals.h util.h scan.h parse.h analyze.h cgen.h
	$(CC) $(CFLAGS) -c main.c

util.o: util.c util.h globals.h
	$(CC) $(CFLAGS) -c util.c

scan.o: scan.c scan.h util.h globals.h
	$(CC) $(CFLAGS) -c scan.c

parse.o: parse.c parse.h scan.h globals.h util.h
	$(CC) $(CFLAGS) -c parse.c

symtab.o: symtab.c symtab.h
	$(CC) $(CFLAGS) -c symtab.c

analyze.o: analyze.c globals.h symtab.h analyze.h
	$(CC) $(CFLAGS) -c analyze.c

code.o: code.c code.h globals.h
	$(CC) $(CFLAGS) -c code.c

cgen.o: cgen.c globals.h symtab.h code.h cgen.h
	$(CC) $(CFLAGS) -c cgen.c

tm: tm.c
	$(CC) $(CFLAGS) -o tm tm.c

clean:
	-rm cminus
	-rm $(OBJS)

test: cminus
	-./cminus test.cm

