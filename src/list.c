#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    int data;
    struct node *next;
}node_t;

node_t *head = NULL;

void add(node_t *next)
{
    node_t *temp = head;

    if (!head)
        head = next;
    else {
        while(temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = next;
    }
}

void del(int data)
{
    node_t *temp, *temp1;

    if (!head)
        return;
    else {
        temp = head;
        temp1 = head;
        while(temp1 && temp1->data != data) {
            temp = temp1;
            temp1 = temp->next;
        }
        if (temp1 == NULL)
            return;
        else {
            if (temp == temp1) {
                head = temp->next;
                free(temp);
                return;
            } else {
                temp->next = temp1->next;
                free(temp1);
                return;
            }
        }
    }
}

void print(node_t *head)
{
    node_t *temp = head;

    while (temp) {
        printf("%d ", temp->data);
        temp = temp->next;
    }

    printf("\n");
}

void sorted_add(node_t *next)
{
    node_t *temp, *temp1;
    temp = temp1 = head;

    while(temp1 && temp1->data > next->data) {
        temp = temp1;
        temp1 = temp1->next;
    }

    if (temp1 == NULL) {
        if (temp == temp1) {
            head = next;
            return;
        } else {
            temp->next = next;
        }
    } else {
        if (temp == temp1) {
            head = next;
            next->next = temp; 
        } else {
            temp->next = next;
            next->next = temp1;
        }
    }
}

void reverse_print(node_t *next)
{
    if (next->next)
        reverse_print(next->next);
    printf("%d ", next->data);
}

node_t * reverse_list(node_t *node, node_t **list_head)
{
    node_t *temp = NULL;

    if (node->next) {
        temp = reverse_list(node->next, list_head);
        temp->next = node;
        return node;
    } else {
        *list_head = node;
        return node;
    }
}

node_t *merge_list = NULL;

node_t * list_concat(node_t *node1, node_t *node2)
{
    node1->next = node2;
    return node1;
}

node_t *merge_lists(node_t *list1, node_t *list2)
{
    if (list1 == NULL) return list2;
    if (list2 == NULL) return list1;

    if (list1->data > list2->data) {
        return list_concat(list1, merge_lists(list2, list1->next));
    } else {
        return list_concat(list2, merge_lists(list1, list2->next));
    }
}

int main()
{
    int a;
    int i = 0;
    printf("Enter no. of nos:");
    scanf("%d", &a);

    for (i = 0; i < a; i++) {
        int temp = 0;
        printf("Enter %d no:", (i+1));
        scanf("%d", &temp);
        node_t *node = (node_t *)malloc(sizeof(node_t));
        memset(node, 0, sizeof(node_t));
        node->data = temp;
        sorted_add(node);
    }

    print(head);
    reverse_print(head);
    printf("\n");
    node_t *new_head = NULL;
    node_t *test = reverse_list(head, &new_head);
    test->next = NULL;
    print(head);
    print(new_head);

    printf("Enter the number to del:");
    scanf("%d", &a);
    del(a);

    print(head);
    
    return 0;
}
