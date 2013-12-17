CFLAGS=-Wall -std=c99 -Wstrict-prototypes
CC=gcc
LD=gcc
tests: mauve ../testdir/runtests.sh
	cd ../testdir/; bash runtests.sh
	
mauve:sqlparser.o lex.yy.o
	$(LD) -o mauve $^
lex.yy.c:
	flex -iR scn.l
lex.yy.o: lex.yy.c sqlparser.h
sqlparser.o: sqlparser.c sqlparser.h
rbtree.o: rbtree.c rbtree.h memman.h

clean:
	rm -f *.o
	rm -f lex.yy.c
	rm -f mauve
