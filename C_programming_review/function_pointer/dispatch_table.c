#include <stdio.h>

typedef int (*Operation)(int, int);

/* 
    Adds two integer values and returns the result
 */
int add(int a, int b)
{
    return a + b;
}

/* 
    Subtracts the second integer from the first and returns the result
 */
int subtract(int a, int b)
{
    return a - b;
}

/* 
    Multiplies two integer values and returns the result
 */
int multiply(int a, int b)
{
    return a * b;
}

/* 
    Divides the first integer by the second and returns the result
 */
int divide(int a, int b)
{
    return a / b;
}

/* 
    Demonstrates calling math operations via an array of function pointers
 */
int main(void)
{
    Operation operations[] = {
        add,
        subtract,
        multiply,
        divide};

    int a = 10;
    int b = 5;
    int choice = 2;

    int result = operations[choice](a, b);

    printf("Result = %d\n", result);

    return 0;
}