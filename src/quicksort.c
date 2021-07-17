#include <stdio.h>

#define N 7

#define swap(x,y) {\
    int temp = x;\
    x=y;\
    y=temp;\
}

#define printarr(arr) {\
    for(int i = 0; i <= N; i++) {\
        printf("%d ", arr[i]);\
    }\
    printf("\n");\
}

#if 0
/* This function takes last element as pivot, places
 * the pivot element at its correct position in sorted
 * array, and places all smaller (smaller than pivot)
 * to left of pivot and all greater elements to right
 * of pivot */
int partition (int arr[], int low, int high)
{
    int pivot = arr[high];    // pivot
    int i = (low - 1);  // Index of smaller element

    for (int j = low; j <= high- 1; j++)
    {
        if (arr[j] <= pivot)
        {
            i++;    // increment index of smaller element
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}
#endif

int partition(int *arr, int low, int high)
{
    int i = low;
    int j = high;
    int pivot = low;
    while(1) {
        while(i < high && arr[i] <= arr[pivot]) i++;
        while(j > low  && arr[j] >= arr[pivot]) j--;
        if (i >= j)
            break;
        swap(arr[i], arr[j]);
        printf("1) ");
        printarr(arr);
    }
    swap(arr[pivot], arr[j]);
    printf("2) ");
    printarr(arr);
    return j;
}


/* The main function that implements QuickSort
   arr[] --> Array to be sorted,
   low  --> Starting index,
   high  --> Ending index */
void quickSort(int arr[], int low, int high)
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partition(arr, low, high);

        // Separately sort elements before
        // partition and after partition
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi+1, high);
    }
}

int main() {
    int arr[] = {23,45,12,6,74,24,94,4};
    int arr1[] = {-1,0,1,2,-1,-4, 0, 0};
    int arr2[] = {74, 24, 94, 45, 0, 0, 0, 0};
    quickSort(arr, 0, 7);
    printarr(arr);
    int nums1[] = { 13,7, 15, 9, 5, 21, 6 };
    quickSort( nums1, 0, 8 );
    printarr(nums1);
    return 0;
}
