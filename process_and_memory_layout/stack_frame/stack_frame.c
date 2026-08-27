#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/prctl.h>
#include <signal.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/resource.h>

int multiply_add(int a, int b, int c)
{
    int x = a * b;
    int y = x + c;
    return y;
}

int calculate(int a, int b, int c, int d, int e, int f, int g)
{
    int x = multiply_add(a, b, c);
    int y = x + d + e;
    return y + f + g;
}

int main()
{
    int result = calculate(2, 3, 4, 5, 6, 7, 8);

    printf("result = %d\n", result);

    return 0;
}