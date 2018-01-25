#ifndef __UTILS_H__
#define __UTILS_H__

#define offsetof(type, mbr)\
  ((size_t)&(((type *)0)->mbr))

#define container_of(ptr, type, mbr) ({\
  const typeof(((type *)0)->mbr) *__mptr = ptr;\
  (type *)((char *)__mptr - (char *)offsetof(type, mbr));})

#define swap(a,b) \
  {\
    (a) = ((a) ^ (b));\
    (b) = ((a) ^ (b));\
    (a) = ((a) ^ (b));\
  }

#define max(a,b) (a ^ ((a^b) & -(a<b))
#define min(a,b) (b ^ ((a^b) & -(a<b))

#endif //__UTILS_H__
