#include <stdio.h>
#include <stdlib.h>

int main()
{
    int x = 5, y = 6, *p;
    // & means the address of
    p = &x;
    // * dereferences the pointer
    printf("1.  x=%d, y=%d, *p=%d\n", x, y, *p);
    x = 45;
    printf("1.  x=%d, y=%d, *p=%d\n", x, y, *p);
    *p = 48;
    printf("1.  x=%d, y=%d, *p=%d\n", x, y, *p);
    p = &y;
    printf("1.  x=%d, y=%d, *p=%d\n", x, y, *p);
    *p = 9078;
    printf("1.  x=%d, y=%d, *p=%d\n", x, y, *p);
}