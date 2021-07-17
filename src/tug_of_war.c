#include <stdio.h>
#include <stdlib.h>

#define N 10

int is_sol(int sol[], int k, int final_sol[], int *diff, int total_sum)
{
    int sum = 0;

    if (k < N/2 || k > N/2)
        return 0;

    int i = 0;

    for (i = 0; i < k; i++)
    {
        sum += sol[i];
    }

    if (!*diff) {
        *diff = abs(total_sum - (2*sum));
        for (i = 0; i < k; i++)
        {
            final_sol[i] = sol[i];
        }
        return 1;
    } else {
        if (abs(total_sum - (2*sum)) < *diff) {
            *diff = abs(total_sum - (2*sum));
            for (i = 0; i < k; i++)
            {
                final_sol[i] = sol[i];
            }
            return 1;
        }
    }
    return 0;
}

int backtrack(int sol[], int k, int s[], int final_sol[], int *diff, int a, int total_sum)
{
    int i = 0;

    if (k>N/2)
        return 1;
    
    if (is_sol(sol, k, final_sol, diff, total_sum)) {
        return 1;
    } else {
        for (i = a; i < N; i++)
        {
            sol[k]=s[i];
            backtrack(sol, (k+1), s, final_sol, diff, i+1, total_sum);
            sol[k]=0;
        }
    }

    return 0;
}

int main()
{
    int s[N] = {3, 4, 5, -3, 100, 1, 89, 54, 23, 20};
    int sol[N/2] = {0};
    int final_sol[N/2] = {0};
    int diff = 0;
    int i = 0, total_sum = 0;
    for (i = 0; i < N; i++)
        total_sum += s[i];
    backtrack(sol, 0, s, final_sol, &diff, 0, total_sum);

    printf("diff %d\n", diff);
    for (i = 0; i < N/2; i++)
        printf("%d ", final_sol[i]);

    printf("\n");

}
