#define _GNU_SOURCE
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
#include <sched.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <mqueue.h>
#include <semaphore.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>


#define STOCK "/tmp/temp"
#define NUMBER_CLIENT "/tmp/temp"
#define project_id_shm 21
#define project_id_sem 2

int stock_id = 0;
int client_id = 0;


void handler_sigint(int sig) {
    (void)sig;
    shmctl(stock_id, IPC_RMID, NULL);
    semctl(client_id, 0, IPC_RMID);
    exit(0);
}


int main()
{
    sigset_t set;
    sigemptyset(&set);
    struct sigaction act;
    act.sa_handler = handler_sigint;
    act.sa_mask = set;
    act.sa_flags = 0;
    sigaction(SIGINT, &act, NULL);

    int fd = open(STOCK, O_CREAT | O_RDWR, 0666);
    if (fd != -1)
    {
        close(fd);
    }
    else
    {
        perror("Creating file failed");
        return 1;
    }
    key_t key_shared_memory = ftok(STOCK, project_id_shm);
    if (key_shared_memory == -1)
    {
        perror("ftok shared memory failed");
        exit(1);
    }

    key_t key_semaphore = ftok(NUMBER_CLIENT, project_id_sem);
    if (key_semaphore == -1)
    {
        perror("ftok semaphore failed");
        exit(1);
    }

    stock_id = shmget(key_shared_memory, 4, 0666 | IPC_CREAT);
    if (stock_id == -1)
    {
        perror("shmget failed");
        exit(1);
    }

     client_id = semget(key_semaphore, 2, 0666 | IPC_CREAT);
    if (client_id == -1)
    {
        perror("semget failed");
        exit(1);
    }

    int *number_of_stock = shmat(stock_id, NULL, 0);
    if (number_of_stock == (void *)-1)
    {
        perror("shmat failed");
        exit(1);
    }

    if (semctl(client_id, 0, SETVAL, 2) == -1)
    {
        perror("semctl client failed");
        exit(1);
    }

    if (semctl(client_id, 1, SETVAL, 1) == -1)
    {
        perror("semctl lock failed");
        exit(1);
    }

    struct sembuf sem_lock;
    sem_lock.sem_num = 1;
    sem_lock.sem_flg = 0;

    *number_of_stock = 10;
    printf("Producer started, initial stock = %d\n", *number_of_stock);

    while (1)
    {
        sleep(10);

        sem_lock.sem_op = -1;
        semop(client_id, &sem_lock, 1);

        if (*number_of_stock < 10)
        {
            *number_of_stock = 10;
            printf("Producer: refilled stock to 10\n");
        }

        sem_lock.sem_op = 1;
        semop(client_id, &sem_lock, 1);
    }

    semctl(client_id, 0, IPC_RMID);
    shmctl(stock_id, IPC_RMID, NULL);
    return 0;
}
