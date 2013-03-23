
#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "except.h"
#include "arena.h"
#define T Arena_T

const Except_T Arena_NewFailed = {"Arena Creation Failed"};
const Except_T Arena_Failed = {"Arena Allocation Failed"};

<macros 71>

//<types 67>
struct T {
    T prev;
    char *avail;
    char *limit;
};

union align {
    int i;
    long l;
    long *lp;
    void *p;
    void (*fp)(void);
    float f;
    double d;
    long double ld;
};

union header {
    struct T b;
    union align a;
};

<data 70>

//<functions 68>
T Arena_new(void)
{
    T arena = malloc(sizeof(*arena));
    if (arena == NULL) {
        RAISE(Arena_NewFailed);
    }
    arena->prev = NULL;
    arena->limit = arena->avail = NULL;
    return arena;
}

void Arena_dispose(T *ap)
{
    assert(ap && *ap);
    Arena_free(*ap);
    free(*ap);
    *ap = NULL;
}

void *Arena_alloc(T arena, long nbytes, const char *file, int line)
{
    assert(arena);
    assert(nbytes > 0);
    //<round nbytes up to an alignment boundary 69>
    nbytes = ((nbytes + sizeof(union align) - 1)/sizeof(union align)) / (sizeof(union align));
    while (nbytes > arena->limit - arena->avail) {
        //<get a new chunk 69>
        T ptr;
        char *limit;
        <ptr <- a new chunk 70>
        *ptr = *arena;
        arena->avail = (char *)((union header *)ptr + 1);
        arena->limit = limit;
        arena->prev = prev;

    }
    arena->avail += nbytes;
    return arena->avail - nbytes;
}
