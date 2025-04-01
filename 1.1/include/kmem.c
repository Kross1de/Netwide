#include "vfs.h"

#define HEAP_SIZE 1024 * 1024 //1MB heap
static unsigned char heap[HEAP_SIZE];
static size_t heapptr = 0;

void *kmalloc(size_t size){
    if(heapptr + size > HEAP_SIZE){
        return NULL;
    }
    void *ptr = &heap[heapptr];
    heapptr += size;
    return ptr;
}

void kfree(void *ptr){
    (void)ptr;
}