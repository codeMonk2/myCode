#include <stdio.h>
#include <stdlib.h>

typedef int (*FN_PTR)(int,int);

typedef int (*FN_PTR_ARR [5])(int, int);

struct fn_list {
    FN_PTR sum_func;
    FN_PTR sum_func1;
};

int funC(int a, int b)
{
    return a+b;
}

void funB(FN_PTR fun_var)
{
    printf("sum is %d\n", fun_var(2,3));
}

void funA()
{
    FN_PTR arr [10];

    arr[0] = funC;
    funB(arr[0]);

    FN_PTR_ARR a;
    a[0] = funC;

    funB( a[0] );
}

void funX()
{
    struct fn_list fns = {
        .sum_func  = funC,
        .sum_func1 = funC,
    };

    printf("sum is %d\n", fns.sum_func(3,5));
}

void funD(int *x)
{
    *x = 5;
}

int main()
{
    int *p = malloc(4);
    *p = 10;
    funD(p);
    printf("%d\n", *p);
    free(p);
    funX();
    funA();
    return 0;
}
