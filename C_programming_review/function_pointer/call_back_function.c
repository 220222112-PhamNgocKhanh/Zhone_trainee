#include <stdio.h>

/*
    This function will call the specific action in other function
*/
void action(void (*member)(), int number)
{
    for (int i = 0; i < number; i++)
    {
        member(number);
    }
}

/*
    Specific action 1
*/
void dog()
{
    printf("gau gau\n");
}

/*
    Specific action 2
*/
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