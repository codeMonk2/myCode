// Copyright (c) 2020 Arista Networks, Inc.  All rights reserved.
// Arista Networks, Inc. Confidential and Proprietary.

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

// From https://loonytek.com/2017/03/17/lock-free-algorithms-part-1/

typedef struct Node {
   void* data;
   struct Node* next;
} lfll_node_t;

typedef struct lfll_s {
   lfq_node_t* head;
} lfll_t;

lfll_t *lfll_new() {
   lfll_t *ll = malloc( sizeof( lfll_t ) );
   lfll_node_t *sentinel = malloc( sizeof( lfll_node_t ) );
   sentinel->data = sentinel->next = NULL;
   ll->head = sentinel;
   return lfll;
}

lfll_t *lfll_free( lfll_t *ll ) {
   lfll_t *next, *node = ll->head;
   while( node ) {
      next = node->next;
      free( node );
      node = next;
   }
   free( ll );
}

void add( lfll_t *ll, void *data ) {
   lfll_node_t *node, *prev, *succ;
   node = malloc( sizeof( lfll_node_t ) );
   if ( !node ) return;
   node->data = data;
retry:
   prev = search( ll, data );
   succ = prev->next;
   node->next = succ;
   while ( CAS( &prev->next, succ, node ) ) {
      goto retry;
   }
}

void delete( lfll_t *ll, void *data ) {
  // CAS LOOP
  do
  {
     node *N1 = NULL;
     node *N2 = NULL;
     node *N3 = NULL;

     // SEARCH FOR THE ELEMENT, N1, AND N2 nodes.
     N2 = search(&N1, element);
     // CHECK ELEMENT SHOULD ALREADY EXIST - IGNORE THE CORNER CASE FOR NOW
         
 
     N3 = N2->next;

     // STEP1 -- TAGGING, MARK THE "NEXT" POINTER OF N2 (THE NODE TO BE DELETED)
     if( CAS(&(N2->next), N2->next, MARK_POINTER(N2->next)))
     {
         // IF WE ARE HERE, MARKING WAS SUCCESSFUL -- LSB set
         // STEP 2 -- SWING THE "NEXT" POINTER OF PREDECESSOR NODE TO REMOVE N2
         ret = CAS(&(N1->next), N2, N3);

         if(ret)
         {
             // CAS SUCCEEDED. N2 was successfully removed from the list. 
             // "next" field in N1 now points to node N3.
             return true;
         }
      }
   }
   while (TRUE);
}

