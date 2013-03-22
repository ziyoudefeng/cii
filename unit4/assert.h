
#include <stdio.h>
#include <stdlib.h>
#undef assert
#ifdef NDEBUG
#define assert(e) ((void)0)
#else
extern void assert(int e);
#define assert(e) ((void)((e) || \
                   (RAISE(Assert_Failed), \
                    0))) \

#endif

//<exported variables 39>
extern const Except_T Assert_Failed;



