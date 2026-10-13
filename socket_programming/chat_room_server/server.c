#include "server.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <pthread.h>


client_t       clients[MAX_CLIENTS];
pthread_mutex_t clients_lock = PTHREAD_MUTEX_INITIALIZER;

static fd_queue_t  worker_queues[NUM_WORKERS];
static worker_arg_t worker_args[NUM_WORKERS];
static pthread_t   worker_threads[NUM_WORKERS];

static volatile int running = 1;


int set_nonblocking(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static void client_register(int fd)
{
    pthread_mutex_lock(&clients_lock);
    if (fd < MAX_CLIENTS) {
        clients[fd].fd            = fd;
        clients[fd].authenticated = 0;
        memset(clients[fd].username, 0, USERNAME_MAX);
    }
    pthread_mutex_unlock(&clients_lock);
}

static void client_remove(int fd)
{
    pthread_mutex_lock(&clients_lock);
    if (fd < MAX_CLIENTS) {
        if (clients[fd].authenticated) {
            char msg[SEND_BUF_SIZE];
            int n = snprintf(msg, sizeof(msg),
                             "[Server] %s has left the chat.\n",
                             clients[fd].username);
            pthread_mutex_unlock(&clients_lock);
            broadcast(fd, msg, n);
            pthread_mutex_lock(&clients_lock);
        }
        clients[fd].fd            = -1;
        clients[fd].authenticated = 0;
        memset(clients[fd].username, 0, USERNAME_MAX);
    }
    pthread_mutex_unlock(&clients_lock);
    close(fd);
}

void broadcast(int sender_fd, const char *msg, int len)
{
    pthread_mutex_lock(&clients_lock);
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd > 0 && clients[i].authenticated && i != sender_fd) {
            /* best-effort, ignore partial/error for now */
            (void)write(clients[i].fd, msg, len);
        }
    }
    pthread_mutex_unlock(&clients_lock);
}


static void queue_init(fd_queue_t *q)
{
    q->head = q->tail = 0;
    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->cond, NULL);
}

static void queue_push(fd_queue_t *q, int fd)
{
    pthread_mutex_lock(&q->lock);
    q->fds[q->tail] = fd;
    q->tail = (q->tail + 1) % WORKER_QUEUE_SIZE;
    pthread_cond_signal(&q->cond);
    pthread_mutex_unlock(&q->lock);
}




static void handle_auth(int fd, char *line)
{
    /* first message from client is treated as username */
    line[strcspn(line, "\r\n")] = '\0';
    if (strlen(line) == 0 || strlen(line) >= USERNAME_MAX) {
        const char *err = "[Server] Invalid username. Disconnecting.\n";
        (void)write(fd, err, strlen(err));
        client_remove(fd);
        return;
    }

    pthread_mutex_lock(&clients_lock);
    clients[fd].authenticated = 1;
    strncpy(clients[fd].username, line, USERNAME_MAX - 1);
    pthread_mutex_unlock(&clients_lock);

    char welcome[SEND_BUF_SIZE];
    int  n = snprintf(welcome, sizeof(welcome),
                      "[Server] Welcome, %s! Type your messages.\n", line);
    (void)write(fd, welcome, n);

    char joined[SEND_BUF_SIZE];
    n = snprintf(joined, sizeof(joined),
                 "[Server] %s has joined the chat.\n", line);
    broadcast(fd, joined, n);
}

static void handle_message(int fd, char *line)
{
    line[strcspn(line, "\r\n")] = '\0';
    if (strlen(line) == 0) return;

    char msg[SEND_BUF_SIZE];
    pthread_mutex_lock(&clients_lock);
    int n = snprintf(msg, sizeof(msg), "[%s] %s\n",
                     clients[fd].username, line);
    pthread_mutex_unlock(&clients_lock);

    broadcast(fd, msg, n);
}

/*
 * Edge-triggered requires draining the entire receive buffer in one go.
 * Returns -1 when the connection should be closed.
 */
static int drain_fd(int fd)
{
    char buf[RECV_BUF_SIZE];

    while (1) {
        ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                return 0;  /* fully drained */
            return -1;
        }
        if (n == 0)
            return -1;     /* peer closed */

        buf[n] = '\0';

        /* split on newlines so multi-line reads work */
        char *saveptr;
        char *line = strtok_r(buf, "\n", &saveptr);
        while (line) {
            if (clients[fd].authenticated)
                handle_message(fd, line);
            else
                handle_auth(fd, line);
            line = strtok_r(NULL, "\n", &saveptr);
        }
    }
}


