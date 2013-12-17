
CC=gcc
LD=gcc
CFLAGS=-Wall -std=c99 -g
alltests:  ../src/mauve
	./runtests.sh
../src/mauve:
	make -C ../src/ mauve

testtree.exe: testtree.o ../src/rbtree.o ../src/memman.o
	$(LD) -o $@ $^
testtree.o: ../src/rbtree.h testtree.c

clean:
	rm *.o *.exe
