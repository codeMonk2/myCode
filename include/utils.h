#ifndef __UTILS_H__
#define __UTILS_H__

#define offsetof(type, mbr)\
  ((size_t)&(((type *)0)->mbr))

#define container_of(ptr, type, mbr) ({\
  const typeof(((type *)0)->mbr) *__mptr = ptr;\
  (type *)((char *)__mptr - (char *)offsetof(type, mbr));})

#endif //__UTILS_H__
