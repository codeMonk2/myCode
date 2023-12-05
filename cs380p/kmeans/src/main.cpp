#include <argparse.h>
#include "kmeans.cuh"
#include "helpers.h"
#include "io.h"

int main(int argc, char **argv)
{
    double *inputData;

    // Parse args
    struct options_t opts;
    get_opts(argc, argv, &opts);

    read_file(&opts, &inputData);

    switch(opts.algoType)
    {
    case 1:
        kMeans(inputData, &opts);
        break;
    case 2:
        runKmeansBasicCuda(&opts, inputData);
        break;
    case 3:
        runKmeansSharedMem(&opts, inputData);
        break;
    case 4:
        runKmeansThrustCuda(&opts, inputData);
        break;
    default:
        printf("Invalid algoType\n");
        exit(1);
    }

    free(inputData);

    return 0;
}
