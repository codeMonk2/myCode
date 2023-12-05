#pragma once

#include "operators.h"
#include <stdlib.h>
#include <pthread.h>
#include <spin_barrier.h>

#define INT_MAX 2147483647

struct prefix_sum_args_t {
  int*               input_vals;
  int*               output_vals;
  bool               spin;
  unsigned int       n_orig_vals;
  unsigned int       n_vals;
  unsigned int       n_threads;
  int                t_id;
  spin_barrier_t*    t_barrier;
  int (*op)(int, int, int);
  int n_loops;
};

prefix_sum_args_t* alloc_args(unsigned int n_threads);

unsigned int next_power_of_two(unsigned int x);

void fill_args(prefix_sum_args_t *args,
               unsigned int n_threads,
               unsigned int n_orig_vals,
               unsigned int n_vals,
               int *inputs,
               int *outputs,
               bool spin,
               spin_barrier_t* t_barrier,
               int (*op)(int, int, int),
               int n_loops);

void printFunc(int *vals, unsigned int num_vals);

unsigned int logTwo(unsigned int x);
