
#ifndef MEM_INCLUDED
#define MEM_INCLUDED

#include "except.h"


//<exported exceptions 51>
extern const Except_T Mem_Failed;

//<exported functions 51>
// 使用long类型，而不是标准库里面的size_t，即unsigned int/unsigned long，
// 是为了避免将负数传递给芜湖好参数可能造成的错误。
extern void *Mem_alloc(long nbytes, const char *file, int line); //分配的内存未初始化，个人理解这样不安全，会不会泄露信息？
extern void *Mem_calloc(long count, long nbytes, const char *file, int line);
extern void Mem_free(void *ptr, const char *file, int line);

extern void *Mem_resize(void *ptr, long nbytes, const char *file, int line);

//<exported macros 51>
#define ALLOC(nbytes) \
    Mem_alloc((nbytes), __FILE__, __LINE__)
#define CALLOC(count, nbytes) \
    Mem_calloc((count), (nbytes), __FILE, __LINE__)

// FREE(ptr)会多次对ptr进行求值。
#define FREE(ptr) ((void)(Mem_free((ptr), __FILE__, __LINE__), \
                          (ptr) = 0))
// RESIZE(ptr, nbytes)会多次对ptr进行求值。
#define RESIZE(ptr, nbytes) ((ptr) = Mem_resize((ptr), (nbytes), __FILE, __LINE__))

// 使用具有负效应的表达式作为这两个宏的实参是安全的，例如NEW(a[i++])。因为sizeof不会计算其参数值。
#define NEW(p) ((p) = ALLOC((long)sizeof *(p)))
#define NEW0(p) ((p) = CALLOC(1, (long)sizeof *(p)))

#endif
