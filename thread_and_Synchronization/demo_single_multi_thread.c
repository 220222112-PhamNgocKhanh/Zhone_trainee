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
#include <time.h>
#include <math.h>

#define N 20000000

typedef struct
{
    long long start;
    long long end;
    double result;
} ThreadData;

double calculate(long long i)
{
    double x = (double)i;

    return sin(x) * sin(x) + cos(x) * cos(x) + sqrt(x + 1.0);
}

/*
   SINGLE THREAD
*/

double single_thread()
{
    double sum = 0.0;

    for (long long i = 0; i < N; i++)
    {
        sum += calculate(i);
    }

    return sum;
}

/*
   MULTI THREAD
*/

void *worker(void *arg)
{
    ThreadData *data = (ThreadData *)arg;

    double sum = 0.0;

    for (long long i = data->start; i < data->end; i++)
    {

        sum += calculate(i);
    }

    data->result = sum;

    return NULL;
}

double multi_thread(int num_threads)
{
    pthread_t *threads =
        malloc(num_threads * sizeof(pthread_t));

    ThreadData *data =
        malloc(num_threads * sizeof(ThreadData));

    long long chunk = N / num_threads;

    for (int i = 0; i < num_threads; i++)
    {

        data[i].start = i * chunk;

        if (i == num_threads - 1)
            data[i].end = N;
        else
            data[i].end = (i + 1) * chunk;

        data[i].result = 0.0;

        pthread_create(&threads[i], NULL, worker, &data[i]);
    }

    double total = 0.0;

    for (int i = 0; i < num_threads; i++)
    {

        pthread_join(threads[i], NULL);

        total += data[i].result;
    }

    free(threads);
    free(data);

    return total;
}

/*
   TIME FUNCTION
*/

double get_time()
{
    struct timespec ts;

    clock_gettime(
        CLOCK_MONOTONIC,
        &ts);

    return ts.tv_sec +
           ts.tv_nsec / 1e9;
}

/*
   MAIN
*/

int main()
{
    double start, end;
    double result;

    printf("N = %d\n\n", N);

    /* Single thread */

    start = get_time();

    result = single_thread();

    end = get_time();

    double single_time = end - start;

    printf("Single-thread\n");
    printf("Result = %.2f\n", result);
    printf("Time   = %.6f seconds\n\n",
           single_time);

    /* Multi-thread */

    int thread_counts[] = {2, 4, 8};
    int num_tests = 3;

    for (int i = 0; i < num_tests; i++)
    {

        int threads = thread_counts[i];

        start = get_time();

        result = multi_thread(threads);

        end = get_time();

        double multi_time = end - start;

        printf("%d threads\n", threads);
        printf("Result = %.2f\n", result);
        printf("Time   = %.6f seconds\n",
               multi_time);

        printf("Speedup = %.2fx\n\n",
               single_time / multi_time);
    }

    return 0;
}
