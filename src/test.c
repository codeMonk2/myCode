#include <stdio.h>

#define swap(x,y) {\
    int temp = x;\
    x=y;\
    y=temp;\
}

#define printarr(arr) {\
    for(int i = 0; i < 8; i++) {\
        printf("%d ", arr[i]);\
    }\
    printf("\n");\
}

void swap1(int* a, int* b)
{
   int t = *a;
   *a = *b;
   *b = t;
}


int binSearch( int arr[], int len, int key ) {
   int low = 0;
   int high = len - 1;
   while( low < high ) {
      int mid = low+( high - low )/2;
      if( arr[mid] < key ) {
         low = mid + 1;
      } else if ( arr[mid] > key ) {
         high = mid - 1;
      } else {
         return mid;
      }
   }
   return -1;
}

int arrPart( int arr[], int low, int high ) {
   int l = low;
   int r = high;
   int p = low+(high-low)/2;
   while( 1 ) {
      while( l < high && arr[l]<=arr[p] ) l++;
      while( r > low && arr[r]>=arr[p] ) r--;
      if ( l >= r ) break;
      swap( arr[l], arr[r] );
   }
   swap( arr[p], arr[l] );
   return l;
}

int partition (int arr[], int low, int high)
{
    int pivot = arr[high]; // pivot
    int i = (low - 1); // Index of smaller element and indicates the right position of pivot found so far
 
    for (int j = low; j <= high - 1; j++)
    {
        // If current element is smaller than the pivot
        if (arr[j] < pivot)
        {
            i++; // increment index of smaller element
            swap1(&arr[i], &arr[j]);
        }
    }
    swap1(&arr[i + 1], &arr[high]);
    return (i + 1);
}

int qSort5( int arr[], int low, int high ) {
   if( low < high ) {
      printf("1)");
      printarr( arr );
      int pi = partition( arr, low, high );
      printf("2)");
      printarr( arr );
      partition( arr, low, pi-1);
      printf("3)");
      printarr( arr );
      partition( arr, pi+1, high);
   }
}

int myStrLen( char *str ) {
   const char *s = str;
   while( *s != '\0' ) s++;
   return s - str;
}

int main() {
   //char * const test = "srinivas";
   //char * test1 = "prasad";
   //char arr[] = {'s','r','i','n','i'};
   //test[2]='n';
   //test = arr;
   //test[4] = 'a';
   //test = test1;
   //int nums[] = { 2, 4, 8, 8, 10, 13, 17, 22, 28, 33, 39, 42, 47 };
   //printf(" bin search %d\n", binSearch( nums, 13, 17 ) );
   int nums1[] = { 13, 7, 15, 9, 5, 21, 12, 6 };
   qSort5( nums1, 0, 7 );
   printarr( nums1 );
   printf("strlen %d\n", myStrLen( "srinivas" ) );
   return 0;
}
