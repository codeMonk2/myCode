#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdbool.h>
#include "utils.h"
#include "list.h"

typedef struct queue {
  lh q_head;
  int size;
} q;

typedef struct q_node {
  void *data;
  lh q_next;
} qn;

void init_queue( q *qu ) {
  qu->size = 0;
  init_list_head(&qu->q_head);
}

bool is_queue_empty( q *qu ) {
  return qu->q_head.next == qu->q_head.prev;
}

void init_q_node( qn *node ) {
  init_list_head(&node->q_next);
}

void enqueue(void *data, q *qu) {
  qn *node = malloc(sizeof(qn));
  init_q_node(node);
  node->data = data;
  list_add_tail(&node->q_next, &qu->q_head);
  qu->size++;
  return;
}

qn * dequeue(q *qu) {
  qn *ret = NULL;
  if (!is_queue_empty(qu)) {
    ret = container_of(qu->q_head.next, qn, q_next);
    list_del(qu->q_head.next);
    qu->size--;
  }
  return ret;
}

#endif //__QUEUE_H__
