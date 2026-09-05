#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

double TAX_RATE = 0.10;   

double calculate_price(double price)
{
    double final_price;

    final_price = price + price * TAX_RATE;

    usleep(1000);

    return final_price;
}

void *thread_func(void *arg)
{
    double price = *(double *)arg;

    printf("Price: %.2f\n", calculate_price(price));

    return NULL;
}

int main()
{
    pthread_t t1, t2;

    double price1 = 100;
    double price2 = 200;

    pthread_create(&t1, NULL, thread_func, &price1);
    pthread_create(&t2, NULL, thread_func, &price2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
