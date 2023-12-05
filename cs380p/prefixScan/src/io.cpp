#include <io.h>
#include <string.h>
#include "helpers.h"

void read_file(struct options_t* args,
               unsigned int*     n_orig_vals,
               unsigned int*     n_vals,
               int**             input_vals,
               int**             output_vals) {

    unsigned int i = 0;
  	// Open file
	std::ifstream in;
	in.open(args->in_file);

	// Get num vals
	in >> *n_orig_vals;

    if ((*n_orig_vals) <= 0 || (*n_orig_vals) > INT_MAX)
    {
        std::cerr << "Invalid number of inputs in input file " << std::endl;
        exit(1);
    }

	// Alloc input and output arrays
    *n_vals = next_power_of_two(*n_orig_vals);
	*input_vals = (int*) malloc((*n_vals+1) * sizeof(int));
    memset((char *)(*input_vals), 0, ((*n_vals+1) * sizeof(int)));
	*output_vals = (int*) malloc((*n_vals+1) * sizeof(int));
    memset((char *)(*output_vals), 0, ((*n_vals+1) * sizeof(int)));

	// Read input vals
	for (i = 0; i < *n_orig_vals; ++i) {
		in >> (*input_vals)[i];
        (*output_vals)[i] = (*input_vals)[i];
	}
    (*input_vals)[i]=(*input_vals)[i-1];
    (*output_vals)[i]=(*output_vals)[i-1];
}

void write_file(struct options_t*         args,
               	struct prefix_sum_args_t* opts,
                bool   sequential) {
    unsigned int i = (sequential)?0:1;
    unsigned int max = opts->n_orig_vals + ((sequential)?0:1);
  // Open file
	std::ofstream out;
	out.open(args->out_file, std::ofstream::trunc);

	// Write solution to output file
	for (; i < max; ++i) {
		out << opts->output_vals[i] << std::endl;
	}

	out.flush();
	out.close();

	// Free memory
	free(opts->input_vals);
	free(opts->output_vals);
}
