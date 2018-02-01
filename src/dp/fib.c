#include <stdio.h>

unsigned long long mem[512];

unsigned long long fib( unsigned int n ) {
  if (mem[n] != 0 ) {
    return mem[n];
  }

  return mem[n]=fib(n-1)+fib(n-2);
}

int main() {
  int val = 1;
  mem[0] = 1, mem[1] = 1;
  while (1) {
    printf("Enter the value:");
    scanf("%d", &val);
    printf("%lld\n", fib(val));
  }
  return 0;
  
}
