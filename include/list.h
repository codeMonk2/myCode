#ifndef __LIST_H__
#define __LIST_H__

#include <stdbool.h>
#include "utils.h"

typedef struct list_head {
  struct list_head *next, *prev;
} lh;

void init_list_head(lh *node) {
  node->next = node->prev = node;
}

void __list_add(lh *node, lh *next, lh *prev) {
  node->next = next;
  node->prev = prev;
  prev->next = node;
  next->prev = node;
}

void list_add(lh *node, lh *head) {
  __list_add(node, head->next, head);
}

void list_add_tail(lh *node, lh *head) {
  __list_add(node, head, head->prev);
}

void __list_del(lh *next, lh *prev) {
  prev->next = next;
  next->prev = prev;
}

void list_del(lh *node){
  __list_del(node->next, node->prev);
}

bool is_list_empty(lh *head) {
  return head->next == head->prev;
}

#define LIST_ITER(var, node)						\
  for (lh *var = (node)->next ; var != (node) ; var = var->next)

#endif //__LIST_H__
