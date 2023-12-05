#include "argparse.h"
#include "helpers.h"
#include <time.h>
#include <math.h>
#include <cstring>
#include <limits>
#include <unistd.h>

static
double euclideanDistance(unsigned int dims, double *point, double *centroid)
{
    double sum = 0.0;

    for(unsigned int i = 0; i < dims; i++)
    {
        sum += pow((point[i]-centroid[i]), 2);
    }

    return sqrt(sum);
}

void kMeans(double *inputData,
            struct options_t *opts)
{
    double bestDistance = 0.0, distance = 0.0;
    int bestCentroid = 0, n = 0;
    double *centroids = NULL;
    double *centroidSums = (double *)malloc((opts->nCentroids * opts->nDims) * sizeof(double));
    int *numPointsAtCentroid = (int *)malloc(opts->nCentroids*sizeof(int));
    int *pointCentroidMap = (int *)malloc(opts->nVals * sizeof(int));
    bool bWithinThreshold;

    initCentroids(opts, inputData, &centroids);
    memset(centroidSums, 0, ((opts->nCentroids * opts->nDims) * sizeof(double)));
    memset(numPointsAtCentroid, 0, (opts->nCentroids*sizeof(int)));
    memset(pointCentroidMap, 0, (opts->nVals * sizeof(int)));

    clock_t tic = clock();
    do {
        bWithinThreshold=true;
        //memset(centroidSums, 0, ((opts->nCentroids * opts->nDims) * sizeof(double)));
        //memset(numPointsAtCentroid, 0, (opts->nCentroids*sizeof(int)));
        for(unsigned int i = 0; i < opts->nVals; i++)
        {
            bestDistance = std::numeric_limits<double>::max();
            bestCentroid = 0;
            for(int j = 0; j < opts->nCentroids; j++)
            {
                distance = euclideanDistance(opts->nDims, &inputData[(i*opts->nDims)], &centroids[(j*opts->nDims)]);
                if(distance < bestDistance)
                {
                    bestDistance = distance;
                    bestCentroid = j;
                }
            }
            //printf("i %d dist %lf centroid %d\n", i, bestDistance, bestCentroid);
            for (int j = 0; j < opts->nDims; j++)
            {
                centroidSums[(bestCentroid*opts->nDims)+j] += inputData[(i*opts->nDims)+j];
            }
            numPointsAtCentroid[bestCentroid]++;
            pointCentroidMap[i]=bestCentroid;
        }
        for(int i = 0; i < opts->nCentroids; i++)
        {
            for (int j = 0; j < opts->nDims; j++)
            {
                if(((double)(centroidSums[(i*opts->nDims)+j]/numPointsAtCentroid[i])) - (double)centroids[(i*opts->nDims)+j] > opts->threshold)
                {
                    bWithinThreshold=false;
                }
                centroids[(i*opts->nDims)+j] = centroidSums[(i*opts->nDims)+j]/numPointsAtCentroid[i];
                centroidSums[(i*opts->nDims)+j] = 0;
            }
            numPointsAtCentroid[i] = 0;
        }
        n++;
    } while(n < opts->nIters && bWithinThreshold==false);
    clock_t toc = clock();

    printf("%d,%lf\n", n,
           (((double)(toc - tic) / CLOCKS_PER_SEC)/n)*1000);

    if (opts->bPrintCentroids)
    {
        printCentroids(opts, centroids);
    }
    else
    {
        printf("clusters:");
        for (int p=0; p < opts->nVals; p++)
            printf(" %d", pointCentroidMap[p]);
    }

    clearCentroids(&centroids);
    free(centroidSums);
    free(numPointsAtCentroid);
}
