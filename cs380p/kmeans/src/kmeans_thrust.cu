#include<stdio.h>
#include<limits>
#include "argparse.h"
#include "helpers.h"
#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/extrema.h>
#include <thrust/execution_policy.h>
#include <thrust/fill.h>
#include <thrust/sequence.h>
#include <thrust/functional.h>
#include <thrust/sort.h>
#include <thrust/reduce.h>
#include <thrust/iterator/constant_iterator.h>
#include <thrust/iterator/counting_iterator.h>
#include <thrust/iterator/transform_iterator.h>
#include <thrust/iterator/permutation_iterator.h>
#include <thrust/functional.h>
#include <thrust/fill.h>
#include <thrust/device_vector.h>
#include <thrust/copy.h>

//{
    //for(unsigned int i = 0; i < opts->nVals; i++)
    //{
    //    //thrust::device_vector<double>::iterator inputIt = d_inputData.begin()+(i * opts->nDims);
    //    bestDistance = std::numeric_limits<double>::max();
    //    bestCentroid = 0;
    //    for(int j = 0; j < opts->nCentroids; j++)
    //    {
    //        //auto centroidIt = d_centroids.begin()+((opts->nDims)*j);
    
    //        distance = sqrt(thrust::transform_reduce(
    //                        thrust::device,
    //                        thrust::make_zip_iterator(thrust::make_tuple(d_inputData.begin()+(i * opts->nDims), (d_centroids.begin()+((opts->nDims)*j)))),
    //                        thrust::make_zip_iterator(thrust::make_tuple(d_inputData.begin()+(i * opts->nDims)+(opts->nDims), (d_centroids.begin()+((opts->nDims)*j)+(opts->nDims)))),
    //                        squaredDistance(),
    //                        0.0,
    //                        reduceDistance()));
    
    //        if(distance < bestDistance)
    //        {
    //            bestDistance = distance;
    //            bestCentroid = j;
    //        }
    //    }
    //    for(int cnt = 0; cnt < opts->nDims; cnt++)
    //        d_pointCentroidKeys[(i*opts->nDims)+cnt] = (bestCentroid*opts->nDims)+cnt;
    //}
//}

struct BestCentroidForInput {
    struct options_t *dOpts;
    double *dInput;
    double *dCentroids;
    int    *dPointCentroidKeys;
    double dMaxValue;
    explicit BestCentroidForInput(struct options_t *_dopts,
                          double *_dInput,
                          double *_dCentroids,
                          int    *_dPointCentroidKeys,
                          double _dMaxValue):
                          dOpts(_dopts),
                          dInput(_dInput),
                          dCentroids(_dCentroids),
                          dPointCentroidKeys(_dPointCentroidKeys),
                          dMaxValue(_dMaxValue) {}
    __host__ __device__ int operator()(int index)
    {
        double distance = 0.0, bestDistance = dMaxValue;
        int bestCentroid = 0;
	    double sum = 0.0;

        for(int j = 0; j < dOpts->nCentroids; j++)
        {
			sum = 0.0;
		    for(unsigned int i = 0; i < dOpts->nDims; i++)
		    {
		    	sum += pow((dInput[index+i]-dCentroids[(j*dOpts->nDims)+i]), 2.0);
		    }

		    distance = sqrt(sum);
            if(distance < bestDistance)
            {
                bestDistance = distance;
                bestCentroid = j;
            }
        }

        for(int cnt = 0; cnt < dOpts->nDims; cnt++)
            dPointCentroidKeys[index+cnt] = (bestCentroid*dOpts->nDims)+cnt;

		return bestCentroid;
    }
};

struct TestThreshold {
    const double a;
    TestThreshold(double _a):a(_a) {}
    __host__ __device__ double operator()(const thrust::tuple<double,double>&arg) {
        return (thrust::get<0>(arg) - thrust::get<1>(arg)) > a ? 1.0:0.0;  
    }
};

struct reduceAndCheckWithinThreshold {
    __host__ __device__ int operator()(double x, double y) {
        return (int)x+(int)y;
    }
};

struct meanOfSums {
    __host__ __device__ double operator()(double &x, double &y) {
        return (x/y); 
    }
};

struct squaredDistance {
    __host__ __device__ double operator()(const thrust::tuple<double,double>&arg) {
        return pow((thrust::get<0>(arg) - thrust::get<1>(arg)), 2.0);
    }
};

struct reduceDistance {
    __host__ __device__ double operator()(double x, double y) {
        return x+y;
    }
};

struct sumCentroidsAndCount
{
    __host__ __device__
    thrust::tuple<double,double> operator ()(const thrust::tuple<double,double>& lhs, const thrust::tuple<double,double>& rhs)
    {
        return thrust::make_tuple(
            thrust::get<0>(lhs) + thrust::get<0>(rhs),
            thrust::get<1>(lhs) + thrust::get<1>(rhs)
        );
    }
};

