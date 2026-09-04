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

pthread_mutex_t mutex;

int count = 0;
void *thread1(void *arg)
{
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < 100000; i++)
    {
        count++;
    }
    pthread_mutex_unlock(&mutex);
}

int main()
{
    pthread_t tid;
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&tid, NULL, thread1, NULL);
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < 100000; i++)
    {
        count++;
    }
    pthread_mutex_unlock(&mutex);
    pthread_join(tid, NULL);

    printf("%d", count);

    return 0;
}