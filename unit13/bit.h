
#ifndef BIT_INCLUDED
#define BIT_INCLUDED

#define T Bit_T
typedef struct T *T;
//<exported functions 142>
extern T Bit_new(int length);
extern int Bit_length(T set);
extern int Bit_count(T set);
extern int Bit_free(T *set);
extern int Bit_get(T set, int n);
extern int Bit_put(T set, int n, int bit);

extern void Bit_clear(T set, int lo, int hi);
extern void Bit_set(T set, int lo, int hi);
extern void Bit_not(T set, int lo, int hi);

extern int Bit_lt(T s, T t);
extern int Bit_eq(T s, T t);
extern int Bit_leq(T s, T t);

extern void Bit_map(T set, void apply(int n, int bit, void *cl), void *cl);

extern T Bit_union(T s, T t);
extern T Bit_inter(T s, T t);
extern T Bit_minus(T s, T t); // t的补集和s按位与
extern T Bit_diff(T s, T t); // s和t的对称差s/t，它是两个位向量的按位异或。

#undef T
#endif // BIT_INCLUDED
