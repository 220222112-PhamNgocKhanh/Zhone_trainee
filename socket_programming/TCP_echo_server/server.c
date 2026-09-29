#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define DEFAULT_PORT 1234
#define BACKLOG 10
#define BUFFER_SIZE 4096

/**
 * Signal handler for SIGCHLD to prevent zombie processes.
 */
void handle_sigchld(int signo) {
    (void)signo;
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        // Reap all dead child processes
    }
    errno = saved_errno;
}

/**
 * Handles communication with a connected client.
 * Executed in the child process.
 */
void handle_client(int client_fd, struct sockaddr_in *client_addr) {
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr->sin_addr, client_ip, sizeof(client_ip));
    int client_port = ntohs(client_addr->sin_port);

    printf("[Child PID %d] Connected to client %s:%d\n", getpid(), client_ip, client_port);

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;

    // Read data from socket and echo back exact content
    while ((bytes_read = recv(client_fd, buffer, sizeof(buffer), 0)) > 0) {
        ssize_t total_sent = 0;
        while (total_sent < bytes_read) {
            ssize_t bytes_sent = send(client_fd, buffer + total_sent, bytes_read - total_sent, 0);
            if (bytes_sent < 0) {
                if (errno == EINTR) {
                    continue;
                }
                perror("[Child] send failed");
                close(client_fd);
                return;
            }
            total_sent += bytes_sent;
        }
    }

    if (bytes_read == 0) {
        printf("[Child PID %d] Client %s:%d disconnected.\n", getpid(), client_ip, client_port);
    } else {
        perror("[Child] recv error");
    }

    close(client_fd);
}

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;

    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Error: Invalid port '%s'. Must be between 1 and 65535.\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    // Set up SIGCHLD handler to reap terminated child processes
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigchld;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("Failed to set SIGCHLD handler");
        exit(EXIT_FAILURE);
    }

    // Create TCP socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Enable SO_REUSEADDR to reuse port quickly upon restart
    int optval = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        perror("Failed to set SO_REUSEADDR");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Configure server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to bind socket");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_fd, BACKLOG) < 0) {
        perror("Failed to listen on socket");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("=========================================\n");
    printf(" TCP Echo Server (fork-per-connection)   \n");
    printf(" Listening on port: %d (PID: %d)          \n", port, getpid());
    printf("=========================================\n");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("accept error");
            continue;
        }

        // Fork child process to handle the connection
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork error");
            close(client_fd);
            continue;
        }

        if (pid == 0) {
            // In child process:
            close(server_fd); // Child doesn't need listening socket
            handle_client(client_fd, &client_addr);
            exit(EXIT_SUCCESS);
        } else {
            // In parent process:
            close(client_fd); // Parent doesn't need client socket
        }
    }

    close(server_fd);
    return 0;
}

