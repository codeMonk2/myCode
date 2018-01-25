#ifndef __STACK_H__
#define __STACK_H__

#include <stdbool.h>
#include "utils.h"
#include "list.h"

typedef struct stack {
  lh stk_head;
  int size;
} stk;

typedef struct stack_node {
  void *data;
  lh stk_next;
} stk_node;

void init_stack( stk *stack ){
  init_list_head(&stack->stk_head);
  stack->size = 0;
} 

bool is_stack_empty( stk *stack ) {
  return stack->stk_head.next == stack->stk_head.prev ;
}

void init_stack_node( stk_node *node ) {
  memset(node, 0, sizeof(stk_node)); 
  init_list_head(&node->stk_next);
}

void push ( void *data, stk *stack ) {
  stk_node *node = malloc( sizeof(stk_node) );
  init_stack_node(node);
  node->data = data;
  list_add(&node->stk_next, &stack->stk_head);
  stack->size++;
  return;
}

stk_node *pop( stk *stack ) {
  stk_node *ret = NULL;  
  if (!is_stack_empty(stack)) {
    ret = container_of(stack->stk_head.next, stk_node, stk_next);
    list_del(stack->stk_head.next);
    stack->size--;
  }
  return ret;
}

#endif //__STACK_H__
