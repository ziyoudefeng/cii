
#include <limits.h>
#include <stddef.h>
#include "mem.h"
#include "assert.h"
#include "arith.h"
#include "set.h"
#define T Set_T

//<types 108>
struct T {
    int length; // 成员数目
    unsigned timestamp; //用于实现Set_map中的已检查的运行时错误，即禁止apply修改集合。
    int (*cmp)(const void *x, const void *y);
    unsigned (*hash)(const void *x);
    int size; // 容量
    struct member {
        struct member *link;
        const void *member;
    } **buckets;
};

//<static functions 108>
static int cmpatom(const void *x, const void *y)
{
    return x != y;
}

static unsigned hashatom(const void *x)
{
    return (unsigned long)x >> 2;
}

static T copy(T t, int hint)
{
    T set;

    assert(t);
    set = Set_new(hint, t->cmp, t->hash);
    //<for each member in t 112>
    int i;
    struct member *q;
    for (i = 0; i < t->size; i++) {
        for (q = t->buckets[i]; q; q = q->link) {
            //<add q->member to set 112>
            struct member *p;
            const void *member = q->member;
            int j = (*set->hash)(member) % set->size;
            //<add member to set 109>
            NEW(p);
            p->member = member;
            p->link = set->bucket[j];
            set->buckets[j] = p;
            set->length++;
        }
    }
}

//<functions 108>
T Set_new(int hint, int cmp(const void *x, const void *y), unsigned hash(const void *x))
{
    T set;
    int i;
    static int primes[] = {509, 509, 1021, 2053, 4093, 8191, 16381, 32771, 65521, INT_MAX};

    assert(hint >= 0);
    for (i = 1; primes[i] < hint, i++) {
        ;
    }
    set = ALLOC(sizeof(*set) + primes[i-1] * sizeof(set->buckets[0]));
    set->size = primes[i-1];
    set->cmp = cmp ? cmp : cmpatom;
    set->hash = hash ? hash : hashatom;
    set->buckets = (struct member **)(set + 1);
    for (i = 0; i < set->size; i++) {
        set->buckets[i] = NULL;
    }
    set->length = 0;
    set->timestamp = 0;
    return set;
}

int Set_member(T set, const void *member)
{
    int i;
    struct member *p;

    assert(set);
    assert(member);
    //<search set for member 109>
    i = (*set->hash)(member) % set->size;
    for (p = set->buckets[i]; p; p = p->link) {
        if ((*set->cmp)(member, p->member) == 0) {
            break;
        }
    }

    return p != NULL;
}

void Set_put(T set, const void *member)
{
    int i;
    struct member *p;

    assert(set);
    assert(member);
    //<search set for member 109>
    i = (*set->hash)(member) % set->size;
    for (p = set->buckets[i]; p; p = p->link) {
        if ((*set->cmp)(member, p->member) == 0) {
            break;
        }
    }

    if (p == NULL) {
        //<add member to set 109>
        NEW(p);
        p->member = member;
        p->link = set->buckets[i];
        set->buckets[i] = p;
        set->length++;
    } else {
        p->member = member; // 不多余吗？
    }

    set->timestamp++;
}

void *Set_remove(T set, const void *member)
{
    int i;
    struct member **pp;

    assert(set);
    assert(member);
    set->timestamp++;
    i = (*set->hash)(member) % set->size;
    for (&pp = set->buckets[i]; *pp; pp = &(*pp)->link) {
        if ((*set->cmp)(member, (*pp)->member) == 0) {
            struct member *p = *pp;
            *pp = p->link;
            member = p->member; // 可能是一个不同于member的指针
            FREE(p);
            set->length--;
            return (void *)member;
        }
    }
    return NULL;
}

int Set_length(T set)
{
    assert(set);
    return set->length;
}

void Set_free(T *set)
{
    assert(set && *set);
    if ((*set)->length > 0) {
        int i;
        struct member *p, *q;
        for (i = 0; i < (*set)->size; i++) {
            for (p = (*set)->buckets[i]; p; p = q) {
                q = p->link;
                FREE(p);
            }
        }
    }
    FREE(*set);
}

void Set_map(T set, void apply(const void *member, void *cl), void *cl)
{
    int i;
    unsigned stamp;
    struct member *p;

    assert(set);
    assert(apply);
    stamp = set->timestamp;
    for (i = 0; i < set->size; i++) {
        for (p = set->buckets[i]; p; p = p->link) {
            apply(p->member, cl);
            assert(set->timestamp == stamp);
        }
    }
}

