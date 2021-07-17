#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define min(x,y) (y^((x^y)&-(x<y)))

int pcost(int cost[16][3], int rsize, int csize, int r, int prev, int **mem)
{
    int ret = 100000;

    if (r == rsize) return 0;

    if (prev != -1 && mem[r][prev] != -1) {
        printf("2 %d %d %d\n", r, prev, mem[r][prev]); 
        return mem[r][prev];
    }

    for (int i = 0; i < csize; i++) {
        ret = min(ret, cost[r][i] + pcost(cost, rsize, csize, r+1, i, mem));
        printf("3 %d %d %d\n", r, prev, ret);
        if (prev != -1) mem[r][prev] = ret;
    }

    return ret;
}

int paint_cost(int cost[16][3], int rsize, int csize)
{
    int *mem[rsize];
    for (int i = 0; i < rsize; i++) {
        mem[i] = malloc(csize * sizeof(int));
        for (int j = 0; j < csize; j++) {
            mem[i][j] = -1;
        }
    }
    return pcost(cost, rsize, csize, 0, -1, mem);
}

int main()
{
    int cost[16][3] = {{7,17,16}, {3,10,1}, {6,12,18}, {12,18,17}, {15,18,16}, {17,4,1}, {13,8,8}, {5,14,5}, {1,11,1}, {5,10,8}, {18,11,15}, {6,13,10}, {17,7,2}, {15,12,10}, {9,3,19}, {2,3,11}} ;
    printf("%d\n", paint_cost(cost, 16, 3));
    return 0;
}
