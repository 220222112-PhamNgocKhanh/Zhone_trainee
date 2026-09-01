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

#define ITERATIONS 1000

pthread_spinlock_t spinlock;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void do_work(long loops)
{
    long x = 0;

    for (long i = 0; i < loops; i++)
    {
        x += i;
    }
}

/*
 * Get current time in nanoseconds.
 */
long long get_time_ns()
{
    struct timespec time;

    clock_gettime(CLOCK_MONOTONIC, &time);

    return (long long)time.tv_sec * 1000000000LL + time.tv_nsec;
}

/* =========================================================
 * SPINLOCK TEST
 * ========================================================= */

void *spinlock_worker(void *arg)
{
    long loops = *(long *)arg;

    for (int i = 0; i < ITERATIONS; i++)
    {

        pthread_spin_lock(&spinlock);

        do_work(loops);

        pthread_spin_unlock(&spinlock);
    }

    return NULL;
}

/* =========================================================
 * MUTEX TEST
 * ========================================================= */

void *mutex_worker(void *arg)
{
    long loops = *(long *)arg;

    for (int i = 0; i < ITERATIONS; i++)
    {

        pthread_mutex_lock(&mutex);

        do_work(loops);

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

/* =========================================================
 * RUN SPINLOCK TEST
 * ========================================================= */

void test_spinlock(long loops)
{
    pthread_t thread1;
    pthread_t thread2;

    pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);

    long long start = get_time_ns();

    pthread_create(&thread1, NULL, spinlock_worker, &loops);
    pthread_create(&thread2, NULL, spinlock_worker, &loops);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    long long end = get_time_ns();

    printf("Spinlock: %.3f ms\n",
           (end - start) / 1000000.0);

    pthread_spin_destroy(&spinlock);
}

/* =========================================================
 * RUN MUTEX TEST
 * ========================================================= */

void test_mutex(long loops)
{
    pthread_t thread1;
    pthread_t thread2;

    long long start = get_time_ns();

    pthread_create(&thread1, NULL, mutex_worker, &loops);
    pthread_create(&thread2, NULL, mutex_worker, &loops);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    long long end = get_time_ns();

    printf("Mutex   : %.3f ms\n",
           (end - start) / 1000000.0);
}


int main()
{
    /*
     * Test 1: Short critical section
     */
    printf("\n===== SHORT CRITICAL SECTION =====\n");

    long short_work = 50;

    test_spinlock(short_work);
    test_mutex(short_work);

    /*
     * Test 2: Long critical section
     */
    printf("\n===== LONG CRITICAL SECTION =====\n");

    long long_work = 500000;

    test_spinlock(long_work);
    test_mutex(long_work);

    return 0;
}