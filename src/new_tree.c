#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define offsetof(type, mbr) \
    ((size_t)&(((type *)0)->mbr))

#define container_of(ptr, type, mbr) ({\
    const typeof(((type *)0)->mbr)* __mptr = ptr;\
    (type *)((char *)__mptr - (char *)offsetof(type, mbr));})

#define max(a,b) (a ^ ((a^b) & -(a<b)))
#define min(a,b) (b ^ ((a^b) & -(a<b)))

#define abs_val(x) ((x+(x>>31)) ^ (x>>31))

#define swap(a,b) {\
    (a) = ((a) ^ (b));\
    (b) = ((a) ^ (b));\
    (a) = ((a) ^ (b));\
}

typedef struct list_head {
    struct list_head *next, *prev;
}lh;

void init_list_head(lh *head)
{
    head->prev = head;
    head->next = head;
}

void __list_add(lh *new, lh *prev, lh *next)
{
    new->next = next;
    new->prev = prev;
    prev->next = new;
    next->prev = new;
}

void list_add(lh *head, lh *node)
{
    __list_add(node, head, head->next);
}

void list_add_tail(lh *head, lh *node)
{
    __list_add(node, head->prev, head);
}

void __list_del(lh *prev, lh *next)
{
    prev->next = next;
    next->prev = prev;
}

void list_del(lh *node)
{
    __list_del(node->prev, node->next);
}

bool list_is_empty(lh *head)
{
    return head->next == head;
}

typedef struct tree_node {
    int data;
    struct tree_node *left;
    struct tree_node *right;
    struct tree_node *next_right; //connecting nodes at same level
    struct tree_node *parent;
}tn;

typedef struct stack {
    lh st_head;
    int size;
}stk;

typedef struct stack_node {
    tn *node;
    lh sn_next;
}sn;

typedef struct queue {
    lh q_head;
    int size;
}que;

typedef struct queue_node {
    tn *node;
    lh qn_next;
}qn;

void init_stack(stk *stack)
{
    stack->size = 0;
    init_list_head(&stack->st_head);
}

bool stack_is_empty(stk *stack)
{
    return list_is_empty(&stack->st_head);
}

void push(stk *stack, tn *node)
{
    sn *sn1 = malloc(sizeof(sn));

    sn1->node = node;
    init_list_head(&sn1->sn_next);

    list_add(&stack->st_head, &sn1->sn_next);

    stack->size++;

    return;
}

sn * pop(stk *stack)
{
    sn *sn1 = NULL;

    if(!stack_is_empty(stack))
    {
        sn1 = container_of(stack->st_head.next, sn, sn_next);
        list_del(stack->st_head.next);
        stack->size--;
    }

    return sn1;
}

void free_stack_node(sn *sn1)
{
    free(sn1);
    return;
}

int stack_size(stk *stack)
{
    return (stack) ? stack->size : 0;
}

void init_queue(que *queue)
{
    queue->size = 0;
    init_list_head(&queue->q_head);
}

bool queue_is_empty(que *queue)
{
    return list_is_empty(&queue->q_head);
}

void enqueue(que *queue, tn *node)
{
    qn *qn1 = malloc(sizeof(qn));

    qn1->node = node;
    init_list_head(&qn1->qn_next);

    list_add_tail(&queue->q_head, &qn1->qn_next);

    queue->size++;

    return;
}

qn * dequeue(que *queue)
{
    qn *qn1 = NULL;

    if (!queue_is_empty(queue))
    {
        qn1 = container_of(queue->q_head.next, qn, qn_next);
        list_del(queue->q_head.next);
        queue->size--;
    }

    return qn1;
}

void free_queue_node(qn *qn1)
{
    free(qn1);
    return;
}

int queue_size(que *queue)
{
    return (queue) ? queue->size : 0;
}

tn * new_tree_node(int data)
{
    tn *node = malloc(sizeof(tn));

    node->data = data;
    node->left = node->right = node->next_right = node->parent = NULL;

    return node;
}

void free_tree_node(tn * node)
{
    free(node);
    return;
}

void print_in_order(tn *node)
{
    if(node && node->left) print_in_order(node->left);
    if(node) printf("%d ", node->data);
    if(node && node->right) print_in_order(node->right);
}

void print_pre_order(tn *node)
{
    if(node) printf("%d ", node->data);
    if(node && node->left) print_pre_order(node->left);
    if(node && node->right) print_pre_order(node->right);
}

