CFLAGS=-Wall -std=c99 -Wstrict-prototypes
CC=gcc
LD=gcc
mauve:sqlparser.o lex.yy.o
	$(LD) -o mauve $^
lex.yy.c:
	flex -iR scn.l
lex.yy.o: lex.yy.c sqlparser.h
sqlparser.o: sqlparser.c sqlparser.h

clean:
	rm -f *.o
	rm -f lex.yy.c
