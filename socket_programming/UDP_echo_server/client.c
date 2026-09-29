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
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <server_ip> [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];
    int port = DEFAULT_PORT;

    if (argc > 2) {
        port = atoi(argv[2]);
        if (port <= 0 || port > 65535) {
            fprintf(stderr, "Error: Invalid port '%s'. Must be between 1 and 65535.\n", argv[2]);
            exit(EXIT_FAILURE);
        }
    }

    // Create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        fprintf(stderr, "Error: Invalid IP address '%s'\n", server_ip);
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Connect the UDP socket (connected UDP)
    // This allows using send/recv and receiving asynchronous errors (like ICMP port unreachable)
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to connect UDP socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Connected UDP socket to %s:%d\n", server_ip, port);
    printf("Type a message and press Enter (Ctrl+D to quit):\n");

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
        size_t len = strlen(buffer);
        
        // Use send() instead of sendto() because the socket is connected
        ssize_t bytes_sent = send(sockfd, buffer, len, 0);
        if (bytes_sent < 0) {
            perror("send failed");
            // If the server is down, we might get ECONNREFUSED here (on Linux)
            continue;
        }

        // Use recv() instead of recvfrom() because the socket is connected
        ssize_t bytes_read = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_read < 0) {
            perror("recv failed");
            continue;
        }

        buffer[bytes_read] = '\0';
        printf("Echo from server: %s", buffer);
    }

    close(sockfd);
    printf("\nDisconnected.\n");
    return 0;
}
