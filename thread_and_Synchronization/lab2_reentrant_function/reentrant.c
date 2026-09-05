#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

double calculate_price(double price, double tax_rate)
{
    double final_price;

    final_price = price + price * tax_rate;

    usleep(1000);

    return final_price;
}

typedef struct
{
    double price;
    double tax_rate;
} ProductData;

void *thread_func(void *arg)
{
    ProductData *data = (ProductData *)arg;

    double result = calculate_price(
        data->price,
        data->tax_rate);

    printf("Price: %.2f\n",

           result);

    return NULL;
}

int main()
{
    pthread_t t1, t2;

    ProductData data1 = {100, 0.10};
    ProductData data2 = {200, 0.20};

    pthread_create(&t1, NULL, thread_func, &data1);
    pthread_create(&t2, NULL, thread_func, &data2);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
