#include "arith.h"

//<arith.c functions 14>
int Arith_max(int x, int y)
{
    return x > y ? x : y;
}

int Arith_min(int x, int y)
{
    return x > y ? y : x;
}

int Arith_div(int x, int y)
{
    //<division truncates toward 0>可以采用一个实际的例子来判断，这里采用-13/5 == -2来判断
    if (/*<division truncates toward 0 14>*/ -13/5 == -2
        && /*<x and y have different signs 14>*/ (x < 0) != (y < 0)
        && x%y != 0) {
            return x/y - 1;
    } else {
        return x/y;
    }
}

int Arith_mod(int x, int y)
{
    // 可以直接return x - y * Arith_div(x, y);
    if (-13/5 == -2 && (x < 0 ) != (y < 0) && x%y != 0) {
        return x%y + y;
    } else {
        return x%y;
    }
}

int Arith_floor(int x, int y)
{
    return Arith_div(x, y);
}

int Arith_ceiling(int x, int y)
{
    return Arith_div(x, y) + (x%y != 0); // Arith_div + 1，除非y能整除x
}
