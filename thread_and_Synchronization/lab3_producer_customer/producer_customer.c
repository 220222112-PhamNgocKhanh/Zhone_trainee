#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "queue.h"

#define QUEUE_SIZE 5
#define NUM_PRODUCERS 3
#define JOBS_PER_PRODUCER 5

queue *head = NULL;
queue *tail = NULL;

int queue_count = 0;

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t queue_not_empty = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_not_full = PTHREAD_COND_INITIALIZER;

void *producer(void *arg)
{
    int producer_id = *(int *)arg;

    for (int i = 0; i < JOBS_PER_PRODUCER; i++)
    {

        int job_id = producer_id * 100 + i;

        pthread_mutex_lock(&queue_mutex);

        while (queue_count == QUEUE_SIZE)
        {

            printf("Producer %d: queue is full, waiting...\n", producer_id);

            pthread_cond_wait(&queue_not_full, &queue_mutex);
        }

        push(&head, &tail, job_id);

        queue_count++;

        printf(
            "Producer %d: added job %d (queue = %d)\n", producer_id, job_id, queue_count);

        pthread_cond_signal(&queue_not_empty);

        pthread_mutex_unlock(&queue_mutex);

        usleep(100000);
    }

    return NULL;
}

void *consumer(void *arg)
{

    while (1)
    {

        pthread_mutex_lock(&queue_mutex);

        while (queue_count == 0)
        {

            printf("Printer: queue is empty, waiting...\n");

            pthread_cond_wait(&queue_not_empty, &queue_mutex);
        }

        int job_id = pop(&head, &tail);

        queue_count--;

        printf(
            "Printer: processing job %d (queue = %d)\n", job_id, queue_count);

        pthread_cond_signal(&queue_not_full);

        pthread_mutex_unlock(&queue_mutex);

        usleep(300000);
    }

    return NULL;
}

int main()
{
    pthread_t producers[NUM_PRODUCERS];
    pthread_t printer;

    int producer_ids[NUM_PRODUCERS];

    pthread_create(&printer, NULL, consumer, NULL);

    for (int i = 0; i < NUM_PRODUCERS; i++)
    {

        producer_ids[i] = i + 1;

        pthread_create(&producers[i], NULL, producer, &producer_ids[i]);
    }

    for (int i = 0; i < NUM_PRODUCERS; i++)
    {

        pthread_join(producers[i], NULL);
    }

    pthread_join(printer, NULL);

    pthread_mutex_destroy(&queue_mutex);

    pthread_cond_destroy(&queue_not_empty);
    pthread_cond_destroy(&queue_not_full);

    return 0;
}
