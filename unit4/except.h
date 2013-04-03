
#ifndef EXCEPT_INCLUDED
#define EXCEPT_INCLUDED
#include <setjmp.h>

#define T Except_T
typedef struct T {
    const char *reason;
} T;

//<exported types 39>
typedef struct Except_Frame Except_Frame;
struct Except_Frame {
    Except_Frame *prev;
    jmp_buf env;
    const char *file;
    int line;
    const T *exception;
};

enum {
    Except_entered = 0,/*第一次调用setjmp*/
    Except_raised,/*表示发生异常*/
    Except_handled,/*处理程序:Except_flag = Except_handled表示已经处理了该异常 */
    Except_finalized
};


//<exported variables 39>
extern Except_Frame *Except_stack;


//<exported functions 35>
void Excpet_raise(const T *e, const char *file, int line);

//<exported macros 35>
#define RAISE(e) Except_raise(&(e), __FILE__, __LINE__)
#define RERAISE Except_raise(Except_frame.exception, Except_frame.file, Except)frame.line)
#define RETURN switch (Except_stack = Except_stack->prev, 0) default: return

#define TRY do { \
    volatile int Except_flag; \
    Except_Frame Except_frame; \
    Except_frame.prev = Except_stack; \
    Except_stack = &Except_Frame; \
    Except_flag = setjmp(Except_frame.env); \
    if (Except_flag == Except_entered) {

#define EXCEPT(e) \
        if (Except_flag == Except_entered) { \
            Except_stack = Except_stack->prev; \
        } \
    } else if (Except_frame.exception == &(e)) { \
        Except_flag = Except_handled;

#define ELSE \
        if (Except_flag == Except_entered) { \
            Except_stack = Except_stack->prev; \
        } \
    } else { \
        Except_flag = Except_handled;

#define FINALLY \
        if (Except_flag == Except_entered) { \
            Except_stack = Except_stack->prev; \
        } \
    } { \
        if (Except_flag == Except_entered) \
            Except_flag = Except_finalized;

#define END_TRY \
            if (Except_flag == Except_entered) { \
                Except_stack = Except_stack->prev; \
            } \
        } if (Except_flag == Except_raised) RERAISE; \
} while (0)

#undef T
#endif
