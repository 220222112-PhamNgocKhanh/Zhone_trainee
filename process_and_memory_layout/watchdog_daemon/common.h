#ifndef COMMON_H
#define COMMON_H

#include <time.h>
#include <sys/types.h>

#define SHM_NAME "/my_watchdog_shm"
#define TIMEOUT_THRESHOLD 5 // Heartbeat timeout in seconds

typedef struct {
    pid_t pid;             // PID of the target process
    time_t last_heartbeat; // Timestamp of the last update
    int is_alive;          // Active status flag
} shared_data_t;

#endif