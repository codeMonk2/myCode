#include <io.h>
#include <string.h>
#include <climits>
#include <cassert>

void read_file(struct options_t* opts,
               double **inputData)
{
    unsigned int i = 0;
    unsigned int LineNo = 0;
    int dim = 0;

    // Open file
    std::ifstream in;
    in.open(opts->in_file);

    // Get num vals
    in >> opts->nVals;

    if ((opts->nVals) <= 0 || (opts->nVals) > INT_MAX)
    {
        std::cerr << "Invalid number of inputs in input file " << std::endl;
        exit(1);
    }

    *inputData = (double *)malloc((opts->nVals * opts->nDims) * sizeof(double));

    for(i = 0; i < opts->nVals; i++)
    {
        in >> LineNo;
        assert(LineNo == (i+1));

        for(dim = 0; dim < opts->nDims; dim++)
        {
            in >> (*inputData)[(i*opts->nDims)+dim];
        }
    }
}
