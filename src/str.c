#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <ctype.h>

#define max(x,y) (x^((x^y)&-(x<y)))

void str_reverse(char str[])
{
    int len = strlen(str);

    for (int i = 0; i < len/2; i++)
    {
        str[i] = str[i] ^ str[len-i-1];
        str[len-i-1] = str[i] ^ str[len-i-1];
        str[i] = str[i] ^ str[len-i-1];
    }

    return;
}

void int_to_ascii(int a)
{
    char str_arr[32] = {'\0'};
    int j = 0;

    while(a) {
        str_arr[j++]  = '0'+a%10;
        a /= 10;
    }

    str_reverse(str_arr);

    printf("%s\n", str_arr);
}

void int_to_ascii_new(int a)
{
    char str_arr[32] = {'\0'};
    int j = 0;
    int n = floor(log10(a)) + 1; //no. of digits in a

    int mask = pow(10, n-1);

    while(a) {
        str_arr[j++]  = '0'+(a/mask);
        a %= mask;
        mask /= 10;
    }

    //str_reverse(str_arr);

    printf("%s\n", str_arr);
}

int ascii_to_int(char str[])
{
    int result = 0;

    for (int i = 0; i < strlen(str); i++)
    {
        result = result * 10 + str[i] - '0';
    }

    printf("%d\n", result);

    return result;
}

void reverse_words_in_a_sentence(char *str, char *res, int flag)
{
    char *tok;
    char *dlim = " ";
    if (flag) {
        tok = strtok(NULL, " ");
    } else {
        tok = strtok(str, " ");
    }

    if (tok == NULL) return;
    reverse_words_in_a_sentence(str, res, 1);
    strcat(res, tok);
    strcat(res, dlim);
    return;
}

void printcount(int *arr)
{
    for (int i = 0; i < 26; i++)
        printf("%d ", arr[i]);
    printf("\n");
}


bool check_count(int *arr1, int *arr2)
{
    //printcount(arr1);
    //printcount(arr2);
    //printf("\n");
    for (int i = 0; i < 26 ; i++)
    {
        if (arr1[i] <= 0) continue;
        if (arr2[i] < arr1[i]) return false;
    }

    //printf("return true\n");
    return true;
}
//Find the smallest window in S containing all chars of t
int window_containing_other_string(char *s, char *t)
{
    int count[26] = {0}, copy_count[26] = {0};

    for (int i = 0; i < strlen(t); i++) {
        count[tolower(t[i])-'a']++;
    }
    //printcount(count);

    int start = 0, end = 0;
    int min_window = 100000;
    for (int i = 0; i < strlen(s); i++)
    {
        copy_count[tolower(s[i])-'a']++;
        end++;

        if (!check_count(count, copy_count)) continue;

        while(1) {
            copy_count[tolower(s[start])-'a']--;
            if (check_count(count, copy_count)) {
                start++;
            } else {
                copy_count[tolower(s[start])-'a']++;
                break;
            }
        }
        if (end-start < min_window) {
            min_window = end-start;
        }
    }

    return min_window;

}

bool valid_count(int *arr, int k)
{
    int ret = 0;
    for (int i = 0; i < 26; i++) {
        if (arr[i] > 0) ret++;
    }

    return ret <= k;
}

int longest_substr_with_k_chars(char *s, int k)
{
    int count[26] = {0};

    int start = 0, end = 0, sub_start = 0, sub_size = 0;

    for (int i = 0; i < strlen(s); i++) {

        count[tolower(s[i])-'a']++;
        end++;

        while(!valid_count(count, k)) {
            count[tolower(s[start])-'a']--;
            start++;
        }

        if (end-start > sub_size) {
            sub_start = start;
            sub_size = end-start;
        }
    }

    return sub_size;
}

bool check_long_count(int *arr)
{
    for (int i = 0; i < 26; i++)
    {
        if (arr[i] > 1) return false;
    }

    return true;
}

