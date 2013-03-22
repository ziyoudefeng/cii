
#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "except.h"
#include "mem.h"

<checking types 58>
//<checking macros 58>
#define hash(p, t) (((unsigned long)(p) >>　3) & (sizeof(t) / sizeof((t)[0]) - 1))

<data 54>


//<checking data 56>
static struct descriptor {
    struct descriptor *free;
    struct descriptor *link; //构建了一个块描述符的链表，这些块散列到htab中的同一索引。
    const void *ptr;
    long size;
    const char *file;
    int line;
} *htab[2048];

static struct descriptor freelist = {&freelist};

//<checking functions 58>
static struct descriptor *find(const void *ptr)
{
    struct descriptor *bp = htab[hash(ptr, htab)];

    while (bp && bp->ptr != ptr) {
        bp = bp->link;
    }
    return bp;
}

void Mem_free(void *ptr, const char *file, int line)
{
    if (ptr) {
        struct descriptor *bp;
        <set bp if ptr is valid 58>
        bp->free = freelist.free;
        freelist.free = bp;
    }
}

