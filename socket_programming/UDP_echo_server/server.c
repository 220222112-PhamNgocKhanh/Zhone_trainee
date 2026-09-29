#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define DEFAULT_PORT 1234
#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    int port = DEFAULT_PORT;

    if (argc > 1) {
        port = atoi(argv[1]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Error: Invalid port '%s'. Must be between 1 and 65535.\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }

    // Create UDP socket
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_fd < 0) {
        perror("Failed to create socket");
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

    printf("=========================================\n");
    printf(" UDP Echo Server (Iterative)             \n");
    printf(" Listening on port: %d                   \n", port);
    printf("=========================================\n");

    char buffer[BUFFER_SIZE];
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while (1) {
        // Receive datagram from client
        ssize_t bytes_read = recvfrom(server_fd, buffer, sizeof(buffer), 0,
                                      (struct sockaddr *)&client_addr, &client_len);
        if (bytes_read < 0) {
            if (errno == EINTR) continue;
            perror("recvfrom error");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        int client_port = ntohs(client_addr.sin_port);

        printf("Received %zd bytes from %s:%d\n", bytes_read, client_ip, client_port);

        // Echo back to client
        ssize_t bytes_sent = sendto(server_fd, buffer, bytes_read, 0,
                                    (struct sockaddr *)&client_addr, client_len);
        if (bytes_sent < 0) {
            perror("sendto error");
        }
    }

    close(server_fd);
    return 0;
}
