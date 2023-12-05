#include <argparse.h>

unsigned int kmeans_rand();
void printFunc(struct options_t *opts, double *inputData);
void printCentroids(struct options_t *opts, double *centroids);
void initCentroids(struct options_t *opts, double *inputData, double **centroids);
void clearCentroids(double **centroids);
void kMeans(double *inputData, struct options_t *opts);
