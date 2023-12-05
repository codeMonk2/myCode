#include "helpers.h"

prefix_sum_args_t* alloc_args(unsigned int n_threads) {
  return (prefix_sum_args_t*) malloc(n_threads * sizeof(prefix_sum_args_t));
}

unsigned int next_power_of_two(unsigned int x) {
    x--;
    x |= x>>1;
    x |= x>>2;
    x |= x>>4;
    x |= x>>8;
    x |= x>>16;
    x++;
    return x;
}

void fill_args(prefix_sum_args_t *args,
               unsigned int n_threads,
               unsigned int n_orig_vals,
               unsigned int n_vals,
               int *inputs,
               int *outputs,
               bool spin,
               spin_barrier_t* barrier,
               int (*op)(int, int, int),
               int n_loops) {
    for (unsigned int i = 0; i < n_threads; ++i) {
        args[i] = {inputs, outputs, spin, n_orig_vals, n_vals,
                   n_threads, (int)i, barrier, op, n_loops};
    }
}

void printFunc(int *vals, unsigned int num_vals)
{
    for(unsigned int i = 0; i < num_vals; i++)
    {
        printf("%d ", vals[i]);
    }
    printf("\n");
}

unsigned int logTwo(unsigned int x)
{
    int result = 0;
    while(x >>= 1) result++;
    return result;
}
