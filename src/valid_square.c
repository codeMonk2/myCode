#include <stdio.h>

typedef enum {
    FALSE = -1,
    TRUE  = 0
}boolean;

boolean is_valid_square(float x)
{
    for (float i = 1; i < x; i++)
    {
        float y = x/i;
        if (y == i)
            return TRUE;
        else if (i > y)
            return FALSE;
    }

    if (x <= 1)
        return TRUE;

    return FALSE;
}


int main()
{
    float x;
    printf("Enter a number:");
    scanf("%f", &x);
    printf("No. is perfect square: %d\n", is_valid_square(x));
    return 0;
}
