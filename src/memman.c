#define _XOPEN_SOURCE 500
#include "memman.h"
#include <unistd.h>
#include <assert.h>

struct mymemory{
	char * startofmem;
	char * nexttoalloc;
	size_t sizeofmemory;
} mymem;
struct mymemory *mem = NULL;

void *heap_alloc(size_t numbytes){
	numbytes = (numbytes+7)&(-8);/* make it at least 8 byte increments */
	assert(numbytes && "heap_alloc called for nothing");
	if (!mem){
		mem = &mymem;
		mem->sizeofmemory = 64 * 1024 *1024;
		mem->startofmem = sbrk(mem->sizeofmemory);
		mem->nexttoalloc = mem->startofmem;
	}
	if (mem->nexttoalloc + numbytes > mem->startofmem + mem->sizeofmemory){
		void *newmem = sbrk(64 * 1024 *1024);
		if (mem->nexttoalloc >= mem->sizeofmemory + mem->startofmem){
			mem->nexttoalloc = newmem;
			mem->sizeofmemory = mem->nexttoalloc - mem->startofmem;
		}
		mem->sizeofmemory += 64 * 1024 *1024;
	}
	void *rmem = mem->nexttoalloc;
	mem->nexttoalloc += numbytes;
	return rmem;
}
