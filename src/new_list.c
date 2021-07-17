#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define offsetof(type, member) \
    ((size_t)&(((type *)0)->member))

#define container_of(ptr, type, member) ({\
    const typeof(((type *)0)->member) *__mptr = ptr;\
    (type *)((char *)__mptr - offsetof(type, member));}) 


typedef struct listhead {
    struct listhead *prev, *next;
}lh;

void init_list_head(lh *head) {
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

void list_add(lh *entry, lh *head)
{
    __list_add(entry, head, head->next);
}

void list_add_tail(lh *entry, lh *head)
{
    __list_add(entry, head->prev, head);
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

typedef struct linkedlist {
    lh lhead;
}ll;

typedef struct listnode {
    int data;
    lh  node;
}ln;

void list_add_node(lh *head, void *data, int data_size)
{
    ln *lnode = (ln *)malloc(sizeof(ln));

    if (!lnode) {
        printf("Error allocating node\n");
        return;
    }

    memset(lnode, 0, sizeof(ln));

    memcpy(&lnode->data, (char *)data, data_size);

    init_list_head(&lnode->node);

    list_add_tail(&lnode->node, head);
}

void remove_duplicates(lh *head)
{
    lh *curr = head->next;

    while (curr->next != head) {
        lh *temp1 = curr->next;
        if (container_of(curr, ln, node)->data == container_of(curr->next, ln, node)->data) {
            list_del(curr);
            ln *temp = container_of(curr, ln, node);
            free(temp);
        }
        curr = temp1;
    }
}

void list_merge(lh *head1, lh *head2, lh *head3)
{
    if (!head1 && !head2) {
        return;
    }
    if (!head1) {
        head3->next = head2->next;
        head3->prev = head2->prev;
        return;
    }

    if (!head2) {
        head3->next = head1->next;
        head3->prev = head1->prev;
        return;
    }

    lh *node1 = head1->next;
    lh *node2 = head2->next;

    while(node1 != head1 && node2 != head2) {
        if (container_of(node1, ln, node)->data < container_of(node2, ln, node)->data) {
            list_add_node(head3, &container_of(node1, ln, node)->data, 4);
            node1 = node1->next;
        } else if (container_of(node1, ln, node)->data > container_of(node2, ln, node)->data) {
            list_add_node(head3, &container_of(node2, ln, node)->data, 4);
            node2 = node2->next;
        } else if (container_of(node1, ln, node)->data == container_of(node2, ln, node)->data) {
            list_add_node(head3, &container_of(node2, ln, node)->data, 4);
            node2 = node2->next; node1 = node1->next;
        }
    }

    if (node1 == head1 && node2 == head2)
    {
        return;
    }

    if (node1 == head1) {
        while (node2 != head2) {
            list_add_node(head3, &container_of(node2, ln, node)->data, 4);
            node2 = node2->next;
        }
    } else if (node2 == head2) {
        while (node1 != head1) {
            list_add_node(head3, &container_of(node1, ln, node)->data, 4);
            node1 = node1->next;
        }
    }

    return;
}

void list_merge2(lh *head1, lh *head2, lh *head3)
{
    lh *node1 = head1->next;
    lh *node2 = head2->next;
    lh *temp;
    
    while(1) {
        if (node1 == head1 && node2 == head2) {
            break;
        }

        if (node1 == head1) {
            node1 = node2;
            head1 = head2;
        }
        if (node2 == head2) {
            node2 = node1;
            head2 = head1;
        }
        
        temp = node1;

        if (container_of(node1, ln, node)->data < container_of(node2, ln, node)->data) {
            temp = node1;
            node1 = node1->next;
        } else if (container_of(node1, ln, node)->data > container_of(node2, ln, node)->data) {
            temp = node2;
            node2 = node2->next;
        } else {
            ln *temp1 = container_of(node2, ln, node);
            if (node1 != node2) {
                node2 = node2->next;
                list_del(&temp1->node);
                free(temp1);
            } else {
                node2 = node2->next;
            }
            node1 = node1->next;
        }

        list_add_tail(temp, head3);
    }
}

void splitlist(lh *lhead, lh *lhead1, lh *lhead2)
{
    lh *fp, *sp;

    fp = sp = lhead;

    while(fp->next != lhead && fp->next->next != lhead) {
        sp = sp->next;
        fp = fp->next->next;
    }

    //If odd no. of nodes are present
    if (fp->next != lhead)
        sp = sp->next;

    lh *temp = sp->next;

    //while(sp != lhead) {
    //    lh *temp1 = sp->prev;
    //    list_add(sp, lhead1);
    //    sp = temp1;
    //}

    //while (temp != lhead) {
    //    lh *temp1 = temp->next;
    //    list_add_tail(temp, lhead2);
    //    temp = temp1;
    //}
    
    lhead1->next = lhead->next;
    lhead->next->prev = lhead1;
    lhead1->prev = sp;
    sp->next = lhead1;

    lhead2->next = temp;
    temp->prev = lhead2;
    lhead2->prev = lhead->prev;
    lhead->prev->next = lhead2;

    return;
}

void merge_sort(lh *lhead)
{
    ll llist1, llist2;
    
    init_list_head(&llist1.lhead);
    init_list_head(&llist2.lhead);

    //List is either empty or there is just one node
    if (lhead->next == lhead || lhead->next->next == lhead)
        return;

    splitlist(lhead, &llist1.lhead, &llist2.lhead);

    lh *pos;

    printf("list1:");
    for (pos = llist1.lhead.next; pos != &llist1.lhead; pos = pos->next)
    {
        printf(" %d ", container_of(pos, ln, node)->data);
    }

    printf("\n");
    printf("list2:");
    for (pos = llist2.lhead.next; pos != &llist2.lhead; pos = pos->next)
    {
        printf(" %d ", container_of(pos, ln, node)->data);
    }

    printf("\n");

    merge_sort(&llist1.lhead);
    merge_sort(&llist2.lhead);

    printf("New list1:");
    for (pos = llist1.lhead.next; pos != &llist1.lhead; pos = pos->next)
    {
        printf(" %d ", container_of(pos, ln, node)->data);
    }

    printf("\n");
    printf("New list2:");
    for (pos = llist2.lhead.next; pos != &llist2.lhead; pos = pos->next)
    {
        printf(" %d ", container_of(pos, ln, node)->data);
    }

    printf("\n");

    init_list_head(lhead);
    list_merge2(&llist1.lhead, &llist2.lhead, lhead); 

    printf("Merged list:");
    for (pos = lhead->next; pos != lhead; pos = pos->next)
    {
        printf(" %d ", container_of(pos, ln, node)->data);
    }

    printf("\n");

    return;
}

bool loop_detect(lh *lhead)
{
    lh *fp, *sp;

    fp = sp = lhead->next;

    while (fp != sp || fp != lhead) {
        fp = fp->next->next;
        sp = sp->next;
    }

    if (fp == sp) {
        printf("Loop found\n");
        //Loop starts at sp->next
    }

    return true;
}

void reverse_k_sublists(lh *lhead, int k, lh *res)
{
    if (k <= 1) {
        return;
    }

    lh *node = lhead->next;
    lh *head = lhead;

    while (1) {

        for (int i = 0; i < k-1; i++) {
            node = node->next;
            if (node == lhead) {
                node = node->prev;
                break;
            }
        }

        lh *temp = node->next;

        while(node != head) {
            lh *temp1 = node->prev;
            list_add_tail(node, res);
            node = temp1;
        }

        //lh *pos;

        //for (pos = res->next; pos != res; pos = pos->next)
        //{
        //    printf(" %d ", container_of(pos, ln, node)->data);
        //}

        //printf("\n");


        node = temp;
        head = temp->prev;

        if (node == lhead) break;
    }
}

void reverse_list(lh *node, lh *head)
{
    if (node == head) {
        lh *temp = head->prev;
        head->prev = head->next;
        head->next = temp;
    } else {
        reverse_list(node->next, head);
        lh *temp = node->prev;
        node->prev = node->next;
        node->next = temp;
    }
    return;
}

int main()
{
    ll llist1, llist2, llist3, llist4, llist5;

    init_list_head(&llist1.lhead);
    init_list_head(&llist2.lhead);
    init_list_head(&llist3.lhead);
    init_list_head(&llist4.lhead);
    init_list_head(&llist5.lhead);

    int i = 5;

    list_add_node(&llist1.lhead, &i, 4); i = 6;
    list_add_node(&llist1.lhead, &i, 4); i = 10;
    list_add_node(&llist1.lhead, &i, 4);

    i = 6;
    list_add_node(&llist2.lhead, &i, 4); i = 9;
    list_add_node(&llist2.lhead, &i, 4); i = 5;
    list_add_node(&llist2.lhead, &i, 4); i = 10;
    list_add_node(&llist2.lhead, &i, 4); i = 7;
    list_add_node(&llist2.lhead, &i, 4); i = 12;
    list_add_node(&llist2.lhead, &i, 4); i = 9;
    list_add_node(&llist2.lhead, &i, 4); i = 14;
    list_add_node(&llist2.lhead, &i, 4);

    //list_merge(&llist1.lhead, &llist2.lhead, &llist3.lhead);
    //lh *temp = list_merge1(llist1.lhead.next, llist2.lhead.next, &llist1.lhead, &llist2.lhead);
    //llist3.lhead.next = temp;
    //list_merge2(&llist1.lhead, &llist2.lhead, &llist3.lhead);
    //remove_duplicates(&llist3.lhead);

    lh *pos;

    for (pos = llist2.lhead.next; pos != &llist2.lhead; pos = pos->next)
    {
        printf(" %d ", container_of(pos, ln, node)->data);
    }

    printf("\n");

    merge_sort(&llist2.lhead);

    //reverse_k_sublists(&llist2.lhead, 5, &llist3.lhead);
    
    for (pos = llist2.lhead.next; pos != &llist2.lhead; pos = pos->next)
    {
        printf(" %d ", container_of(pos, ln, node)->data);
    }

    printf("\n");

    reverse_list(llist2.lhead.next, &llist2.lhead);

    for (pos = llist2.lhead.next; pos != &llist2.lhead; pos = pos->next)
    {
        printf(" %d ", container_of(pos, ln, node)->data);
    }

    printf("\n");

}