void **Set_toArray(T set, void *end)
{
    int i, j = 0;
    void **array;
    struct member *p;

    assert(set);
    array = ALLOC((set->length + 1) * sizeof(*array));
    for (i = 0; i < set->size; i++) {
        for (p = set->buckets[i]; p; p = p->link) {
            array[j++] = (void *)p->member;
        }
    }
    array[j] = end;
    return array;
}

T Set_union(T s, T t)
{
    if (s == NULL) {
        assert(t);
        return copy(t, t->size);
    } else if (t == NULL) {
        return copy(s, s->size);
    } else {
        T set = copy(s, Arith_max(s->size, t->size));
        assert(s->cmp = t->cmp && s->hash == t->hash);
        //<for each members q in t 112>
        int i;
        struct member *q;
        for (i = 0; i < t->size; i++) {
            for (q = t->buckets[i]; q; q = q->link) {
                Set_put(set, q->member);
            }
        }
        return set;
    }
}

T Set_inter(T s, T t)
{
    if (s == NULL) {
        assert(t);
        return Set_new(t->size, t->cmp, t->hash);
    } else if (t == NULL) {
        return Set_new(s->size, s->cmp, s->hash);
    } else if (s->length < t->length) {
        return Set_inter(t, s);
    } else {
        T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);
        assert(s->cmp == t->cmp && s->hash == t->hash);
        //<for each member q in t 112>
        int i;
        struct member *q;
        for (i = 0; i < t->size; i++) {
            for (q = t->buckets[i]; q; q = q->link) {
                if (Set_member(s, q->member)) {
                    //<add q->member to set 112>
                    struct member *p;
                    const void *member = q->member;
                    int i = (*set->hash)(member) % set->size;
                    //<add member to set 109>
                    NEW(p);
                    p->member = member;
                    p->link = set->buckets[i];
                    set->buckets[i] = p;
                    set->length++;
                }
            }
        }
        return set;
    }
}

T Set_minus(T t, T s)
{
    if (t == NULL) {
        assert(s);
        return Set_new(s->size, s->cmp, s->hash);
    } else if (s == NULL) {
        return copy(t, t->size);
    } else {
        T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);
        assert(s->cmp == t->cmp && s->hash == t->hash);
        //<for each member q in t 112>
        int i;
        struct member *q;
        for (i = 0; i < t->size; i++) {
            for (q = t->buckets[i]; q; q = q->link) {
                if (!Set_member(s, q->member)) {
                    //<add q->member to set 112>
                    struct member *p;
                    const void *member = q->member;
                    int j = (*set->hash)(member) %set->size;
                    //<add member to set 109>
                    NEW(p);
                    p->member = member;
                    p->link = set->buckets[j];
                    set->buckets[j] = p;
                    set->length++;
                }
            }
        }
        return set;
    }
}


T Set_diff(T s, T t)
{
    if (s == NULL) {
        assert(t);
        return copy(t, t->size);
    } else if (t == NULL) {
        return copy(s, s->size);
    } else {
        T set = Set_new(Arith_min(s->size, t->size), s->cmp, s->hash);
        assert(s->cmp == t->cmp && s->hash == t->hash);
        //<for each member q in t 112>
        int i;
        struct member *q;
        for (i = 0; i < t->size; i++) {
            for (q = t->buckets[i]; q; q = q->link) {
                if (!Set_member(s, q->member)) {
                    //<add q->member to set 112>
                    struct member *p;
                    const void *member = q->member;
                    int j = (*set->hash)(member) % set->size;
                    //<add member to set 109>
                    NEW(p);
                    p->member = member;
                    p->link = set->buckets[j];
                    set->buckets[j] = p;
                    set->length++;
                }
            }
        }

        T u = t;
        t = s;
        s = u;
        //<for each member q in t 112>
        for (i = 0; i < t->size; i++) {
            for (q = t->buckets[i]; q; q = q->link) {
                if (!Set_member(s, q->member)) {
                    //<add q->member to set 112>
                    struct member *p;
                    const void *member = q->member;
                    int j = (*set->hash)(member) % set->size;
                    //<add member to set 109>
                    NEW(p);
                    p->member = member;
                    p->link = set->buckets[j];
                    set->buckets[j] = p;
                    set->length++;
                }
            }
        }

        return set;
    }
}
