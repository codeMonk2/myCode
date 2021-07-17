#include <stdio.h>

int add(int a, int b)
{
    int c;
    c = a+b;
    return c;
}

int main()
{
    int a  = add(10,20);
    int b = 0x12345678;;
    char *c = (char *)&b;
    printf("%p %x %p %x\n", c, *c, c+1, *(c+1)); 
    return 0;
}
