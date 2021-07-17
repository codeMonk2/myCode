#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool isSolution(int a[], int k, int n)
{
    return (k == n);
}

void construct_candidates(int a[], int k, int n, int clist[], int *num_cand)
{
    clist[0] = 1;
    clist[1] = 0;
    *num_cand = 2;
    return;
}

void backtrack(int a[], int k, int n, int sol[])
{
    if (isSolution(a, k, n)) {
        int flag = 0;
        for(int i = 0; i < k; i++) {
            if (sol[i]) {
                printf("%d ", a[i]);
                flag = 1;
            }
        }
        if (!flag) printf("Empty set");
        printf("\n");
        return;
    } else {
        int clist[2] = {0};
        int num_cand = 0;
        construct_candidates(a, k, n, clist, &num_cand);
        for (int i = 0; i < num_cand; i++ ){
            sol[k] = clist[i];
            backtrack(a, (k+1), n, sol);
            sol[k] = 0;
        }
    }
    return;
}

int main()
{
    int a[6] = {1,2,3};
    int sol[3] = {0};
    backtrack(a, 0, 3, sol);
}
