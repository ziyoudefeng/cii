
#ifndef LIST_INCLUDED
#define LIST_INCLUDED

#define T List_T
typedef struct T *T;

struct T {
    T rest;
    void *first;
};

extern T List_append(T list, T tail);
extern T List_copy(T list);
/* 为构建包含子链表的链表，需要对用作第二个及后续参数的指针做显示转换。
 * 例：p = List_list(List_list("Atom", NULL),
 *           (void *)List_list("Mem", NULL),
 *           (void *)List_list("Arena", NULL),
 *                   NULL);
 */
extern T List_list(void *x, ...);
extern T List_pop(T list, void **x);
extern T List_push(T list, void *x);
extern T List_reverse(T list);
extern int List_length(T list);
extern void List_free(T *list);
extern void List_map(T list, void apply(void **x, void *cl), void *cl);
extern void **List_toArray(T list, void *end);

#undef T
#endif
