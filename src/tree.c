#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    int data;
    struct node *right;
    struct node *left;
}node_t;

void insert_node(node_t *root, node_t *node)
{
    if (root && node && node->data < root->data) {
        if (root->left)
            insert_node(root->left, node);
        else 
            root->left = node;
    } else if (root && node && node->data > root->data) {
        if (root->right)
            insert_node(root->right, node);
        else
            root->right = node;
    }
}

int height(node_t *node)
{
    if (node == NULL)
        return 0;
    else {
        int lh = height(node->left);
        int rh = height(node->right);

        return (lh > rh) ? (lh+1):(rh+1);
    }
}

void print_given_level(node_t *node, int level)
{
    if (node == NULL)
        return;
    if (level == 1)
        printf("%d ", node->data);
    if (level > 1) {
        print_given_level(node->left, level-1);
        print_given_level(node->right, level-1);
    }
}

void print_level_order_tree(node_t *node)
{
    int ht = height(node);
    printf("height of tree %d\n", ht);
    int i = 0;
    for (i = 1; i <= ht; i++) {
        print_given_level(node, i);
    }
}

node_t *newNode(int data)
{
    node_t *temp = (node_t *)malloc(sizeof(node_t));
    memset(temp, 0, sizeof(node_t));
    temp->data = data;
    return temp;
}

void print_reverse_in_order_tree(node_t *node)
{
    if (node->right) print_reverse_in_order_tree(node->right);
    printf("%d ", node->data);
    if (node->left) print_reverse_in_order_tree(node->left);
}

void print_in_order_tree(node_t *node)
{
    if (node->left) print_in_order_tree(node->left);
    printf("%d ", node->data);
    if (node->right) print_in_order_tree(node->right);
}

void print_post_order_tree(node_t *node)
{
    if (node->left) print_post_order_tree(node->left);
    if (node->right) print_post_order_tree(node->right);
    printf("%d ", node->data);
}

void print_pre_order_tree(node_t *node)
{
    printf("%d ", node->data);
    if (node->left) print_pre_order_tree(node->left);
    if (node->right) print_pre_order_tree(node->right);
}

int search_tree(node_t *node, int data)
{
    if (node && node->data == data) {
        return 1;
    } else if (node && node->data > data) {
        return search_tree(node->left, data);
    } else if (node && node->data < data) {
        return search_tree(node->right, data);
    } else {
        return 0;
    }
}

node_t * minValueNode(node_t *root)
{
    if (root == NULL)
        return NULL;
    else if (root->left) {
        return minValueNode(root->left);
    } else
        return root;
}

node_t * maxValueNode(node_t *root)
{
    if (root == NULL)
        return NULL;
    else if (root->right) {
        return maxValueNode(root->right);
    } else
        return root;
}

node_t * deleteNode(node_t *root, int key)
{
    if (root == NULL)
        return NULL;
    if (root->data > key)
        root->left = deleteNode(root->left, key);
    else if (root->data < key)
        root->right = deleteNode(root->right, key);
    else {    
        if (root->left == NULL && root->right == NULL) {
            free(root);
            root = NULL;
        }
        else if (root->left && root->right == NULL) {
            node_t *temp = root;
            root = root->right;
            free(temp);
        }
        else if (root->right && root->left == NULL) {
            node_t *temp = root;
            root = root->right;
            free(temp);
        } else {
            node_t *temp = minValueNode(root->right);
            root->data = temp->data;
            root->right = deleteNode(root->right, temp->data);
        }
    }

    return root;
}

struct node *mirror(struct node *root)
{
    if(root==NULL)
        return;
    mirror(root->left);
    mirror(root->right);
    struct node *temp;
    temp=root->left;
    root->left=root->right;
    root->right=temp;
}

//int isBst(node_t *root)
//{
//    int isBst = 1;
//
//    if (root != NULL) {
//        if (root->left) {
//            isBst = (root->data >= root->left->data) && (root->data >= (maxValueNode(root->left))->data) && isBst(root->left);
//        }
//        if (root->right) {
//            isBst = (root->data <= root->right->data) && (root->data <= (minValueNode(root->right))->data) && isBst(root->right);
//        }
//    }
//
//    return isBst;
//}

//Finding LCAncestor in a binary search tree
//Finding LCAncestor in a forest with node having parent pointer
//Replace each node with the sum of all greater nodes in a given BST? (Use reverse in-order sum)

int main()
{
    node_t *root = newNode(5);
    int i = 0;
    for (i = 0; i < 5; i++) {
        int temp;
        scanf("%d", &temp);
        insert_node(root, newNode(temp));
    }

    print_pre_order_tree(root); printf("\n");
    print_post_order_tree(root); printf("\n");
    print_in_order_tree(root); printf("\n");
    print_level_order_tree(root); printf("\n");
    print_reverse_in_order_tree(root); printf("\n");

    printf("Search tree for data:");
    scanf("%d", &i);
    printf("Search res: %d\n", search_tree(root, i));

    printf("Delete data from tree:");
    scanf("%d", &i);
    deleteNode(root, i);
    print_in_order_tree(root); printf("\n");
    print_level_order_tree(root); printf("\n");

    return 0;
}
