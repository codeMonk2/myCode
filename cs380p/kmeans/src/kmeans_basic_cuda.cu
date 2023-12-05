#include<stdio.h>
#include<limits>
#include "argparse.h"
#include "helpers.h"
#include <cuda.h>

//__global__ void cudaAdd(int *d_a, int *d_b, int *d_result)
//{
//    d_result[threadIdx.x] = d_a[threadIdx.x]+d_b[threadIdx.x];
//}
//
//void setup(int *a, int *b, int *result)
//{
//    int *d_a, *d_b, *d_result;
//    cudaMalloc(&d_a, 16);
//    cudaMalloc(&d_b, 16);
//    cudaMalloc(&d_result, 16);
//    cudaMemcpy(d_a, a, 16, cudaMemcpyHostToDevice);
//    cudaMemcpy(d_b, b, 16, cudaMemcpyHostToDevice);
//    cudaAdd<<<1,4>>>(d_a, d_b, d_result);
//    cudaMemcpy(result, d_result, 16, cudaMemcpyDeviceToHost);
//    printf("%d %d %d %d\n", result[0],result[1],result[2],result[3]);
//    cudaFree(d_a);
//    cudaFree(d_b);
//    cudaFree(d_result);
//}

__device__ double pointDistanceBasicCuda(unsigned int dims, double *point, double *centroid)
{
    double sum = 0.0;

    for(unsigned int i = 0; i < dims; i++)
    {
        sum += pow((point[i]-centroid[i]), 2.0);
    }

    return sqrt(sum);
}


__global__ void cudaNewCentroids2(struct options_t *d_opts,
                                  double *d_centroids,
                                  double *d_centroidSums,
                                  int    *d_ExceedThreshold,
                                  int    *d_numPointsAtCentroid)
{
    const int centroid = threadIdx.x;
    const int count = max(1, d_numPointsAtCentroid[centroid]);

    for (int j = 0; j < d_opts->nDims; j++)
    {
        if(((double)d_centroidSums[(centroid*d_opts->nDims)+j]/count) - (double)d_centroids[(centroid*d_opts->nDims)+j] > d_opts->threshold)
        {
			atomicAdd(d_ExceedThreshold, 1);
        }
        d_centroids[(centroid*d_opts->nDims)+j] = d_centroidSums[(centroid*d_opts->nDims)+j]/count;
        d_centroidSums[(centroid*d_opts->nDims)+j] = 0;
    }
    d_numPointsAtCentroid[centroid] = 0;

    __syncthreads();
}

__global__ void cudaKmeans2(struct options_t *d_opts,
                            double *d_inputData,
                            double *d_centroids,
                            double *d_centroidSums,
                            int    *d_numPointsAtCentroid,
                            int    *d_pointCentroidMap,
                            double maxDistance)
{
    const int index = blockIdx.x * blockDim.x + threadIdx.x;
    double distance = 0.0, bestDistance = maxDistance;
    int bestCentroid = 0;

    for(int j = 0; j < d_opts->nCentroids; j++)
    {
        distance = pointDistanceBasicCuda(d_opts->nDims, &d_inputData[(index*d_opts->nDims)], &d_centroids[(j*d_opts->nDims)]);
        if(distance < bestDistance)
        {
            bestDistance = distance;
            bestCentroid = j;
        }
    }

    for (int j = 0; j < d_opts->nDims; j++)
    {
        atomicAdd(&d_centroidSums[(bestCentroid*d_opts->nDims)+j], d_inputData[(index*d_opts->nDims)+j]);
    }
    atomicAdd(&d_numPointsAtCentroid[bestCentroid], 1);
    d_pointCentroidMap[index]=bestCentroid;

    __syncthreads();
}

void runKmeansBasicCuda(struct options_t *opts, double *inputData)
{
    double *d_inputData, *d_centroids, *d_centroidSums, *centroids;
    int *d_ExceedThreshold, *d_numPointsAtCentroid, *d_pointCentroidMap, *pointCentroidMap, n = 0, threads = 0, blocks = 0, exceedThreshold = 0;
    struct options_t *d_opts;
    cudaDeviceProp prop;
    cudaEvent_t start, stop;
    float delta_ms=0.0;

    cudaGetDeviceProperties(&prop, 0);

    threads = prop.maxThreadsPerBlock;
    blocks = (opts->nVals+threads-1) / threads;

    initCentroids(opts, inputData, &centroids);
    pointCentroidMap = (int *)malloc((opts->nVals * sizeof(int)));
    memset(pointCentroidMap, 0, (opts->nVals * sizeof(int)));

    cudaMalloc(&d_opts, sizeof(struct options_t));
    cudaMemcpy(d_opts, opts, sizeof(struct options_t), cudaMemcpyHostToDevice);

    cudaMalloc(&d_inputData, ((opts->nVals * opts->nDims) * sizeof(double)));
    cudaMemcpy(d_inputData, inputData, ((opts->nVals * opts->nDims) * sizeof(double)), cudaMemcpyHostToDevice);

    cudaMalloc(&d_centroids, ((opts->nCentroids * opts->nDims) * sizeof(double)));
    cudaMemcpy(d_centroids, centroids, ((opts->nCentroids * opts->nDims) * sizeof(double)), cudaMemcpyHostToDevice);

    cudaMalloc(&d_centroidSums, ((opts->nCentroids * opts->nDims) * sizeof(double)));
    cudaMalloc(&d_numPointsAtCentroid, (opts->nCentroids*sizeof(int)));
    cudaMalloc(&d_pointCentroidMap, (opts->nVals * sizeof(int)));
    cudaMalloc(&d_ExceedThreshold, sizeof(int));

    cudaMemset(d_centroidSums, 0, ((opts->nCentroids * opts->nDims) * sizeof(double)));
    cudaMemset(d_numPointsAtCentroid, 0, (opts->nCentroids*sizeof(int)));
    cudaMemset(d_pointCentroidMap, 0, (opts->nVals * sizeof(int)));
	cudaMemset(d_ExceedThreshold, 0, sizeof(int));

    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    do {
		cudaMemset(d_ExceedThreshold, 0, sizeof(int));

        cudaKmeans2<<<blocks,threads>>>(d_opts, d_inputData, d_centroids,
                                        d_centroidSums, d_numPointsAtCentroid,
                                        d_pointCentroidMap,
                                        std::numeric_limits<double>::max());

        cudaNewCentroids2<<<1,opts->nCentroids>>>(d_opts, d_centroids, d_centroidSums, d_ExceedThreshold, d_numPointsAtCentroid);

		cudaMemcpy(&exceedThreshold, d_ExceedThreshold, sizeof(int), cudaMemcpyDeviceToHost);

        n++;
    } while (n < opts->nIters && exceedThreshold != 0);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&delta_ms, start, stop);

    cudaMemcpy(centroids, d_centroids, ((opts->nCentroids * opts->nDims) * sizeof(double)), cudaMemcpyDeviceToHost);
    cudaMemcpy(pointCentroidMap, d_pointCentroidMap, (opts->nVals * sizeof(int)), cudaMemcpyDeviceToHost);

    printf("%d,%lf\n", n, delta_ms/n);

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

    cudaFree(d_opts);
    cudaFree(d_inputData);
    cudaFree(d_centroids);
    cudaFree(d_centroidSums);
    cudaFree(d_numPointsAtCentroid);
    cudaFree(d_pointCentroidMap);
	free(pointCentroidMap);
    clearCentroids(&centroids);
}
