CFLAGS=-Wall -std=c99 -Wstrict-prototypes -g
CC=gcc
LD=gcc
tests: mauve ../testdir/runtests.sh
	cd ../testdir/; bash runtests.sh
	
mauve: sqlparser.o lex.yy.o memman.o rbtree.o
	$(LD) -o mauve $^
lex.yy.c: scn.l
	flex -iR scn.l
lex.yy.o: lex.yy.c sqlparser.h
sqlparser.o: sqlparser.c sqlparser.h rbtree.h memman.h
rbtree.o: rbtree.c rbtree.h memman.h

clean:
	rm -f *.o
	rm -f lex.yy.c
	rm -f mauve