int longest_substr_with_distinct_chars(char *s)
{
    int count[26] = {0};

    int start = 0, end = 0, size = 0;

    for (int i = 0; i < strlen(s); i++) {
        count[tolower(s[i])-'a']++;
        end++;

        while(!check_long_count(count)) {
            count[tolower(s[start])-'a']--;
            start++;
        }

        if (end-start > size) {
            printf("%d %d\n", start, end);
            size = end-start;
        }
    }
    return size;
}

bool check_palindromic_str(char *s, int start, int end)
{
    int len = end - start;
    int count[26] = {0};

    if (len == 1) return true;

    for(int i = start; i < (len+start); i++) {
        count[tolower(s[i])-'a']++;
    }
    bool flag = false;
    for (int i = 0; i < 26; i++) {
        if (count[i]%2 == 1) {
            if (flag == true) return false;
            flag = true;
        }
    }
    return true;
}

int longest_palindromic_substring(char *s, int start, int end, int mem[32][32])
{
    if (start == end) return 0;

    if (mem[start][end] != -1)
        return mem[start][end];

    bool choice1 = check_palindromic_str(s, start, end);
    int choice2 = 0, choice3 = 0; 

    if (!choice1) {
        choice2 = longest_palindromic_substring(s, start+1, end, mem);
        choice3 = longest_palindromic_substring(s, start, end-1, mem);
    }

    mem[start][end] =  (choice1) ? (end-start): max(choice2, choice3);
    return mem[start][end];
}

bool chk_palindrome(char *s, int start, int end)
{
    int size = end-start;
    for (int i = start; i < start+size/2; i++) {
        if (s[i] != s[size+start-1-i+start]) {
            return false;
        }
    }

    return true;
}


int longest_palindromic_substring1(char *s, int start, int end, int mem[32][32])
{
    if (start == end) return 0;

    if (mem[start][end] != -1)
        return mem[start][end];

    bool choice1 = chk_palindrome(s, start+1, end);
    bool choice2 = chk_palindrome(s, start, end-1);

    int choice3 = 0, choice4 = 0; 
    if (!choice1 && !choice2) {
        choice3 = longest_palindromic_substring1(s, start+1, end, mem);
        choice4 = longest_palindromic_substring1(s, start, end-1, mem);
    }

    int ret = 0;
    if (choice1) ret = end - (start+1);
    else if (choice2) ret = end-1 - start;
    else ret = max(choice3, choice4);
    mem[start][end] = ret; 
    return mem[start][end];
}

int count_no_of_palindromes(char *s, int start, int end)
{
    if (start == end) return 0;

    bool choice1 = check_palindromic_str(s, start, end);
    int choice2 = 0, choice3 = 0;
    //if (!choice1) {
        choice2 = count_no_of_palindromes(s, start+1, end);
        choice3 = count_no_of_palindromes(s, start, end-1);
    //}

    return choice1?1+(choice2+choice3):(choice2+choice3);
}

int longest_common_subsequence(char *str1, char *str2, int len1, int len2)
{
    if (len1 == strlen(str1) || len2 == strlen(str2)) return 0;

    if (str1[len1] == str2[len2])
        return 1+longest_common_subsequence(str1, str2, len1+1, len2+1);

    return max(longest_common_subsequence(str1, str2, len1+1, len2),
               longest_common_subsequence(str1, str2, len1, len2+1));
}

int main()
{
    int_to_ascii_new(52987345);
    //ascii_to_int("52987345");
    char res[32] = {'\0'};
    char a[] = "srinivas is a good boy";
    reverse_words_in_a_sentence(a, res, 0);
    printf("%s %s\n", res, a);
    printf("%d\n", window_containing_other_string("this is a test stringr","rin")); 
    printf("%d\n", longest_substr_with_k_chars("mississippi", 2));
    printf("%d\n", longest_substr_with_distinct_chars("BBBB"));
    int mem[32][32];
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < 32; j++) {
            mem[i][j] = -1;
        }
    }
    printf("%d\n", longest_palindromic_substring1("cbbd", 0, 4, mem));
    //printf("%d\n", count_no_of_palindromes("arc", 0, 3)); 
    printf("%d\n", longest_common_subsequence("abcdef", "acbef", 0, 0));
    return 0;
}
