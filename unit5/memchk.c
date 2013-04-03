
#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "except.h"
#include "mem.h"

//<checking types 58>
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

//<checking macros 58>
#define hash(p, t) (((unsigned long)(p) >>　3) & (sizeof(t) / sizeof((t)[0]) - 1))
#define NDESCRIPTORS 512
#define NALLOC ((4096 + sizeof(union align) - 1)/(sizeof(union align))) \
                * (sizeof(union align))

//<data 54>
const Except_T Mem_Failed = {"Allocation Failed"};

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
        //<set bp if ptr is valid 58>
        if (((unsigned long)ptr)%(sizeof(union align)) != 0
            || (bp = find(ptr)) == NULL
            || bp->free) {
            Except_raise(&Assert_Failed, file, line);
        }
        bp->free = freelist.free;
        freelist.free = bp;
        // 上面代码并没有修改htab中节点bp的link信息，也即htab保存有空闲块和非空闲块。
    }
}

void *Mem_resize(void *ptr, long nbytes, const char *file, int line)
{
    struct descriptor *bp;
    void *new_ptr;

    assert(ptr);
    assert(nbytes > 0);
    //<set bp if ptr is valid 58>
    if (((unsigned long)ptr)%(sizeof(union align)) != 0
            || (bp = find(ptr)) == NULL
            || bp->free) {
            Except_raise(&Assert_Failed, file, line);
    }
    new_ptr = Mem_alloc(nbytes, file, line);
    memcpy(new_ptr, ptr, nbytes < bp->size ? nbytes : bp->size);
    Mem_free(ptr, file, line);
    return new_ptr;
}


void *Mem_calloc(long count, long nbytes, const char *file, int line)
{
    void *ptr;

    assert(count > 0);
    assert(nbytes > 0);
    ptr = Mem_alloc(count*nbytes, file, line);
    memset(ptr, '\0', count*nbytes);
    return ptr;
}

static struct descriptor *dalloc(void *ptr, long size, const char *file, int line)
{
    static struct descriptor *avail;
    static int nleft;

    if (nleft <= 0) {
        //<allocate descriptors 60>
        avail = malloc(NDESCRIPTORS * sizeof(*avail));
        if (avail == NULL) {
            return NULL;
        }
        nleft = NDESCRIPTORS;
    }
    avail->ptr = ptr;
    avail->size = size;
    avail->file = file;
    avail->line = line;
    avail->free = avail->link = NULL;
    nleft--;
    return avail++;
}

void *Mem_alloc(long nbytes, const char *file, int line)
{
    struct descriptor *bp;
    void *ptr;

    assert(nbytes > 0);
    //<round nbytes up to an alignment boundary 61>
    //将nbytes向上舍入，使得其返回的每个指针都对齐到联合align大小的倍数:((x + y - 1)/y) * y
    nbytes = ((nbytes + sizeof(union align) -1)/(sizeof(union align))) * sizeof(union align);
    for (bp = freelist.free; bp; bp = bp->free) {
        if (bp->size > nbytes) {
            //<use the end of the block at bp->ptr 61>
            bp->size -= nbytes;
            ptr = (char *)bp->ptr + bp->size;
            if ((bp = dalloc(ptr, nbytes, file, line) != NULL) {
                unsigned h = hash(ptr, htab);
                bp->link = htab[h];
                htab[h] = bp;
                return ptr;
            } else {
                //<raise Mem_Failed 54>
                if (file == NULL) {
                    RAISE(Mem_Failed);
                } else {
                    Except_raise(&Mem_Failed, file, line);
                }
            }
        }
        if (bp == &freelist) {
            struct descriptor *new_ptr;
            //<new_ptr <- a block of size NALLOC + nbytes 62>
            if ((ptr = malloc(nbytes + NALLOC)) == NULL
                || (new_ptr = dalloc(ptr, nbytes+NALLOC, __FILE__, __LINE__)) == NULL) {
                //<raise Mem_Failed 54>
                if (file == NULL) {
                    RAISE(Mem_Failed);
                } else {
                    Except_raise(&Mem_Failed, file, line);
                }
            }
            new_ptr->free = freelist.free;
            freelist.free = new_ptr;
        }
    }
    assert(0);
    return NULL;
}
