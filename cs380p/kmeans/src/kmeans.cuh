#include "argparse.h"

void runKmeansBasicCuda(struct options_t *opts, double *inputData);
void runKmeansSharedMem(struct options_t *opts, double *inputData);
void runKmeansThrustCuda(struct options_t *opts, double *inputData);
