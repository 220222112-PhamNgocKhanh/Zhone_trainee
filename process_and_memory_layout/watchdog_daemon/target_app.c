#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <signal.h>
#include "common.h"

shared_data_t *shm_ptr = NULL;

void cleanup(void) {
    if (shm_ptr) {
        shm_ptr->is_alive = 0; // Signal clean termination to the daemon
        munmap(shm_ptr, sizeof(shared_data_t));
    }
}

void sig_handler(int sig) {
    cleanup();
    exit(EXIT_SUCCESS);
}

int main(void) {
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    /* Open or create the shared memory object */
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open failed");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd, sizeof(shared_data_t));
    shm_ptr = mmap(0, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    /* Register Target Process info */
    shm_ptr->pid = getpid();
    shm_ptr->last_heartbeat = time(NULL);
    shm_ptr->is_alive = 1;

    printf("[Target App] Started with PID: %d\n", getpid());

    int count = 0;
    while (1) {
        /* Update Heartbeat timestamp in Shared Memory */
        shm_ptr->last_heartbeat = time(NULL);
        printf("[Target App] Heartbeat updated at %ld\n", shm_ptr->last_heartbeat);

        sleep(1);
        count++;

        /* SIMULATION: Freeze after 10 seconds */
        if (count == 10) {
            printf("[Target App] SIMULATING FREEZE/DEADLOCK! (Heartbeats stopped...)\n");
            while (1) {
                sleep(3600); // Enter infinite sleep without updating heartbeat
            }
        }
    }

    cleanup();
    return EXIT_SUCCESS;
}