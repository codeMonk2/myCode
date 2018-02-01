#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// The longest common subsequence (LCS) problem is the problem
// of finding the longest subsequence common to all sequences
//  in a set of sequences (often just two sequences).

#define MAX(x,y) (x)>(y)?(x):(y)

int mem[10][10];

int cmp_str(char a[], char b[] ) {
  int i = 0;
  for (i = 0; i < strlen(a) && i < strlen(b); i++) {
    if (a[i] != b[i]) break;
  }
  return i;
}


int lcs( char a[], char b[]) {

  if (strlen(a) == 0 || strlen(b) == 0)
    return 0;

  if (mem[strlen(a)][strlen(b)] != 0)
    return mem[strlen(a)][strlen(b)];

  int len = cmp_str(a,b);
  int len1, len2;

  //printf("len %d a %s b %s\n", len, a, b);

  if ( strlen(b) == 1 ) {
    len1 = 0;
  } else {
    len1 = lcs(a, strstr(b, &b[0]+1));
  }

  if ( strlen(a) == 1 ) {
    len2 = 0;
  } else {
    len2 = lcs(strstr(a, &a[0]+1), b);
  }

  mem[strlen(a)][strlen(b)] = MAX(len, MAX(len1, len2));

  return mem[strlen(a)][strlen(b)];
 
}


int main() {

  char a[32] = {'\0'};
  char b[32] = {'\0'};

  printf("Enter Str1:");
  scanf("%s", a);
  printf("Enter Str2:");
  scanf("%s", b);

  int ret = lcs(a,b);

  printf("Final ret %d\n", ret);

  return 0; 
}
