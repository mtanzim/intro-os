#include <stdio.h>
#include <stdlib.h>

int main()
{
  float x = 5.0, y = 6.0;
  void swap_A(float *x, float *y), swap_V(float x, float y);

  printf("START:\t\t\t x = %6.2f, y = %6.2f\n", x, y);
  swap_V(x, y);
  printf("AFTER SWAP BY VALUE:\t x = %6.2f, y = %6.2f\n", x, y);
  swap_A(&x, &y);
  printf("AFTER SWAP BY REF:\t x = %6.2f, y = %6.2f\n", x, y);
}

void swap_A(float *x, float *y) /**  passes addresses of x and y  **/
{
  float tmp = *x;
  *x = *y;
  *y = tmp;
}

void swap_V(float x, float y) /**  passes values of x and y  **/
{
  float tmp = x;
  x = y;
  y = tmp;
}