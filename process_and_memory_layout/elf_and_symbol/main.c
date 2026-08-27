#include <stdio.h>
void foo(void);

int main(void)
{
    printf("the program normally run");
    getchar();
    foo();
    return 0;
}