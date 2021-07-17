#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Buying and selling a stock once. For max profit
//you have to buy at min_price and then sell on a day 
//after the min_price where the difference is maximum
void max_profit(int price_arr[])
{
    int i = 0, profit = 0, max_profit = 0;
    int min_price = price_arr[0];

    for (i = 1; i < 10; i++) {
        if (price_arr[i] < min_price) {
            min_price = price_arr[i];
        } else {
            profit = price_arr[i] - min_price;
            if(profit > max_profit)
                max_profit = profit;
        }
    }

    printf("Max profit is %d\n", max_profit);
}

#define my_sizeof(var) ((char *)(&var+1) - (char *)(&var))


//This solution is based on dynamic programming
//https://www.youtube.com/watch?v=oDhu5uGq_ic
//
void max_profit_k_trans(int price_arr[], int k)
{
    //int *tbl = malloc(10 * (k+1) * 4);

    //memset(tbl, 0, (10*(k+1)*4));

    int tbl[4][10] = {0};

    for (int i = 1; i <= k; i++)
    {
        for (int j = 1; j < 10; j++)
        {
            int val1 = tbl[i][j-1];
            int max_val = 0;
            for (int m = 0; m < j; m++)
            {
                int val2 = price_arr[j] - price_arr[m] + tbl[i-1][m];
                if (val2 > max_val) max_val = val2;
            }

            tbl[i][j] = (val1 > max_val) ? val1 : max_val;
        }
    }

    for (int i = 0; i <= k; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%d ", tbl[i][j]);
        }
        printf("\n");
    }

    //free(tbl);
}

void max_profit_k_trans_faster(int price_arr[], int k)
{
    //int *tbl = malloc(10 * (k+1) * 4);

    //memset(tbl, 0, (10*(k+1)*4));

    int tbl[4][10] = {0};

    for (int i = 1; i <= k; i++)
    {
        int max_diff = -price_arr[0];
        for (int j = 1; j < 10; j++)
        {
            tbl[i][j] = (tbl[i][j-1] > (price_arr[j]+max_diff)) ? tbl[i][j-1] : (price_arr[j]+max_diff);

            max_diff = (max_diff > tbl[i-1][j] - price_arr[j]) ? max_diff : (tbl[i-1][j] - price_arr[j]);

        }
    }

    for (int i = 0; i <= k; i++) {
        for (int j = 0; j < 10; j++) {
            printf("%d ", tbl[i][j]);
        }
        printf("\n");
    }

    //free(tbl);
}
void print_arr(int arr[], int n)
{
    int i;

    for ( i = 0; i < n ; i++)
    {
        printf("%d ", arr[i]);
    }

    printf("\n");
}

void swap(int *a, int *b)
{
    *a = *a + *b;
    *b = *a - *b;
    *a = *a - *b;
}

int main()
{
    //int price_arr[] = {310,315,275,295,260,270,290,230,255,250};
    //int price_arr[] = {2,5,7,1,4,3,1,3};
    int price_arr[] = {12,11,13,9,12,8,14,13,15,1};
    max_profit_k_trans(price_arr, 2);
    max_profit_k_trans_faster(price_arr, 2);
    int i = 5;
    printf("%ld \n", my_sizeof(i));
    return 0;
}