void print_post_order(tn *node)
{
    if(node && node->left) print_post_order(node->left);
    if(node && node->right) print_post_order(node->right);
    if(node) printf("%d ", node->data);
}

void print_reverse_in_order(tn *node)
{
    if(node && node->right) print_reverse_in_order(node->right);
    if(node) printf("%d ", node->data);
    if(node && node->left) print_reverse_in_order(node->left);
}

void print_level_order(tn *node)
{
    que lvl_queue;

    init_queue(&lvl_queue);

    if (!node)
    {
        return;
    }
    enqueue(&lvl_queue, node);

    while(!queue_is_empty(&lvl_queue))
    {
        qn *qn1 = dequeue(&lvl_queue);

        printf("%d ", qn1->node->data);

        if (qn1->node->left) {
            enqueue(&lvl_queue, qn1->node->left);
        }

        if (qn1->node->right) {
            enqueue(&lvl_queue, qn1->node->right);
        }

        free_queue_node(qn1);
    }
    return;
}

int height(tn *node)
{
    if (!node) 
        return 0;
    return 1 + max(height(node->left), height(node->right));
}
/* Diameter is number of nodes on the longest path between two leaves in the tree */
int diameter(tn *node, int *ht)
{
    int lh = 0, rh = 0;
    int ldia = 0, rdia = 0;

    if (!node) {
        *ht = 0;
        return 0;
    }

    ldia = diameter(node->left, &lh);
    rdia = diameter(node->right, &rh);

    *ht = 1 + max(lh, rh);

    return max((lh+rh+1), max(ldia, rdia));
}

void print_in_order_without_recursion(tn *node)
{
    tn *curr = node;
    stk stack;
    init_stack(&stack);

    while(!stack_is_empty(&stack) || curr)
    {
        if (curr) {
            push(&stack, curr);
            curr = curr->left;
        } else {
            sn *sn1 = pop(&stack);
            printf("%d ", sn1->node->data);
            curr = sn1->node->right;
            free_stack_node(sn1);
        }
    }
}

void print_pre_order_without_recursion(tn *node)
{
    tn *curr = node;
    stk stack;
    init_stack(&stack);

    while(!stack_is_empty(&stack) || curr)
    {
        if (curr) {
            push(&stack, curr);
            printf("%d ", curr->data);
            curr = curr->left;
        } else {
            sn *sn1 = pop(&stack);
            curr = sn1->node->right;
            free_stack_node(sn1);
        }
    }
}

void print_post_order_without_recursion(tn *node)
{
    tn *curr = node;
    stk stack;
    init_stack(&stack);
    printf("Printing post order in reverse\n");
    while(!stack_is_empty(&stack) || curr)
    {
        if (curr) {
            push(&stack, curr);
            printf("%d ", curr->data);
            curr = curr->right;
        } else {
            sn *sn1 = pop(&stack);
            curr = sn1->node->left;
            free_stack_node(sn1);
        }
    }
}

int replace_tree_with_greater_sum(tn *node, int sum)
{
    if (!node) return 0;
    int val = node->data;
    sum = replace_tree_with_greater_sum(node->right, sum);
    node->data = sum;
    sum += val;
    sum = replace_tree_with_greater_sum(node->left, sum);
    return sum;
}

void mirror(tn *node)
{
    if (node && node->left) mirror(node->left);
    if (node && node->right) mirror(node->right);
    if (node) {
        tn *temp = node->left;
        node->left = node->right;
        node->right = temp;
    }
    return;
}

//Use this same technique for
//1) connecting adjacent pointers (next_right)
//2) Max/min level sum
int max_width_of_tree(tn *node)
{
    int count = 0, max_width = 0;
    que wd_queue;
    tn *curr = node;

    init_queue(&wd_queue);

    enqueue(&wd_queue, curr);

    while(!queue_is_empty(&wd_queue))
    {
        count = queue_size(&wd_queue);
        max_width = max(max_width, count);

        while(count--) {
            qn *qn1 = dequeue(&wd_queue);
            if (qn1->node->left)
                enqueue(&wd_queue, qn1->node->left);
            if (qn1->node->right)
                enqueue(&wd_queue, qn1->node->right);
            free_queue_node(qn1);
        }
    }

    return max_width;
}

void print_nodes_at_k_dist_from_root(tn *node, int k)
{
    if (!node) return;

    if (k == 0)
    {
        printf("%d ", node->data);
    }
    else
    {
        print_nodes_at_k_dist_from_root(node->left, k-1);
        print_nodes_at_k_dist_from_root(node->right, k-1);
    }

    return;
}

