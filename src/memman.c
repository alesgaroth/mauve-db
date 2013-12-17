#include "memman.h"

void *heap_alloc(size_t numbytes){
	return malloc(numbytes);
}
