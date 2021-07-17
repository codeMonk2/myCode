#include <stdio.h>

typedef struct inp_data {
    int *arr;
    int size;
}inp_data;

#define swap(a, b) {\
    a = a ^ b;\
    b = a ^ b;\
    a = a ^ b;\
}

#define NUM_NOS 8

#define LEFT(i) (2*(i) + 1)
#define RIGHT(i) (2*(i) + 2)
#define PARENT(i) ((i-1)/2)

void print_heap(inp_data *data)
{
    for(int i = 0; i < data->size; i++)
    {
        printf("%d ", data->arr[i]);
    }
    printf("\n");
}

void max_heapify(inp_data *data, int i)
{
    int l = LEFT(i);
    int r = RIGHT(i);

    int largest = i;

    if (l < data->size && largest < data->size && data->arr[l] > data->arr[largest])
        largest = l;

    if (r < data->size && largest < data->size && data->arr[r] > data->arr[largest])
        largest = r;

    if (largest != i) {
        swap(data->arr[i], data->arr[largest]);
        max_heapify(data, largest);
    }
    return;
}

void build_max_heap(inp_data *data)
{
    for (int i = data->size/2; i >= 0; i--) {
        max_heapify(data, i);
    }
}

void heap_sort(inp_data *data)
{
    int size = data->size;
    
    build_max_heap(data);

    for (int i = data->size-1; i >= 1; i--)
    {
        swap(data->arr[0], data->arr[i]);
        data->size--;
        max_heapify(data, 0);
    }

    data->size = size;
    print_heap(data);
}

int extract_max(inp_data *data)
{
    int max = data->arr[0];

    swap(data->arr[0], data->arr[data->size-1]);
    data->size--;

    build_max_heap(data);

    return max;
}

void insert_heap_key(inp_data *data, int key)
{
    data->arr[data->size++] = key;
    int i = data->size-1;
    while(i > 0 && data->arr[PARENT(i)] < data->arr[i]) {
        swap(data->arr[i], data->arr[PARENT(i)]);
        i = PARENT(i);
    }
}

int main()
{
    inp_data data;

    int arr[15] = {4,5,6,2,7,8,1,14,0,0,0,0,0,0,0};
    
    data.arr = arr; 
    data.size = NUM_NOS;
 
    build_max_heap(&data);

    insert_heap_key(&data, 22);
    heap_sort(&data);

    return 0;
}
