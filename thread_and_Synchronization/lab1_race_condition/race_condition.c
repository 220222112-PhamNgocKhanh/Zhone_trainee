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

int count = 0;
void *thread1(void *arg)
{
    for (int i = 0; i < 100000; i++)
    {
        count++;
    }
}

int main()
{
    pthread_t tid;

    pthread_create(&tid, NULL, thread1, NULL);
    for (int i = 0; i < 100000; i++)
    {
        count++;
    }
    pthread_join(tid, NULL);

    printf("%d", count);

    return 0;
}