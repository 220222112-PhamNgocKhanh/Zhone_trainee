#pragma once

#include <stdint.h>
#include <pthread.h>
#include <sys/epoll.h>

#define SERVER_PORT       5000
#define MAX_EVENTS        64
#define NUM_WORKERS       4
#define MAX_CLIENTS       1024
#define RECV_BUF_SIZE     4096
#define USERNAME_MAX      32
#define SEND_BUF_SIZE     (RECV_BUF_SIZE + USERNAME_MAX + 8)
#define WORKER_QUEUE_SIZE 256

typedef struct {
    int  fd;
    char username[USERNAME_MAX];
    int  authenticated;
} client_t;

typedef struct {
    int  fds[WORKER_QUEUE_SIZE];
    int  head;
    int  tail;
    pthread_mutex_t lock;
    pthread_cond_t  cond;
} fd_queue_t;

typedef struct {
    int       worker_id;
    int       epoll_fd;
    fd_queue_t *queue;
} worker_arg_t;

extern client_t  clients[MAX_CLIENTS];
extern pthread_mutex_t clients_lock;

void broadcast(int sender_fd, const char *msg, int len);
int  set_nonblocking(int fd);

