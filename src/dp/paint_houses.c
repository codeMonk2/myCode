// There are a row of n houses, each house can be painted 
// with one of the three colors: red, blue or green. 
// The cost of painting each house with a certain color is 
// different. You have to paint all the houses such that 
// no two adjacent houses have the same color.

// The cost of painting each house with a certain color is 
// represented by a n x 3 cost matrix. For example, costs[0][0] 
// is the cost of painting house 0 with color red; costs[1][2] is 
// the cost of painting house 1 with color green, and so on... 
// Find the minimum cost to paint all houses.

#include <stdio.h>

#define MAX_ROWS 5

int cost[MAX_ROWS][3] = { {10,  5,  8},
                          {8,   5, 12},
                          {9,  14, 16},
                          {13, 12,  5},
                          {15,   8,  4} };

int mem[MAX_ROWS][3];

#define MIN(x,y) ((x)<(y)?(x):(y))

int
paint_cost( int row, int curr ) {
   
   if (row >= MAX_ROWS)
      return 0;

   if (mem[row][curr] != 0)
      return mem[row][curr];

   int val1 = paint_cost(row+1, ((curr+1)%3));
   int val2 = paint_cost(row+1, ((curr+2)%3));

   return mem[row][curr] = cost[row][curr] + MIN(val1,val2);

}


int main()
{
   int ret = MIN(paint_cost(0,0), MIN(paint_cost(0,1), paint_cost(0,2)));
   printf("%d\n", ret);

   return 0;

}
