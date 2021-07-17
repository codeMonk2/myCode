#include <stdio.h>

#define N  6

int is_sol(int sol[], int k, int n)
{
    return (k == n) ? 1:0;
}

int backtrack(int sol[], int k, int n)
{
    if (is_sol(sol, k, n)) {
        int i = 0;
        for (i = 0; i < k; i++)
        {
            printf("%d ", sol[i]);
        }
        printf("\n");
        return 1;
        
    } else {
        int i = 0;
        for (i = 0; i < n; i++)
        {
            sol[k]=i+1;
            backtrack(sol, k+1, n);
            sol[k]=0;
        }
        return 0;
    }
}

int main()
{
    int n = 3;
    int sol[N] = {0};
    backtrack(sol, 0, n);
    return 0;
}
