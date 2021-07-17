#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdbool.h>

#define max(x,y) (x ^ ((x^y) & -(x<y)))

char* addStrings(char* num1, char* num2) {
    int len1 = strlen(num1);
    int len2 = strlen(num2);
    int carry = 0;
    int size = len1+abs(len2-len1);
    char *ret = malloc(size + 1 + 1);
    memset(ret, '\0', (size+1+1));
    int count = size+1;
    int res = 0;
    while (len1 > 0 || len2 > 0) {
        ret = carry;
        if (len1 > 0 && len2 > 0) {
            res += (num1[len1-1] - '0')+ (num2[len2-1] - '0');
            len1--;
            len2--;
        } else if (len1 > 0) {
            res += (num1[len1-1] - '0');
            len1--;
        } else if (len2 > 0) {
            res += (num2[len2-1] - '0');
            len2--;
        }
        carry = res/10;
        res %= 10;
        ret[count--] = res+'0';
    }

    printf("%d\n", size);
    if (count >= 0) {
        char *ret_arr = malloc(size+1);
        memcpy(ret_arr, ret+1, size);
        free(ret);
        return ret_arr;
    }

    return ret;

}


int longestPalindrome(char* s) {
    int arr[256] = {0};
    int count = 0, size = 0, flag = 0;
    int len = strlen(s);
    for (int i = 0; i < len; i++) {
        arr[s[i]]++;
    }

    for (int i = 0; i < 256; i++) {
        if (arr[i] && arr[i]%2 == 0) {
            printf("%c %d\n", i, arr[i]);
            count +=  arr[i];
            //printf("%c %d\n", i, count);
        } else {
            if (!flag && arr[i] == 1) {
                count += 1;
                flag = 1;
            } else {
                if (arr[i])
                    count += arr[i] - 1;
            }
        }
    }
    printf("%d\n", count+size);
    return count+size;
}

bool isPerfectSquare1(int num) {
    if (num < 1) return false;
    long t = num / 2;
    while (t * t > num) {
        t = (t + num / t) / 2;
    }
    printf("%ld \n", t*t);
    return t * t == num;
}

bool isPerfectSquare(int num) {
    int x = 1;
    if (!num || num == 1)
        return true;
    if (num < 0)
        return false;

    int low = 1;
    int high = num/2;
    while(low <= high) {
        int mid = low + (high - low)/2;
        unsigned long val = mid * mid;
        printf("%d %d %d %lu\n", low, high, mid, val);
        if (val == num) {
            return true;
        }
        if (val < num) {
            low = mid+1;
        } else {
            high = mid-1;
        }
    }
    return false;
}

int digitSquareSum(int x)
{
    int sum = 0;

    while(x) {
        sum += (x%10) * (x%10);
        x /= 10;
    }

    return sum;
}

bool isHappy_floyd(int n)
{
    int x = abs(n);
    int fast = x, slow = x;
    do {
        slow = digitSquareSum(slow);
        fast = digitSquareSum(fast);
        fast = digitSquareSum(fast);
    } while(slow != fast);

    if (slow == 1) return true;
    return false;
}

bool isHappy(int n) {
    int arr[10] = {0};
    int i = 0, count = 0;
    int x = abs(n);

    while(x) {
        count++;
        while (x) {
            arr[i++] = x%10;
            x /= 10;
        }
        for (int j = i-1; j >= 0; j--) {
            x += (arr[j] * arr[j]);
        } 
        i = 0;
        if (x == 1)
            break;
        if (count == 100)
            return false;
    }

    return true;

}

bool isPalindrome(int m) {
    int x = abs(m);
    int n = floor(log10(abs(x))) + 1;
    int mask = pow(10, n-1);

    while (x) {
        if (x/mask != x%10)
            return false;
        x %= mask;
        x /= 10;
        mask /= 100;
    }

    if (m < 0)
        return false;
    return true;
}

bool search_tok(char **arr, char *tok)
{
    for(int i = 0; i < 256; i++) {
        if (arr[i]) {
            if (!strcmp(arr[i], tok))
                return true;
        }
    }
    return false;
}

