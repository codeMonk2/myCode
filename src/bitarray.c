#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>


uint8_t *create_bitmap(int num_bits)
{
    uint8_t *bitmap;
    int num_bytes = ceil(num_bits/8);
    bitmap = malloc(num_bytes*sizeof(uint8_t));
    memset((char *)bitmap, 0, (num_bytes*sizeof(uint8_t)));
    return bitmap;
}

#define set_bit(a,k) a[k/8] |= (1<<(k%8))
#define clear_bit(a,k) a[k/8] &= ~(1<<(k%8))
#define test_bit(a,k) (a[k/8] & (1<<(k%8))

int main()
{
    uint8_t *bitmap = create_bitmap(16);
    printf("0x%x 0x%x\n", bitmap[0], bitmap[1]);
    set_bit(bitmap, 8);
    printf("0x%x 0x%x\n", bitmap[0], bitmap[1]);
    return 0;
}
