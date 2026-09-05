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
#include <semaphore.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void *thread_A(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&lock1);
        sleep(3);
        pthread_mutex_lock(&lock2);

        printf("A: GOT BOTH LOCKS! Doing work...\n");

        pthread_mutex_unlock(&lock2);
        pthread_mutex_unlock(&lock1);

        break;
    }

    return NULL;
}

void *thread_B(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&lock1);
        pthread_mutex_lock(&lock2);

        printf("B: GOT BOTH LOCKS! Doing work...\n");

        pthread_mutex_unlock(&lock2);
        pthread_mutex_unlock(&lock1);

        break;
    }

    return NULL;
}

int main()
{
    pthread_t A, B;

    pthread_create(&A, NULL, thread_A, NULL);
    sleep(1);
    pthread_create(&B, NULL, thread_B, NULL);

    pthread_join(A, NULL);
    pthread_join(B, NULL);

    return 0;
}