bool wordPattern(char* pattern, char* str) {
    char **arr = malloc(256 * sizeof(char *));
    memset((char *)arr, 0, sizeof(256 * sizeof(char *)));
    int pat_len = strlen(pattern);
    char *tok = strtok(str, " ");
    for (int i = 0; i < pat_len; i++) {
        if (tok) {
            if (!arr[pattern[i]]) {
                bool val = search_tok(arr, tok);
                if (val)
                    return false;
                arr[pattern[i]] = tok;
                printf("%s %s\n", tok, arr[pattern[i]]);
            } else {
                printf("%s %s\n", tok, arr[pattern[i]]);
                if(strcmp(tok, arr[pattern[i]])) {
                    free(arr);
                    return false;
                }
            }
            tok = strtok(NULL, " ");
        } else {
            return false;
        }
    }
    return true;
}


char** generatePossibleNextMoves(char* s, int* returnSize) {
    char **ret = NULL;
    int count = 0;
    int len = strlen(s);
    printf("%d %d\n", len, len - 2); 
    for (int i = 0; i <= len - 2; i++) {
        if (s[i] == '+' && s[i+1] == '+') {
            char *t = malloc(len+1);
            memset(t, '\0', (len+1));
            strcpy(t, s);
            t[i] = '-'; t[i+1] = '-';
            if (!count) {
                ret = malloc((count+1) * sizeof(char *));
            } else {
                ret = realloc(ret, (count+1));
            }
            ret[count++] = t;
        }
    }
    *returnSize = count;
    for (int i = 0; i < count; i++)
        printf("%s ", ret[i]);
    printf("\n");
    return ret;
}

void merge(int* nums1, int m, int* nums2, int n) {
    int len = m+n;
    int i = 0, j = 0, k = 0;

    for (i = len-1,j=m-1,k=n-1; i >= 0 && j >= 0 && k >= 0; i--)
    {
        if (nums1[j] > nums2[k])
            nums1[i] = nums1[j--];
        else if (nums1[j] < nums2[k])
            nums1[i] = nums2[k--];
        else {//equal
            nums1[i] = nums1[j--];
            nums1[--i] = nums2[k--];
        }
    }

    if (j < 0) {
        for (; i >= 0 && k >= 0; i--)
            nums1[i] = nums2[k--];
    } else if (k < 0) {
        for (; i >= 0 && j >= 0; i--)
            nums1[i] = nums1[j--];
    }

    return;
}

int lengthOfLastWord(char* s) {
    char *temp = NULL;
    if (s)
        temp = strstr(s, " "); 
    char *tok = strtok(s, " ");

    if (!tok)
        return -1;
    else {
        int val = lengthOfLastWord(NULL);
        if (val == -1) {
            if (s && !temp)
                return 0;
            return strlen(tok);
        } else
            return val;
    }
}

int convertBinStrtoInt(char *str)
{
    int len = strlen(str);
    int res = 0;
    for (int i = len-1; i >= 0; i--) {
        if (str[i] - '0') { //This is a one
            res += pow(2,((len-1)-i));
        } else {//This is a zero
        }   
    }   

    return res;
}

void convertInttoBinStr(int a, char *str)
{
    int i = 0;

    if (!a) {
        str[0] = '0';
        return;
    }

    while(a) {
        str[i++] = (a%2)+'0';
        a /= 2;
    }   

    int len = strlen(str);
    for (i = 0; i < len/2; i++)
    {   
        str[i] = str[i] ^ str[(len-1)-i];
        str[(len-1)-i] =  str[i] ^ str[(len-1)-i];
        str[i] = str[i] ^ str[(len-1)-i];
    }   
}

#define max(x,y) (x ^ ((x^y) & -(x<y)))

char* addBinary(char* a, char* b) {
    int alen = strlen(a);
    int blen = strlen(b);
    int result = convertBinStrtoInt(a) + convertBinStrtoInt(b);
    int len = max(alen,blen);
    char *str = malloc(len+1);
    memset(str, '\0', (len+1));
    convertInttoBinStr(result, str);
    printf("%s\n", str);
    return str;
}
bool validWordAbbreviation(char* word, char* abbr) {
    int word_len = strlen(word);
    int abbr_len = strlen(abbr);
    int res = 0;
    int j = 0;

    for (int i = 0; i < abbr_len; i++) {
        if (isalpha(abbr[i])) {
            if (res != 0) {
                j += res;
            }
            if (word[j++] != abbr[i])
                return false;
            res = 0;
        } else if (isdigit(abbr[i])) {
            if (res == 0 && abbr[i] == '0') return false;
            res = (res*10) + (abbr[i] - '0');
        }
    }

    if (res == word_len - j)
        return true;
    else
        return false;
}

