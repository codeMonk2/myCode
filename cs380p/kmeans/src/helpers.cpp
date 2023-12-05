#include <stdio.h>
#include <argparse.h>

static unsigned long int seed;
static unsigned long kmeans_rmax = 32767;
unsigned int kmeans_rand() {
    seed = seed * 1103515245 + 12345;
    return (unsigned int)(seed/65536) % (kmeans_rmax+1);
}

void printFunc(struct options_t *opts, double *inputData)
{
    for(int i = 0; i < opts->nVals; i++)
    {
        for (int j = 0; j < opts->nDims; j++)
        {
            printf("%lf ", inputData[(i*opts->nDims)+j]);
        }
        printf("\n");
    }
}

void printCentroids(struct options_t *opts, double *centroids)
{
    int count = 0;
    for (int i = 0; i < opts->nCentroids; i++)
    {
        printf("%d ", count);
        for(unsigned int j = 0; j < opts->nDims; j++)
        {
            //std::cout << setprecision(5) << pt[i] << " ";
            printf("%0.5f", centroids[(i*opts->nDims)+j]);
            if ((j+1) != opts->nDims) printf(" ");
        }
        //std::cout << std::endl;
        printf("\n");
        count++;
    }
}

void initCentroids(struct options_t *opts, double *inputData, double **centroids)
{
    seed = opts->randSeed;

    *centroids = (double *)malloc((opts->nCentroids * opts->nDims) * sizeof(double));

    for (int i = 0; i < opts->nCentroids; i++)
    {
        int index = kmeans_rand() % opts->nVals;
        for(int j = 0; j < opts->nDims; j++)
        {
            (*centroids)[(i*opts->nDims)+j] = inputData[(index*opts->nDims)+j];
        }
    }
}

void clearCentroids(double **centroids)
{
    free(*centroids);
}