bool print_ancestor(tn *node, int key)
{
    bool flag = false;

    if (!node) return flag;

    if (node->data == key) {
        printf("%d ", node->data);
        flag = true;
    } else {
        if (print_ancestor(node->left, key)) {
            printf("%d ", node->data);
            flag = true;
        }
        if (print_ancestor(node->right, key)) {
            printf("%d ", node->data);
            flag = true;
        }
    }

    return flag;
}

bool is_subtree(tn *node1, tn *node2)
{
    if (!node1 && !node2) return true;
    if (!node1 || !node2) return false;

    if ((node1->data == node2->data) &&
        is_subtree(node1->left, node2->left) &&
        is_subtree(node1->right, node2->right)) {
        return true;
    } else {
        return (is_subtree(node1->left, node2) || is_subtree(node1->right, node2));
    }
}

void connect_nodes_at_same_level(tn *node)
{
    que lvl_queue;
    int count = 0;
    init_queue(&lvl_queue);

    if(!node) return;

    enqueue(&lvl_queue, node);

    while(!queue_is_empty(&lvl_queue))
    {
        count = queue_size(&lvl_queue);

        tn *prev = NULL;

        while (count--)
        {
            qn *qn1 = dequeue(&lvl_queue);
            if (prev) {
                prev->next_right = qn1->node;
                printf("%d --> %d ", prev->data, qn1->node->data);
            }

            qn1->node->next_right = NULL;

            if (qn1->node->left)
                enqueue(&lvl_queue, qn1->node->left);

            if (qn1->node->right)
                enqueue(&lvl_queue, qn1->node->right);

            prev = qn1->node;
            free_queue_node(qn1);
        }
    }

    return;
}

int print_nodes_having_k_leaves(tn *node, int k)
{
    if (!node) return 0;

    if (node->left == NULL && node->right == NULL) return 1;

    int num_leaves = print_nodes_having_k_leaves(node->left, k) +
                     print_nodes_having_k_leaves(node->right, k);

    if (num_leaves == k) printf("%d ", node->data);

    return num_leaves;
}

tn * insert_bst(tn *node, int key)
{
    if (!node)
        return new_tree_node(key);

    if (node->data > key) {
        node->left = insert_bst(node->left, key);
    } else {
        node->right = insert_bst(node->right, key);
    }

    return node;
}

tn * search_bst(tn *node, int key)
{
    if (!node) return NULL;

    if (key < node->data)
        return search_bst(node->left, key);

    if (key > node->data)
        return search_bst(node->right, key);

    //node->key == key
    return node;
}

tn * minValNode(tn *node)
{
    if (node && node->left)
        return minValNode(node->left);
    if (node)
        return node;
    return NULL;
}

tn * maxValNode(tn *node)
{
    if (node && node->right)
        return maxValNode(node->right);
    if (node)
        return node;
    return NULL;
}

tn * delete_bst(tn *node, int key)
{
    tn *temp = node;

    if (!node) return NULL;

    if (key < node->data) {
        node->left = delete_bst(node->left, key);
    }
    else if (key > node->data) {
        node->right = delete_bst(node->right, key);
    } else {
        //key should be matching root->key
        //Case 1: No children
        if(node->left == NULL && node->right == NULL)
        {
            free_tree_node(node);
            temp = NULL;
        }
        //Case 2: One child
        else if(node->left != NULL && node->right == NULL) {
            temp = node->left;
            free_tree_node(node);
        } else if(node->right != NULL && node->left == NULL) {
            temp = node->right;
            free_tree_node(node);
        }
        //Case 3: 2 children
        else if (node->left != NULL && node->right != NULL) {
            tn *temp = minValNode(node->right);
            node->data = temp->data;
            node->right = delete_bst(node->right, node->data);
            temp = node;
        }
    }
    return temp;
}

bool isbinaryTreeBst(tn *node, int min, int max)
{
    if (!node) return true;

    if (node->data < min || node->data > max)
        return false;

    return isbinaryTreeBst(node->left, min, node->data-1) &&
           isbinaryTreeBst(node->right, node->data+1, max);
}

bool isBST(tn *root, tn **prev)
{
    if (!root) return true;
    if (!isBST(root->left, prev)) return false;
    if (*prev != NULL && (*prev)->data >= root->data) return false;
    *prev = root;
    return isBST(root->right, prev);
}