int main()
{
    //bool val = validWordAbbreviation("internationalization", "i5a11o1");
    //printf("%d\n", val);
    //addBinary("111011110", "110011010");
    //lengthOfLastWord("srinivas");
    //int arr1[6] = {1,2,3,0,0,0};
    //int arr2[3] = {2,3,6};
    //merge(arr1, 3, arr2, 3);
    //int x = 0;
    //char **ret = generatePossibleNextMoves("---+++-+++-+", &x);
    //free(ret);
    //char *pattern = "abba";
    //char *str = "dog cat cat dog";
    //wordPattern(pattern, str);
    //printf("%d \n", isPalindrome(-2147447412));
    //printf("%d \n", isHappy_floyd(19));
    //isPerfectSquare(2147395600);
    //isPerfectSquare(1000000);
    //longestPalindrome("civilwartestingwhetherthatnaptionoranynartionsoconceivedandsodedicatedcanlongendureWeareqmetonagreatbattlefiemldoftzhatwarWehavecometodedicpateaportionofthatfieldasafinalrestingplaceforthosewhoheregavetheirlivesthatthatnationmightliveItisaltogetherfangandproperthatweshoulddothisButinalargersensewecannotdedicatewecannotconsecratewecannothallowthisgroundThebravelmenlivinganddeadwhostruggledherehaveconsecrateditfaraboveourpoorponwertoaddordetractTgheworldadswfilllittlenotlenorlongrememberwhatwesayherebutitcanneverforgetwhattheydidhereItisforusthelivingrathertobededicatedheretotheulnfinishedworkwhichtheywhofoughtherehavethusfarsonoblyadvancedItisratherforustobeherededicatedtothegreattdafskremainingbeforeusthatfromthesehonoreddeadwetakeincreaseddevotiontothatcauseforwhichtheygavethelastpfullmeasureofdevotionthatweherehighlyresolvethatthesedeadshallnothavediedinvainthatthisnationunsderGodshallhaveanewbirthoffreedomandthatgovernmentofthepeoplebythepeopleforthepeopleshallnotperishfromtheearth");
    addStrings("0", "0");
}

#if 0
int thirdMax(int* nums, int numsSize) {
    //int first = pow(-2,31), second = pow(-2,31), third = pow(-2,31);
    int first = 0, second = 0, third = 0;

    for (int i = 0; i < numsSize; i++) {
        if (nums[i] == first || nums[i] == second || nums[i] == third)
            continue;
        if (nums[i] > first || first == 0) {
            third = second;
            second = first;
            first = nums[i];
        } else if (nums[i] > second || second == 0) {
            third = second;
            second = nums[i];
        } else if (nums[i] > third || third == 0) {
            third = nums[i];
        }
    }

    if (third == 0) return first;
    else return third;
}

int main()
{
    int arr[3] = {1,1,2};
    printf("%08x\n", thirdMax(arr, 3));
}

uint32_t reverseBits(uint32_t n) {
    uint32_t res = 0;
    uint32_t i = 0;

    while (i < 32) {
        res |= (((n & (1<<i)) >> i) << (31-i)); 
        i++;
    }

    printf("%x %d\n", res, res);
    return res;
}


void reverseInt(int x)
{
    long long res = 0;
    int temp = x;
    while (x) {
        res = (res * 10) + x%10;
        x /= 10;
    }

    if (-2147483648 <= res  && res <= 2147483647)
        printf("%x %x\n", temp, (int)res);
    else
        printf("Zero %lld\n", res);
}

int main()
{
    reverseBits(43261596);
    printf("%d\n", isdigit('1'));
    return 0;
}


char* reverseString(char* s) {

    int len = strlen(s);
    int count = 0;
    char *temp = malloc(len);
    memset(temp, '\0', len);
    printf("%s %d\n", s, len);
    while(s[count] != '\0') {
        printf("%c %d %d\n", s[count], count, (len-count-1));
        temp[len-count-1] = s[count];
        count++;
    }
    return temp;
}

int main()
{
    char *temp = reverseString("race a car");
    printf("%s\n", temp);
}

int main()
{
    unsigned int i = 5;
    char **arr = malloc(i * sizeof(char *));

    for (int k = 0; k < i ; k++)
    {
        arr[k] = malloc(16 * sizeof(char));
        sprintf(arr[k], "%s", i);
    }

    for (int k = 0; k < i ; k++)
    {
        printf("%s", arr[k]);
        free(arr[k]);
    }

    free(arr);

    return 0;
    
}
#endif
