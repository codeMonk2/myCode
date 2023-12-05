#include <iostream>
#include <argparse.h>
#include <threads.h>
#include <io.h>
#include <chrono>
#include <cstring>
#include "operators.h"
#include "helpers.h"
#include "prefix_sum.h"
#include <math.h>
#include <stdlib.h>

using namespace std;

int main(int argc, char **argv)
{
    unsigned int n_orig_vals,n_vals;
    int *input_vals, *output_vals;
    spin_barrier_t barrier;

    // Parse args
    struct options_t opts;
    get_opts(argc, argv, &opts);

    bool sequential = false;
    if (opts.n_threads == 0) {
        opts.n_threads = 1;
        sequential = true;
    }

    // Setup threads
    pthread_t *threads = sequential ? NULL : alloc_threads(opts.n_threads);

    // Setup args & read input data
    prefix_sum_args_t *ps_args = alloc_args(opts.n_threads);

    read_file(&opts, &n_orig_vals, &n_vals, &input_vals, &output_vals);

    //"op" is the operator you have to use, but you can use "add" to test
    int (*scan_operator)(int, int, int);
    scan_operator = op;
    //scan_operator = add;

    if (opts.n_threads > 0)
    {
        int ret = spin_barrier_init(&barrier, opts.spin, opts.n_threads);
        if (ret)
        {
            std::cerr << "Failed to initialize barrier" << std::endl;
            exit(1);
        }
    }

    fill_args(ps_args, opts.n_threads, n_orig_vals, n_vals, input_vals, output_vals,
        opts.spin, &barrier, scan_operator, opts.n_loops);

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    if (sequential)  {
        //sequential prefix scan
        output_vals[0] = input_vals[0];
        for (unsigned int i = 1; i < n_orig_vals; ++i) {
            //y_i = y_{i-1}  <op>  x_i
            output_vals[i] = scan_operator(output_vals[i-1], input_vals[i], ps_args->n_loops);
        }
    }
    else {
        start_threads(threads, ps_args->n_threads, ps_args, compute_prefix_sum);
        // Wait for threads to finish
        join_threads(threads, ps_args->n_threads);

        if (__builtin_popcount(n_vals) == 1)
        {
            output_vals[n_vals] += output_vals[n_vals-1];
        }
    }

    //End timer and print out elapsed
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "time: " << diff.count() << std::endl;

    // Write output data
    write_file(&opts, &(ps_args[0]), sequential);

    // Free other buffers
    free(threads);
    free(ps_args);
}
