
#include <stdarg.h>
#include <stddef.h>
#include "assert.h"
#include "mem.h"
#include "list.h"

#define T List_T

 <functions 79>
 T List_push(T list, void *x)
 {
     T p;

     NEW(p);
     p->first = x;
     p->rest = list;
     return p;
 }

 T List_list(void *x, ...)
 {
     va_list ap;
     T list, *p = &list;

    // NB: p最初指向list，因此循环体中的操作会把指向第一个结点的指针赋值给list。
    //     此后，p指向链表中最后一个结点的rest字段，因此对*p的赋值就是向链表添加了一个结点。
     va_start(ap, x);
     for (; x; x = va_arg(ap, void *)) {
        NEW(*p);
        (*p)->first = x;
        p = &((*p)->first);
     }
     *p = NULL;// 即使x初始值为NULL，也可以确保List_list(NULL)返回NULL，即list = NULL。
     va_end(ap);
     return list;
 }

T List_append(T list, T tail)
{
    T *p = &list;

    while (*p) {
        p = &(*p)->list;
    }
    *p = tail;
    return list;
}

T List_copy(T list)
{
    T head, *p = &head;

    for (; list; list = list->rest) {
        NEW(*p);
        (*p)->first = list->first;
        p = &(*p)->rest;
    }
    *p = NULL;
    return head;
}

T List_pop(T list, void **x)
{
    if (list) {
        T head = list->rest;
        if (x) {
            *x = list->first;
        }
        FREE(list);
        return head;
    } else {
        return list;
    }
}


T List_reverse(T list)
{
    T head = NULL, next;

    for (; list; list = next) {
        next = list->rest;
        list->rest = head;
        head = list;
    }
    return head;
}

int List_length(T list)
{
    int n;

    for (n = 0; list; list = list->rest) {
        n++;
    }
    return n;
}

void List_free(T *list)
{
    T next;

    assert(list);
    for (; *list; *list = next) {
        next = (*list)->rest;
        FREE(*list);
    }
}

void List_map(T list, void apply(void **x, void *cl), void *cl)
{
    assert(apply);
    for (; list; list = list->rest) {
        apply(&list->first, cl);
    }
}

void **List_toArray(T list, void *end)
{
    int i, n = List_length(list);
    void **array = ALLOC((n + 1)*sizeof(*array));

    for (i = 0; i < n; i++) {
        array[i] = list->first;
        list = list->rest;
    }
    array[i] = end;
    return array;
}
