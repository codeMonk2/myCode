// Copyright (c) 2020 Arista Networks, Inc.  All rights reserved.
// Arista Networks, Inc. Confidential and Proprietary.

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

typedef struct Node {
   void* data;
   struct Node* next;
} lfq_node_t;

typedef struct Queue {
   lfq_node_t* head;
   lfq_node_t* tail;
} lfq_t;

lfq_t* lfq_new();
void lfq_free(lfq_t* q);
void lfq_enq(lfq_t* q, void* data);
void* lfq_deq(lfq_t* q);

#define CAS(ref, old, new) __sync_bool_compare_and_swap(ref, old, new)

lfq_t* lfq_new() {
   lfq_t* q = malloc(sizeof(*q));
   lfq_node_t* sentinel = malloc(sizeof(*sentinel));
   sentinel->data = sentinel->next = NULL;
   q->head = q->tail = sentinel;

   return q;
}

void lfq_free(lfq_t* q) {
   lfq_node_t *next, *node = q->head;
   while (node != NULL) {
      next = node->next;
      free(node);
      node = next;
   }
   free(q);
}

void lfq_enq(lfq_t* q, void* data) {
   lfq_node_t *node, *last, *next;

   node = malloc(sizeof(*node));
   node->data = data;
   node->next = NULL;

   while (1) {
      last = q->tail;
      next = last->next;
      if (last == q->tail) {
         if (next == NULL) {
            if (CAS(&(last->next), next, node)) {
               CAS(&(q->tail), last, node);
               return;
            }
         } else {
            CAS(&(q->tail), last, next);
         }
      }
   }
}

void* lfq_deq(lfq_t* q) {
   lfq_node_t *first, *last, *next;
   while (1) {
      first = q->head;
      last = q->tail;
      next = first->next;

      if (first == q->head) {
         if (first == last) {
            if (next == NULL) return NULL;
            CAS(&(q->tail), last, next);
         } else {
            void* data = first->next->data;
            if (CAS(&(q->head), first, next)) {
               // free(first);
               return data;
            }
         }
      }
   }
}

int main() {
   int values[] = {1, 2, 3, 4, 5};
   lfq_t* q = lfq_new();
   for (int i = 0; i < 5; ++i) {
      printf("ENQ %i\n", values[i]);
      lfq_enq(q, &values[i]);
   }
   for (int i = 0; i < 5; ++i) printf("DEQ %i\n", *(int*)lfq_deq(q));
   lfq_free(q);
   return 0;
}
