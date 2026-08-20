#include <stdio.h>

void action(void (*member)(), int number)
{
    for (int i = 0; i < number; i++)
    {
        member(number);
    }
}

void dog()
{
    printf("gau gau\n");
}

void cat()
{
    printf("meo meo\n");
}

int main()
{
    void (*func)();
    func = dog;
    action(func, 5);
    func = cat;
    action(func, 3);
}