#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#define swap(x,y) {\
    int t=x;\
    x=y;\
    y=t;\
}

#define max(x,y) (x^((x^y)&-(x<y)))

typedef struct tree_node {
    int data;
    struct tree_node *left;
    struct tree_node *right;
}tn;

bool valid_perfect_square(int n)
{
    int low = 1;
    int high = n/2;

    while (low <= high) {
        int mid = low + (high-low)/2;
        unsigned long long sq = mid * mid;
        if (sq == n) return true;
        else if (sq < n) low = mid+1;
        else if (sq > n) high = mid-1;
    }

    return false;
}

void most_freq_elem_in_arr(int *arr, int size)
{
    int count = 0;
    int max_count = 0;
    int val = 0;
    int num_freq = 0;
    for (int i = 0; i < size; i++) {
        if (arr[i] != val) {
            val = arr[i];
            count = 0;
        }
        count++;
        if (count > max_count) {
            max_count = count;
            num_freq = 1;
        } else if (count == max_count) {
            num_freq++;
        }
    }

    printf("%d\n", num_freq);
    return;
}

bool check_same_tree(tn *tree1, tn * tree2)
{
    if (!tree1 && !tree2) return true;
    else if (!tree1 || !tree2) return false;
    else if (tree1->data == tree2->data) {
        return check_same_tree(tree1->left, tree2->left) && check_same_tree(tree1->right, tree2->right);
    } else
        return false;
}

int excelColumnNumber(char *s)
{
    int len = strlen(s);
    int res = 0;
    for (int i = 0; i < len; i++) {
        res = res*26+s[i]-'A'+1;
    }
    return res;
}

char *excelColumnName(int num) {
    char *buf = malloc(32 * sizeof(char));
    memset(buf, '\0', (32*sizeof(char)));
    int i = 0;
    while(num) {
        buf[i++] = (num-1)%26+'A';
        num = (num-1)/26;
    }

    for (int j = 0; j < i/2; j++) {
        swap(buf[j], buf[i-1-j]);
    }

    return buf;
}


//In the below code zero case is not handled
//Also negative number is not handled
char *convertToBase7(int num) {
    char *buf = malloc(32 * sizeof(char));
    memset(buf, '\0', (32*sizeof(char)));
    int i = 0;
    while(num) {
        buf[i++] = num%7+'0';
        num /= 7;
    }

    for (int j = 0; j < i/2; j++) {
        swap(buf[j], buf[i-1-j]);
    }

   return buf; 
}

int convertToDigit(char a)
{
    if (a >= '0' && a <= '9') return a - '0';
    else if (tolower(a) >= 'a' && tolower(a) <= 'f') return 10+(tolower(a) - 'a');
    return -1;
}

int convertToBase(char *str, int base)
{
    int res = 0;
    if (!str) return -1;
    if (base < 2 || (base > 10 && base != 16)) return -1;
    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        int a = convertToDigit(str[i]);
        if (a == -1 || a > base) return -1;
        int exp = len - 1 - i;
        res += a * pow(base, exp);
    }
    return res;
}

int sum_of_squares_of_digits(int num)
{
    int res = 0;
    while(num) {
        res += (num%10) * (num%10);
        num /= 10;
    }

    return res;
}

bool digit_square_sum(int num)
{
    int fast = num, slow = num;

    do {
        slow = sum_of_squares_of_digits(slow);
        fast = sum_of_squares_of_digits(fast);
        fast = sum_of_squares_of_digits(fast);
    }while(slow != fast);

    printf("%d %d\n", slow, fast);
    if(slow == 1) return true;
    return false;
}

int sum(int a, int b)
{
    int carry = 0;
    while(b) {
        carry = a&b;
        a ^= b;
        b = carry<<1;
    }
    return a;
}

int max_depth(tn *root) {
    if (!root) return 0;

    int lh = max_depth(root->left);
    int rh = max_depth(root->right);

    return 1+max(lh,rh);
}

char *reverseString(char *s)
{
    int len = strlen(s);
    int count = 0;

    while(count < len/2) {
        swap(s[count], s[len - 1 - count]);
        count++;
    }

    return s;
}

int main()
{
    char buf[32] = {'\0'};
    sprintf(buf, "%s", "srinivas");
    printf("%s\n", reverseString(buf));
    printf("%d\n", sum(5,-2));
    printf("%d\n", digit_square_sum(342));
    printf("%d\n", convertToBase("14", 16));
    printf("%s\n", convertToBase7(14));
    printf("%d\n", convertToBase(convertToBase7(21), 7));
    printf("%d %s\n", excelColumnNumber("CGTW"), excelColumnName(456167));
    int arr[10] = {1,2,2,2,4,5,4,3,3,3};
    most_freq_elem_in_arr(arr, 10);
    printf("%d\n", valid_perfect_square(5));
    return 0;
}