static void *worker_fn(void *arg)
{
    worker_arg_t    *wa  = (worker_arg_t *)arg;
    int              efd = wa->epoll_fd;
    struct epoll_event ev, events[MAX_EVENTS];

    while (running) {
        /* pick up any newly assigned fds from main */
        pthread_mutex_lock(&wa->queue->lock);
        while (wa->queue->head != wa->queue->tail) {
            int new_fd = wa->queue->fds[wa->queue->head];
            wa->queue->head = (wa->queue->head + 1) % WORKER_QUEUE_SIZE;
            pthread_mutex_unlock(&wa->queue->lock);

            ev.events  = EPOLLIN | EPOLLET;
            ev.data.fd = new_fd;
            if (epoll_ctl(efd, EPOLL_CTL_ADD, new_fd, &ev) < 0)
                perror("epoll_ctl add");

            /* ask the client for a username */
            const char *prompt = "[Server] Enter your username: ";
            (void)write(new_fd, prompt, strlen(prompt));

            pthread_mutex_lock(&wa->queue->lock);
        }
        pthread_mutex_unlock(&wa->queue->lock);

        int nfds = epoll_wait(efd, events, MAX_EVENTS, 50 /* ms timeout */);
        for (int i = 0; i < nfds; i++) {
            int fd = events[i].data.fd;

            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP)) {
                epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL);
                client_remove(fd);
                continue;
            }

            if (events[i].events & EPOLLIN) {
                if (drain_fd(fd) < 0) {
                    epoll_ctl(efd, EPOLL_CTL_DEL, fd, NULL);
                    client_remove(fd);
                }
            }
        }
    }
    return NULL;
}


static int create_server_socket(int port)
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return -1; }

    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family      = AF_INET,
        .sin_port        = htons(port),
        .sin_addr.s_addr = INADDR_ANY,
    };

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(fd); return -1;
    }
    if (listen(fd, SOMAXCONN) < 0) {
        perror("listen"); close(fd); return -1;
    }
    return fd;
}

static void sig_handler(int sig)
{
    (void)sig;
    running = 0;
}

int main(void)
{
    signal(SIGINT,  sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGPIPE, SIG_IGN);

    /* init client table */
    for (int i = 0; i < MAX_CLIENTS; i++)
        clients[i].fd = -1;

    /* spin up workers */
    for (int i = 0; i < NUM_WORKERS; i++) {
        queue_init(&worker_queues[i]);

        int efd = epoll_create1(0);
        if (efd < 0) { perror("epoll_create1"); return 1; }

        worker_args[i].worker_id = i;
        worker_args[i].epoll_fd  = efd;
        worker_args[i].queue     = &worker_queues[i];

        pthread_create(&worker_threads[i], NULL, worker_fn, &worker_args[i]);
        printf("[Main] Worker %d started (epoll_fd=%d)\n", i, efd);
    }

    /* server socket – blocking is fine for the accept loop */
    int server_fd = create_server_socket(SERVER_PORT);
    if (server_fd < 0) return 1;
    printf("[Main] Listening on port %d\n", SERVER_PORT);

    static unsigned int rr = 0;  /* round-robin counter */

    while (running) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(server_fd,
                               (struct sockaddr *)&client_addr,
                               &addr_len);
        if (client_fd < 0) {
            if (errno == EINTR)   continue;
            if (errno == EAGAIN)  continue;
            perror("accept");
            break;
        }

        if (client_fd >= MAX_CLIENTS) {
            fprintf(stderr, "[Main] Max clients reached, rejecting fd=%d\n",
                    client_fd);
            close(client_fd);
            continue;
        }

        if (set_nonblocking(client_fd) < 0) {
            perror("set_nonblocking");
            close(client_fd);
            continue;
        }

        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
        printf("[Main] New connection fd=%d from %s:%d -> worker %d\n",
               client_fd, ip, ntohs(client_addr.sin_port), rr % NUM_WORKERS);

        client_register(client_fd);

        /* distribute to worker via lock-free-ish ring buffer */
        queue_push(&worker_queues[rr % NUM_WORKERS], client_fd);
        rr++;
    }

    /* cleanup */
    running = 0;
    for (int i = 0; i < NUM_WORKERS; i++) {
        pthread_cond_signal(&worker_queues[i].cond);
        pthread_join(worker_threads[i], NULL);
        close(worker_args[i].epoll_fd);
    }
    close(server_fd);
    printf("[Main] Server shut down.\n");
    return 0;
}
