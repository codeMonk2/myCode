#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

bool is_prime(int n)
{
    for (int i = 2; i*i <= n; i++) {
        if (!(n%i)) {
            return false;
        }
    }
    return true;
}

int main()
{
    int x = 0;
    printf("Enter the number:");
    scanf("%d", &x);
    if (is_prime(x)) printf("Prime\n");
    else printf("Not Prime \n");
    return 0;
}
