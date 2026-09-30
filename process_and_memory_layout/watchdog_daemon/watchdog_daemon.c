#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>
#include <sys/mman.h>
#include <errno.h>
#include <time.h>
#include "common.h"

volatile sig_atomic_t keep_running = 1;

void handle_signal(int sig) {
    if (sig == SIGTERM || sig == SIGINT) {
        keep_running = 0;
    }
}

/**
 * Traditional SysV Daemon Initialization
 */
static void skeleton_daemon(void) {
    pid_t pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    if (setsid() < 0) exit(EXIT_FAILURE);

    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    signal(SIGTERM, handle_signal);
    signal(SIGINT, handle_signal);

    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    umask(0);
    if (chdir("/") < 0) exit(EXIT_FAILURE);

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    int dev_null = open("/dev/null", O_RDWR);
    if (dev_null != -1) {
        dup2(dev_null, STDIN_FILENO);
        dup2(dev_null, STDOUT_FILENO);
        dup2(dev_null, STDERR_FILENO);
        if (dev_null > 2) close(dev_null);
    }

    openlog("watchdog_daemon", LOG_PID, LOG_DAEMON);
}

/**
 * Restarts the Target Process upon failure or deadlock detection
 */
void restart_target_process(void) {
    syslog(LOG_WARNING, "[Watchdog] Relaunching Target Process...");
    pid_t pid = fork();
    if (pid == 0) {
        char *args[] = {"/home/khanhnp/lab/Zhone_trainee/process_and_memory_layout/watchdog_daemon/target_app", NULL};
        execv(args[0], args);
        exit(EXIT_FAILURE); // Exec failed
    }
}

int main(void) {
    skeleton_daemon();
    syslog(LOG_NOTICE, "[Watchdog] Daemon started successfully.");

    /* Connect to Shared Memory Segment */
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(shared_data_t));
    shared_data_t *shm_ptr = mmap(0, sizeof(shared_data_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    while (keep_running) {
        sleep(2); // Inspection interval (seconds)

        pid_t target_pid = shm_ptr->pid;
        time_t now = time(NULL);

        if (target_pid <= 0) {
            syslog(LOG_INFO, "[Watchdog] Waiting for Target Process registration...");
            continue;
        }

        /* 1. CHECK IF PROCESS WAS KILLED OR CRASHED */
        if (kill(target_pid, 0) == -1 && errno == ESRCH) {
            syslog(LOG_ERR, "[Watchdog] CRITICAL: Target Process (PID %d) terminated unexpectedly!", target_pid);
            restart_target_process();
            continue;
        }

        /* 2. CHECK IF PROCESS IS FROZEN / DEADLOCKED */
        double diff = difftime(now, shm_ptr->last_heartbeat);
        if (diff > TIMEOUT_THRESHOLD && shm_ptr->is_alive) {
            syslog(LOG_ERR, "[Watchdog] WARNING: Target Process (PID %d) unresponsive for %.0f seconds.", target_pid, diff);

            /* Forcefully terminate frozen process */
            syslog(LOG_WARNING, "[Watchdog] Terminating frozen process (PID %d) via SIGKILL...", target_pid);
            kill(target_pid, SIGKILL);

            /* Reset PID entry and restart */
            shm_ptr->pid = 0;
            restart_target_process();
        } else {
            syslog(LOG_INFO, "[Watchdog] Target Process (PID %d) operational. Last heartbeat: %.0f sec ago.", target_pid, diff);
        }
    }

    syslog(LOG_NOTICE, "[Watchdog] Daemon shutting down...");
    munmap(shm_ptr, sizeof(shared_data_t));
    shm_unlink(SHM_NAME);
    closelog();

    return EXIT_SUCCESS;
}