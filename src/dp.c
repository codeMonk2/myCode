#include <stdio.h>

unsigned long fib_arr[500];

unsigned long fib(unsigned long n);

int printFib()
{
    int nth_fib = 0;
    printf("Nth Fib No:");
    scanf("%d", &nth_fib);
    fib_arr[0] = 0;
    fib_arr[1] = 1;
    printf("\nNth fib no. is : %ld\n", fib(nth_fib));

}

unsigned long fib(unsigned long  n)
{
    if (n == 0)
        return 0;
    if (fib_arr[n] != 0)
        return fib_arr[n];

    fib_arr[n] = fib(n-1) + fib(n-2);

    return fib_arr[n];
}

void printgrid(int grid[10][10]) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%d ", grid[i][j]);
        }
        printf("\n");
    }
    return;
}

int upaths(int m, int n, int r, int c, int grid[10][10]) {
    if (r > m || c > n) return 0;

    if (r == m-1 && c == n-1) return 1;

    if (grid[r][c] != -1) {
        return grid[r][c];
    }

    grid[r][c] = upaths(m,n,r+1,c,grid) + upaths(m,n,r,c+1,grid);
    return grid[r][c];
}


int uniquePaths(int m, int n) {
    int grid[10][10] = {-1};
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
           grid[i][j] = -1; 
        }
    }
    return upaths(m,n,0,0, grid);
}

int main()
{
    printf("%d\n", uniquePaths(2,2));
    return 0;
}
