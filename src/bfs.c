#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define offsetof(TYPE, MEMBER)  ((size_t)&((TYPE *)0)->MEMBER)

#define container_of(ptr, type, member) ({          \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
        (type *)( (char *)__mptr - offsetof(type,member) );})

typedef struct listhead {
    struct listhead *prev, *next;
}lh;

void init_list(lh *head)
{
    head->prev = head;
    head->next = head;
}

void __list_add(lh *new, lh *prev, lh *next)
{
    new->next = next;
    next->prev = new;
    prev->next = new;
    new->prev = prev;
}

void list_add_tail(lh *new, lh *head)
{
    __list_add(new, head->prev, head);
}

void list_add(lh *new, lh *head)
{
    __list_add(new, head, head->next);
}

void __list_del(lh *prev, lh *next)
{
    prev->next = next;
    next->prev = prev;
}

void list_del(lh *entry)
{
    __list_del(entry->prev, entry->next);
}

bool list_is_empty(lh *list)
{
    return list->next == list;
}

void reverse(lh* node, lh* head)
{
    if (node->next != head) {
        reverse(node->next, head);
    }
    lh *temp = node->next;
    node->next = node->prev;
    node->prev = temp;
}

typedef struct queue {
    lh qhead;
}que;

typedef struct queuenode {
    int data;
    lh entry;
}qn;

void init_queue(que *q)
{
    init_list(&q->qhead);
}

void enqueue(que *q, int data)
{
    qn *qn1 = malloc(sizeof(qn));

    qn1->data = data;

    list_add_tail(&qn1->entry, &q->qhead);
}

qn* dequeue(que *q)
{
    if(!list_is_empty(&q->qhead)) {
        qn *qn1 = container_of(q->qhead.next, qn, entry);
        //list_del(q->qhead.next);
        list_del( &qn1->entry );
        return qn1;
    }
    return NULL;
}

bool queue_is_empty(que *q)
{
    return list_is_empty(&q->qhead);
}

typedef struct stack {
    lh shead;
}st;

typedef struct stacknode {
    int data;
    lh  entry;
}sn;

void init_stack(st *stack)
{
    init_list(&stack->shead);
}

void push(st *stack, int data)
{
    sn *sn1 = malloc(sizeof(sn));

    sn1->data = data;

    list_add(&sn1->entry, &stack->shead);
}

sn* pop(st *stack)
{
    if(!list_is_empty(&stack->shead)) {
        sn *sn1 = container_of(stack->shead.next, sn, entry);
        list_del(&sn1->entry);
        return sn1;
    }

    return NULL;
}

bool is_stack_empty(st *stack)
{
    return list_is_empty(&stack->shead);
}


int main()
{
    que q;
    init_queue(&q);
    enqueue(&q, 5);
    enqueue(&q, 6);
    enqueue(&q, 7);
    enqueue(&q, 8);

    lh *pos;

    for(pos = q.qhead.next; pos != &q.qhead; pos = pos->next)
    {
        printf("%d ", container_of(pos, qn, entry)->data);
    }

    //dequeue(&q);
    //dequeue(&q);

    printf("\n");

    reverse(&q.qhead, &q.qhead);

    for(pos = q.qhead.next; pos != &q.qhead; pos = pos->next)
    {
        printf("%d ", container_of(pos, qn, entry)->data);
    }

    printf("\n");
    printf("List is empty %d\n", list_is_empty(&q.qhead)); 
    return 0;
}
