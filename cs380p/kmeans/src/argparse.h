#ifndef _ARGPARSE_H
#define _ARGPARSE_H

#include <getopt.h>
#include <stdlib.h>
#include <iostream>

struct options_t {
    char *in_file;
    int nCentroids;
    int nDims;
    int nIters;
    double threshold;
    unsigned long int randSeed;
    bool bPrintCentroids;
    int nVals;
    int algoType;
};

void get_opts(int argc, char **argv, struct options_t *opts);
#endif