void findpresuc(tn *node, int key, tn *pre, tn *suc)
{
    if (!node) return;

    if (key == node->data)
    {
        //predecessor is the largest element in the left sub-tree
        if (node->left)
        {
            pre = maxValNode(node->left);
        }

        //successor is the smallest element in the right sub-tree
        if (node->right)
        {
            suc = minValNode(node->right);
        }

        return;
    } else if (key < node->data) {
        suc = node;
        findpresuc(node->left, key, pre, suc);
    } else if (key > node->data) {
        pre = node;
        findpresuc(node->right, key, pre,suc);
    }

    return;
}

bool find_path(tn *node, int key, stk *path_stack)
{
    if (!node) return false;

    push(path_stack, node);

    if (node->data == key) return true;

    if ((key < node->data && find_path(node->left, key, path_stack)) ||
        (key > node->data && find_path(node->right, key, path_stack)))
        return true;

    //This node is not in the path of the key. Hence remove this node from path stack
    sn *sn1 = pop(path_stack);
    free_stack_node(sn1);
    return false;
}

tn* lca(tn *node, int key1, int key2)
{
    if (!node) return NULL;

    stk path_stack1, path_stack2;

    init_stack(&path_stack1);
    init_stack(&path_stack2);

    //If either one of the keys are not found, return NULL
    if (!find_path(node, key1, &path_stack1) ||
        !find_path(node, key2, &path_stack2))
        return NULL;
    
    int size1 = stack_size(&path_stack1); int size2 = stack_size(&path_stack2);

    if (size1 < size2) {
        while(size1 != size2) {
            sn *sn1 = pop(&path_stack2);
            free_stack_node(sn1);
            size2--;
        }
    }

    if (size1 > size2) {
        while(size1 != size2) {
            sn *sn1 = pop(&path_stack1);
            free_stack_node(sn1);
            size1--;
        }
    }
    
    for (int i = 0; i < size1; i++)
    {
        sn *sn1 = pop(&path_stack1); sn *sn2 = pop(&path_stack2);

        tn *temp = sn1->node;

        if (sn1->node->data == sn2->node->data) {
            return temp;
        }

        free_stack_node(sn1);
        free_stack_node(sn2);
    }
    
    //If control has reached here means no lca found
    return NULL;
}

tn *lca_binary_tree(tn *node, tn *p, tn *q)
{
    if (!node || node == p || node == q) return node;
    tn *left = lca_binary_tree(node->left, p, q);
    tn *right = lca_binary_tree(node->right, p, q);
    return !left ? right : !right ? left : node;
}

void swap_nodes( int* a, int* b )
{
    int t = *a;
    *a = *b;
    *b = t;
}

//Problem: Two nodes of a BST are swapped, correct the BST
//This function does inorder traversal to find out the two swapped nodes.
// It sets three pointers, first, middle and last.  If the swapped nodes are
// adjacent to each other, then first and middle contain the resultant nodes
// Else, first and last contain the resultant nodes
void correctBSTUtil( tn* node, tn** first,
        tn** middle, tn** last,
        tn** prev )
{
    if( node )
    {
        // Recur for the left subtree
        correctBSTUtil( node->left, first, middle, last, prev );

        // If this node is smaller than the previous node, it's violating
        // the BST rule.
        if (*prev && node->data < (*prev)->data)
        {
            // If this is first violation, mark these two nodes as
            // 'first' and 'middle'
            if ( !*first )
            {
                *first = *prev;
                *middle = node;
            }
            // If this is second violation, mark this node as last
            else
                *last = node;
        }
        // Mark this node as previous
        *prev = node;
        // Recur for the right subtree
        correctBSTUtil( node->right, first, middle, last, prev );
    }
}

// A function to fix a given BST where two nodes are swapped.  This
// function uses correctBSTUtil() to find out two nodes and swaps the
// nodes to fix the BST
void correctBST( tn* node )
{
    // Initialize pointers needed for correctBSTUtil()
    tn *first, *middle, *last, *prev;
    first = middle = last = prev = NULL;

    // Set the poiters to find out two nodes
    correctBSTUtil( node, &first, &middle, &last, &prev );

    // Fix (or correct) the tree
    if( first && last )
        swap_nodes( &(first->data), &(last->data) );
    else if( first && middle ) // Adjacent nodes swapped
        swap_nodes( &(first->data), &(middle->data) );

    // else nodes have not been swapped, passed tree is really BST.
}

