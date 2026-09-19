#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pthread.h>

#define FIFO_1_TO_2 "/tmp/chat_fifo_1_to_2"
#define FIFO_2_TO_1 "/tmp/chat_fifo_2_to_1"
#define BUFFER_SIZE 1024

/* Thread function to continuously receive messages from Client 1 */
void *receive_messages(void *arg)
{
    int fd_read = *(int *)arg;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = read(fd_read, buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytes_read] = '\0';
        printf("\nClient 1: %s", buffer);
        printf("Client 2: ");
        fflush(stdout);
    }

    if (bytes_read == 0)
    {
        printf("\nClient 1 has disconnected.\n");
    }
    else
    {
        perror("read");
    }

    exit(0);
    return NULL;
}

int main(void)
{
    int fd_write;
    int fd_read;
    pthread_t recv_thread;
    char buffer[BUFFER_SIZE];

    mkfifo(FIFO_1_TO_2, 0666);
    mkfifo(FIFO_2_TO_1, 0666);

    printf("[Client 2] Waiting for Client 1 to connect...\n");

    fd_read = open(FIFO_1_TO_2, O_RDONLY);
    if (fd_read < 0)
    {
        perror("open read fifo");
        exit(1);
    }

    fd_write = open(FIFO_2_TO_1, O_WRONLY);
    if (fd_write < 0)
    {
        perror("open write fifo");
        close(fd_read);
        exit(1);
    }

    printf("[Client 2] Connected!\n");

    if (pthread_create(&recv_thread, NULL, receive_messages, &fd_read) != 0)
    {
        perror("pthread_create");
        close(fd_read);
        close(fd_write);
        exit(1);
    }

    while (1)
    {
        printf("Client 2: ");
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            break;
        }

        if (write(fd_write, buffer, strlen(buffer)) < 0)
        {
            perror("write");
            break;
        }
    }

    close(fd_read);
    close(fd_write);
    unlink(FIFO_1_TO_2);
    unlink(FIFO_2_TO_1);

    return 0;
}
