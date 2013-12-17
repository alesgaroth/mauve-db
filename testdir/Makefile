
CC=gcc
LD=gcc
CFLAGS=-Wall -std=c99 -g
alltests: 
	./runtests.sh

testtree.exe: testtree.o ../src/rbtree.o ../src/memman.o
	$(LD) -o $@ $^
testtree.o: ../src/rbtree.h testtree.c ../src/memman.h
