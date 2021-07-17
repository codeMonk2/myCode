#include <stdio.h>

#define N 3

int is_sol(int sol[], int k, int max_sum, int *cur_sum)
{
    int i = 0;
    *cur_sum = 0;
    for (i = 0; i < k; i++)
    {
        *cur_sum += sol[i];
    }
    return (*cur_sum == max_sum) ? 1:0;
}

int backtrack(int sol[], int k, int s[], int max_sum)
{
    int cur_sum = 0;

    if (k == N)
        return 0;

    if (is_sol(sol, k, max_sum, &cur_sum)) {
        int i = 0;
        for (i = 0; i < k; i++) {
            printf("%d ", sol[i]);
        }
        printf("\n");
        return 1;
    } else {
        if (cur_sum > max_sum) {
            return 0;
        } else {
            int i = 0;
            for (i = k; i < N; i++) {
                sol[k] = s[i];
                backtrack(sol, (k+1), s, max_sum);
                sol[k] = 0;
            }
        }
    }

    return 0;
}


int main()
{
    int S[N] = {1,2,3};
    int max_sum = 3;
    int sol[N] = {0};
    backtrack(sol, 0, S, max_sum);
    
    return 0;
}

