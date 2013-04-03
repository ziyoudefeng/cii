
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "assert.h"
#inlcude "seq.h"
#include "array.h"
#include "arrayrep.h"
#include "mem.h"

#define T Seq_T

struct T {
    struct Array_T array;
    int length; // 序列中值的数目
    int head; // 序列中索引为0的值保存在数组中索引为head的元素处，序列中索引号连续的值，也保存在数组的“连续”元素中。
};

//<static functions 128>
static void expand(T seq)
{
    int n = seq->array.length;
    Array_resize(&seq->array, 2*n);
    if (seq->head > 0) { // expand需处理将数组用作环型缓冲区的情形。原数组后半段的那些元素（从head之后）必须移动到扩展之后的数组末端
        //<slide tail down 129>
        void **old = &((void **)seq->array.array)[seq->head];
        memcpy(old+n, old, (n - seq->head)*sizeof(void *));
        seq->head += n;
    }
}

//<functions 126>
T Seq_new(int hint)
{
    T seq;

    assert(hint >= 0);
    NEW0(seq);
    if (hint == 0) {
        hint = 16;
    }
    ArrayRep_init(&seq->array, hint, sizeof(void *), ALLOC(hint * sizeof(void *)));
    return seq;
}

void Seq_seq(void *x, ...)
{
    va_list ap;
    T seq = Seq_new(0);

    va_start(ap, x);
    for (; x; x = va_arg(ap, void *)) {
        Seq_addhi(seq, x);
    }
    va_end(ap);
    return seq;
}

void Seq_free(T *seq)
{
    assert(seq && *seq);
    // 这里即：Array_T结构必须是Seq_T结构中的第一个字段，这样Seq_new中NEW0返回的指针既指向一个Seq_T实例，同时也指向一个Array_T实例。
    assert((void *)*seq == (void *)&(*seq)->array);
    Array_free((Array_T *)seq); // Array_free(T *array)主要代码为：FREE((*array)->array);FREE(*array);
}

int Seq_length(T seq)
{
    assert(seq);
    return seq->length;
}

void *Seq_get(T seq, int i)
{
    assert(seq);
    assert(i >= 0 && i < seq->length);
    return /*<seq[i] 127>*/((void **)seq->array.array)[(seq->head + i)%seq->array.length];
}

void *Seq_put(T seq, int i, void *x)
{
    void *prev;

    assert(seq);
    assert(i >= 0 && i < seq->length);
    prev = /*<seq[i] 127>*/((void **)seq->array.array)[(seq->head + i)%seq->array.length];
    /*<seq[i] 127>*/((void **)seq->array.array)[(seq->head + i)%seq->array.length]
     = x;
    return prev;
}

void *Seq_remhi(T seq)
{
    int i;

    assert(seq);
    assert(seq->length > 0);
    i = --seq->length;
    return /*<seq[i] 127>*/((void **)seq->array.array)[(seq->head + i)%seq->array.length];
}

void *Seq_remlo(T seq)
{
    int i = 0;
    void *x;
    assert(seq);
    assert(seq->length > 0);
    x = /*<seq[i] 127>*/((void **)seq->array.array)[(seq->head + i)%seq->array.length];
    seq->head = (seq->head + 1)%seq->array.length;
    --seq->length;
    return x;
}

void *Seq_addhi(T seq, void *x)
{
    int i;

    assert(seq);
    if (seq->length == seq->array.length) {
        expand(seq);
    }
    i = seq->length++;
    return /*<seq[i] 127>  = x;*/ /*<seq[i] 127>*/((void **)seq->array.array)[(seq->head + i)%seq->array.length] = x;
}

void *Seq_addlo(T seq, void *x)
{
    int i = 0;

    assert(seq);
    if (seq->length == seq->array.length) {
        expand(seq);
    }
    if (--seq->head < 0) {
        seq->head = seq->array.length - 1;
    }
    seq->length++;
    return /*<seq[i] 127>  = x;*/ /*<seq[i] 127>*/((void **)seq->array.array)[(seq->head + i)%seq->array.length] = x;
}
