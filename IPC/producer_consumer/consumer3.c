#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define STOCK "/tmp/temp"
#define NUMBER_CLIENT "/tmp/temp"
#define project_id_shm 21
#define project_id_sem 2

int main()
{
    struct sembuf sem_number_client;
    struct sembuf sem_lock;

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

    int stock_id = shmget(key_shared_memory, 4, 0666);
    if (stock_id == -1)
    {
        perror("shmget failed (producer might not be running)");
        exit(1);
    }

    int client_id = semget(key_semaphore, 2, 0666);
    if (client_id == -1)
    {
        perror("semget failed (producer might not be running)");
        exit(1);
    }

    int *number_of_stock = shmat(stock_id, NULL, 0);
    if (number_of_stock == (void *)-1)
    {
        perror("shmat failed");
        exit(1);
    }

    printf("Consumer 3 started\n");

    while (1)
    {
        sem_number_client.sem_op = -1;
        sem_number_client.sem_num = 0;
        sem_number_client.sem_flg = 0;

        sem_lock.sem_op = -1;
        sem_lock.sem_num = 1;
        sem_lock.sem_flg = 0;

        semop(client_id, &sem_number_client, 1);
        semop(client_id, &sem_lock, 1);

        if (*number_of_stock <= 1)
        {
            printf("Consumer 3: out of stock\n");

            sem_lock.sem_op = 1;
            semop(client_id, &sem_lock, 1);

            sem_number_client.sem_op = 1;
            semop(client_id, &sem_number_client, 1);
        }
        else
        {
            *number_of_stock -= 2;
            printf("Consumer 3 sucessed, remaining: %d\n", *number_of_stock);

            sem_lock.sem_op = 1;
            semop(client_id, &sem_lock, 1);

            sem_number_client.sem_op = 1;
            semop(client_id, &sem_number_client, 1);
        }
        sleep(3);
    }

    return 0;
}