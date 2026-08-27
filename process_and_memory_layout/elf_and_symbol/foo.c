#include <stdio.h>

extern void missing_function(void);

void foo(void)
{
    printf("foo() is running\n");
    missing_function();
}