// Test 1: simple macro
#define PI 3.14
#define COUNT 5

// Test 2: function-like macro
#define SQUARE(x) ((x) * (x))

#include "test_header.h"

int main(void)
{
    int radius = 2;
    int area = PI * radius * radius;
    int number = SQUARE(COUNT);

    /* This is a
       multi-line comment. */

    printf("PI = %f\n", PI);
    printf("Area = %d\n", area);
    printf("Square = %d\n", number);
    printf("Header = %d\n", header_number);

    return 0; // End of test
}