/*
 *  helper function -- given two circular doubly linked
 *   lists, append them and return the new list.
 *   */
static tn * append(tn *a, tn *b) {
    tn *aLast, *bLast;

    if (a==NULL) return(b);
    if (b==NULL) return(a);

    aLast = a->left;
    bLast = b->left;

    aLast->right = b;
    b->left = aLast;
    bLast->right = a;
    a->left = bLast;

    return(a);
}

tn * treetoCLL(tn *node)
{
    tn *left_node = NULL, *right_node = NULL;

    if(!node) return NULL;

    if (node->left) {
        left_node = treetoCLL(node->left);
    }
    if (node->right) {
        right_node = treetoCLL(node->right);
    }

    node->left = node->right = node;

    left_node = append(left_node, node);
    left_node = append(left_node, right_node);

    return left_node;

}

tn *CLLtoTree1(tn *head, tn *tail)
{
    if (head==tail) return NULL;

    if (tail == NULL) tail = head;

    tn *sp = head, *fp = head;

    while(fp->right != tail && fp->right->right != tail) {
        sp = sp->right;
        fp = fp->right->right;
    }

    tn *root = sp;
    root->left = CLLtoTree1(head, sp);
    root->right = CLLtoTree1(sp->right, tail);

    return root;
}


tn *CLLtotree(tn **node, int count)
{
    if (count <= 0)
        return NULL;

    tn *left = CLLtotree(node, count/2);

    tn *root = *node;
    root->left = left;

    (*node) = (*node)->right;

    root->right = CLLtotree(node, (count - (count/2) - 1));

    return root;
}

int main()
{
    tn *root = new_tree_node(1);
    root->left = new_tree_node(2);
    root->right = new_tree_node(3);
    root->left->left = new_tree_node(4);
    root->left->right = new_tree_node(5);
    root->right->left = new_tree_node(6);
    root->right->right = new_tree_node(7);
    root->right->right->left = new_tree_node(8);
    //root->left->right->left = new_tree_node(8);

    print_level_order(root); printf("\n");
    print_in_order(root); printf("\n");
    print_pre_order(root); printf("\n");
    print_post_order(root); printf("\n");
    mirror(root);
    print_in_order_without_recursion(root); printf("\n");
    mirror(root);
    print_in_order_without_recursion(root); printf("\n");
    int ht = 0;
    printf("ht of tree %d\n", diameter(root, &ht));
    printf("max_width of tree %d\n", max_width_of_tree(root));
    print_ancestor(root, 8); printf("\n");
    tn *root1 = new_tree_node(2);
    root1->left = new_tree_node(4);
    root1->right = new_tree_node(5);
    root1->right->left = new_tree_node(8);
    printf("%d \n", is_subtree(root, root1));
    connect_nodes_at_same_level(root); printf("\n");
    print_nodes_having_k_leaves(root, 1); printf("\n");


    tn* bst_root = insert_bst(NULL, 5);
    insert_bst(bst_root, 2);
    insert_bst(bst_root, 3);
    insert_bst(bst_root, 7);
    insert_bst(bst_root, 1);
    insert_bst(bst_root, 6);
    insert_bst(bst_root, 8);

    print_in_order(bst_root); printf("\n");
    print_reverse_in_order(bst_root); printf("\n");
    print_pre_order(bst_root); printf("\n");
    printf("pre order without recursion\n");
    print_pre_order_without_recursion(bst_root);printf("\n");
    print_post_order(bst_root); printf("\n");
    print_post_order_without_recursion(bst_root);printf("\n");
    printf("%d\n", search_bst(bst_root, 6)->data);
    delete_bst(bst_root, 7);
    print_in_order(bst_root); printf("\n");
    insert_bst(bst_root, 7);
    print_level_order(bst_root); printf("\n");
    tn *lca_node = lca(bst_root, 8,17);
    printf("LCA is %d\n", (lca_node)?lca_node->data:0);


    tn *test_node = treetoCLL(bst_root);
    tn *temp = test_node;
    while(temp->right != test_node)
    {
        printf("%d ", temp->data);
        temp = temp->right;
    }

    printf("%d ", temp->data);
    printf("\n");
    printf("cll to tree1\n");
    bst_root = CLLtoTree1(test_node, NULL);

    //bst_root = CLLtotree(&test_node, 7);
    print_level_order(bst_root); printf("\n");

    return 0;
}
