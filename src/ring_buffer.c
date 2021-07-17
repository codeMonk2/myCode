#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct ring_desc {
    int fd;
} ring_desc_t;

typedef struct ring_buff {
    char *buff;
    ring_desc_t desc;
} ring_buff_t;

typedef struct ring_s {
    ring_buff_t ring_buff[10];
    int tail;
    int head;
    int count;
    rwlock_t rwlock; 
} ring_t;

ring_t ring = {.tail = 0, .head = 0, .count = 0};

void read_ring_buff(void *buf, int len)
{
    down_read(&ring.rwlock);
    if (ring.count > 0) {
        memcpy((char *)buf, (char *)ring.ring_buff[ring.tail].buff, len);
        ring.tail = (ring.tail +  1)%10;
        ring.count--;
    }
    up_read(&ring.rwlock);
    return;
}

void write_ring_buff(void *buf, int len)
{
    down_write(&ring.rwlock);
    if ((ring.count+1) % 10 != 0) {
        memcpy((char *)ring.ring_buff[ring.head].buff, (char *)buf, len);
        ring.head = (ring.head + 1) % 10;
        ring.count++;
    }
    up_write(&ring.rwlock);
}