void runKmeansThrustCuda(struct options_t *opts, double *inputData)
{
    double *centroids;
    int n = 0;
    cudaEvent_t start, stop;
    float delta_ms=0.0;
    struct options_t *d_opts;
    int *pointCentroidMap;

    initCentroids(opts, inputData, &centroids);

    cudaMalloc(&d_opts, sizeof(struct options_t));
    cudaMemcpy(d_opts, opts, sizeof(struct options_t), cudaMemcpyHostToDevice);

    pointCentroidMap = (int *)malloc((opts->nVals * sizeof(int)));
    memset(pointCentroidMap, 0, (opts->nVals * sizeof(int)));

    thrust::device_vector<double>d_inputData(inputData, inputData+(opts->nVals*opts->nDims));
    thrust::device_vector<double>d_inputData1(inputData, inputData+(opts->nVals*opts->nDims));

    thrust::device_vector<double>d_centroids(centroids, centroids+(opts->nCentroids*opts->nDims));
    thrust::device_vector<int>d_pointCentroidKeys(opts->nVals*opts->nDims);

    thrust::device_vector<int>d_inputIndex(opts->nVals);
    thrust::sequence(thrust::device, d_inputIndex.begin(), d_inputIndex.end(), 0, opts->nDims);

    thrust::device_vector<double> d_centroidPointCount(opts->nVals*opts->nDims);
    thrust::device_vector<int> d_pointCentroidMap(opts->nVals);

    int withinThreshold = 0;

    thrust::equal_to<double> binary_pred;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    do {
        withinThreshold=0;
        d_inputData = d_inputData1;

        /* 
         * Transform the input by taking input index in strides of nDims
         * and computing the best centroid for each point in the Functor.
         * The index here acts as the index in a CUDA kernel.
         * The returned vector contains the best centroid selected for each input
         */
        thrust::transform(thrust::device, d_inputIndex.begin(), d_inputIndex.end(),
                          d_pointCentroidMap.begin(), 
                          BestCentroidForInput(d_opts,
                                       thrust::raw_pointer_cast(d_inputData.data()),
                                       thrust::raw_pointer_cast(d_centroids.data()),
                                       thrust::raw_pointer_cast(d_pointCentroidKeys.data()),
                                       std::numeric_limits<double>::max()));
        /*
         * The point centroid keys vector contains the best centroid + nDims index on all the dimensions
         * of each input. Sort by key sorts this set of keys to a set which consecutive indices contain the
         * columns of each input assigned to a certain centroid. The input is sorted according to this key array.
         */
        thrust::stable_sort_by_key(thrust::device, d_pointCentroidKeys.begin(), d_pointCentroidKeys.end(), d_inputData.begin());
        
        /*
         * Here the centroids assigned to a particular centroid are summed up and number of points assigned to
         * each centroid are counted.
         */
        thrust::reduce_by_key(d_pointCentroidKeys.begin(), d_pointCentroidKeys.end(),
            thrust::make_zip_iterator(thrust::make_tuple(d_inputData.begin(), thrust::make_constant_iterator(1.0))),
            d_pointCentroidKeys.begin(),
            thrust::make_zip_iterator(thrust::make_tuple(d_inputData.begin(), d_centroidPointCount.begin())),
            binary_pred,
            sumCentroidsAndCount());
        /*
         * We get mean value of the current assignments. This is the new centroid.
         */
        thrust::transform(thrust::device, d_inputData.begin(), d_inputData.begin()+(opts->nCentroids*opts->nDims),
                          d_centroidPointCount.begin(),  d_inputData.begin(), meanOfSums());

        /*
         * Check whether the old and new centroids are within a threshold. This is done by comparing the old and new
         * centroids and saving a 0 or 1. This is then reduced to check the sum and see if it is > 0 or = 0.
         */
        withinThreshold = thrust::transform_reduce(thrust::device, 
                                 thrust::make_zip_iterator(thrust::make_tuple(d_inputData.begin(), d_centroids.begin())), 
                                 thrust::make_zip_iterator(thrust::make_tuple(d_inputData.begin()+(opts->nCentroids*opts->nDims), d_centroids.begin()+(opts->nCentroids*opts->nDims))),
                                 TestThreshold(opts->threshold),
                                 0.0,
                                 reduceAndCheckWithinThreshold());

        thrust::copy(d_inputData.begin(), d_inputData.begin()+(opts->nCentroids*opts->nDims), d_centroids.begin());

        n++;
    } while(n < opts->nIters && withinThreshold != 0);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    cudaEventElapsedTime(&delta_ms, start, stop);

    printf("%d,%lf\n", n, delta_ms/n);

    cudaMemcpy(centroids, thrust::raw_pointer_cast(d_centroids.data()), ((opts->nCentroids*opts->nDims) * sizeof(double)), cudaMemcpyDeviceToHost); 
    cudaMemcpy(pointCentroidMap, thrust::raw_pointer_cast(d_pointCentroidMap.data()), (opts->nVals * sizeof(int)), cudaMemcpyDeviceToHost);

    if (opts->bPrintCentroids)
    {
        printCentroids(opts, centroids);
    }
    else
    {
        printf("clusters:");
        for (int p=0; p < opts->nVals; p++)
            printf(" %d", (int)pointCentroidMap[p]);
    }

    cudaFree(d_opts);
    free(pointCentroidMap);
    clearCentroids(&centroids);
}
