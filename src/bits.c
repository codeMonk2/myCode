#include <stdio.h>
#include <math.h>

void
checkEndian() {
    unsigned int i = 0x1234;
    char *c = (char*)&i;
    if (*c) 
        printf("Little endian\n");
    else
        printf("Big endian\n");
    //getchar();
    unsigned int j = 0;
    for (j = 0; j < 4; j++)
    {
        char *d = &((char *)&i)[j];
        printf("%p %02x\n", d, *d); 
    }
    printf ("%p\n", (int *)&i );
}

int add(int x, int y)
{
    int carry = 0;

    while(y) {
        carry = x&y;
        x ^= y;
        y = carry<<1;
    }

    return x;
}

int subtract(int x, int y)
{
    int carry = 0;

    while (y)
    {
        carry = ~x&y;
        x=x^y;
        y=carry<<1;
    }

    return x;
}

/* Multiplication is done using the school method for decimal
 * multiplication. Whenever the second number is one, shift
 * the first number by so many places to the left and add to
 * the result. If it is zero skip and move to the next number
 */

int multiply(int y, int x)
{
    int sum = 0;

    while(x) {
        if (x&1) {
            sum = add(sum, y);
        }
        x >>= 1; y <<= 1;
    }

    return sum;
}

int divide(int x, int y)
{
    int power = 32;
    unsigned long long y_power = y << power;
    int quo = 0;

    while (x >= y) {
        while(y_power > x) {
            y_power >>= 1;
            power--;
        }

        quo += 1<<power;
        x -= y_power;
    }

    printf("Remainder %d\n", x);
    return quo;
}

int check_sign(int v)
{
    int sign = -(v<0);

    printf("%d\n", sign);

    sign = v >> (sizeof(int) * 8 - 1);

    printf("%08x\n", v);

    return sign;
}

int abs_val(int v)
{
    int mask = v >> (sizeof(int) * 8 - 1);

    int r = (v + mask) ^ mask;
    //int r = (v + (-1)) ^ (-1);

    printf("%d\n", r);

    return r;
}

int min_max(int x, int y)
{
    int min = y ^ ((x^y) & -(x<y));
    int max = x ^ ((x^y) & -(x<y));

    printf("min %d max %d\n", min, max);

    return 0;
}

//if n is even n2 = (2(n/2))2 = 4(n/2)2
//if n is odd  n2 = (2(n/2) + 1)2 = 4(n/2)2 + 2.2(n/2).1 + 1 
int square_of_num(int k)
{
    if (k & 1) //k is odd
    {
        return ((int)pow((k>>1), 2)<<2) + ((k>>1)<<2) + 1;
    }
    else // k is even
    {
        return (int)pow((k>>1),2)<<2;
    }
}

// p^q = (p  OR q ) AND NOT ( p AND q ) 
int myXOR(int x, int y)
{
    return (x | y) & (~x | ~y);
}

//log to base 2 of an integer is the same as
//the position of the highest bit set
int log_base_2(int v)
{
    int result = 0;

    while(v >>= 1) {
        result++;
    }

    return result;
}

int log_base_10_1(int v)
{
    //unsigned int v; non-zero 32-bit integer value to compute the log base 10 of 
    int r;          // result goes here

    r = (v >= 1000000000) ? 9 : (v >= 100000000) ? 8 : (v >= 10000000) ? 7 : 
        (v >= 1000000) ? 6 : (v >= 100000) ? 5 : (v >= 10000) ? 4 : 
        (v >= 1000) ? 3 : (v >= 100) ? 2 : (v >= 10) ? 1 : 0;

    return r;
}

/* The integer log base 10 is computed by first using one of the techniques above 
 * for finding the log base 2. By the relationship log10(v) = log2(v) / log2(10), 
 * we need to multiply it by 1/log2(10), which is approximately 1233/4096, or 1233 
 * followed by a right shift of 12. Adding one is needed because log_base_2 rounds 
 * down. Finally, since the value t is only an approximation that may be off by one, 
 * the exact value is found by subtracting the result of v < PowersOf10[t].
 */

int log_base_10_2(int v)
{
    //unsigned int v; non-zero 32-bit integer value to compute the log base 10 of 
    int r;          // result goes here
    int t;          // temporary

    static unsigned int const PowersOf10[] = 
    {1, 10, 100, 1000, 10000, 100000,
        1000000, 10000000, 100000000, 1000000000};

    t = (log_base_2(v) + 1) * 1233 >> 12; // (use a lg2 method from above)
    r = t - (v < PowersOf10[t]);

    return r;
}

void change_val(unsigned int val, unsigned int start, unsigned int stop, unsigned int new_val)
{
    unsigned int mask = ((1<<start)-1)^((1<<stop)-1);
    printf("mask %08x\n", mask);
    new_val = (val & mask) | (new_val & ~mask);

    printf("%08x %08x\n", val, new_val);

    return;
}

void number_of_bits_to_flip( int a, int b ) {
   printf("%d\n", __builtin_popcount( a^b) );
   return;
}

int highest_set_bit( int a ) {
   int n = a;
   printf("%8x\n", n );
   n |= n>>1;
   printf("%8x\n", n );
   n |= n>>2;
   printf("%8x\n", n );
   n |= n>>4;
   printf("%8x\n", n );
   n |= n>>8;
   printf("%8x\n", n );
   n |= n>>16;
   printf("%8x\n", n );
   n = n+1;
   printf("%8x\n", n );
   return log2(n>>1);
}

int lowest_set_bit( int a ) {
   return log2( a & ~(a-1) );
}

int countBits( int n ){
    int count = 0;
    while( n ) {
        n &= (n-1);
        count++;
    }
    return count;
}

int reverseBits( unsigned int n ){
    int size = sizeof( n )*8;
    unsigned int ret = 0, i=0;
    while( n ) {
        if ( n & 0x1 ) {
            ret |= 1 << ( size - 1 - i);
        }
        n >>= 1; i++;
    }
    return ret;
}

void *myMemcpy( void *dest, void *src, int len ) {
    char *d = ( char *)dest;
    char *s = ( char *)src;
    while( len-- ) {
        *d++ = *s++;
    }
    return dest;
}

#define swap( a, b )\
    a ^= b; b ^= a; a ^= b;

char *strReverse( char *s, int start, int len) {
    for( int i = 0; i < len/2; i++) {
        swap( s[i+start], s[start+len-1-i]);
    }
    return s;
}

#define mySizeOf( type ) ((type *)0)+1

int main()
{
    checkEndian();
    check_sign(-4);
    abs_val(-4);
    min_max(5, -8);
    printf("%d\n", square_of_num(3));
    printf("%d\n", subtract(5, 3));
    printf("%d\n", divide(3502342,235623));
    printf("%d\n", multiply(-3,2));
    change_val(0xffff1234, 8, 16, 0xffffffc1);
    number_of_bits_to_flip( 7, 10 );
    printf("highest set bit %d\n", highest_set_bit(536870912) );
    printf("lowest set bit %d\n", lowest_set_bit(536870912) );
    printf("reverse bits %08x\n", reverseBits( 1 ) ) ;
    return 0;
}
