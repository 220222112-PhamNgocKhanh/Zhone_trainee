#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/epoll.h>
#include <time.h>
#include <errno.h>

#define DEFAULT_ITERATIONS 2000
#define WARMUP_ITERATIONS   50
#define MAX_EVENTS          16

typedef struct {
    double min_us;
    double max_us;
    double avg_us;
    double median_us;
    double p95_us;
    double p99_us;
} benchmark_stats_t;

typedef struct {
    int num_fds;
    benchmark_stats_t poll_syscall;
    benchmark_stats_t poll_total;
    benchmark_stats_t epoll_syscall;
    benchmark_stats_t epoll_total;
} test_result_t;

static inline double timespec_diff_us(const struct timespec *start, const struct timespec *end) {
    return (double)(end->tv_sec - start->tv_sec) * 1e6 +
           (double)(end->tv_nsec - start->tv_nsec) / 1e3;
}

static int compare_double(const void *a, const void *b) {
    double da = *(const double *)a;
    double db = *(const double *)b;
    if (da < db) return -1;
    if (da > db) return 1;
    return 0;
}

static void compute_stats(double *samples, int count, benchmark_stats_t *stats) {
    qsort(samples, count, sizeof(double), compare_double);

    double sum = 0.0;
    for (int i = 0; i < count; i++) {
        sum += samples[i];
    }

    stats->min_us = samples[0];
    stats->max_us = samples[count - 1];
    stats->avg_us = sum / count;
    stats->median_us = samples[count / 2];
    stats->p95_us = samples[(int)(count * 0.95)];
    stats->p99_us = samples[(int)(count * 0.99)];
}

static int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static int setup_pipes(int num_fds, int (*pipes)[2]) {
    for (int i = 0; i < num_fds; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("pipe");
            for (int j = 0; j < i; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            return -1;
        }
        set_nonblocking(pipes[i][0]);
        set_nonblocking(pipes[i][1]);
    }
    return 0;
}

static void cleanup_pipes(int num_fds, int (*pipes)[2]) {
    for (int i = 0; i < num_fds; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

static int benchmark_poll(int num_fds, int (*pipes)[2], int iters,
                          benchmark_stats_t *syscall_stats,
                          benchmark_stats_t *total_stats) {
    struct pollfd *pfds = malloc(sizeof(struct pollfd) * num_fds);
    double *syscall_samples = malloc(sizeof(double) * iters);
    double *total_samples = malloc(sizeof(double) * iters);

    if (!pfds || !syscall_samples || !total_samples) {
        perror("malloc");
        free(pfds);
        free(syscall_samples);
        free(total_samples);
        return -1;
    }

    for (int i = 0; i < num_fds; i++) {
        pfds[i].fd = pipes[i][0];
        pfds[i].events = POLLIN;
        pfds[i].revents = 0;
    }

    for (int it = 0; it < WARMUP_ITERATIONS; it++) {
        int target_idx = (it * 37 + 13) % num_fds;
        if (write(pipes[target_idx][1], "w", 1) != 1) {
            perror("write");
            goto error;
        }
        if (poll(pfds, num_fds, -1) < 0) {
            perror("poll");
            goto error;
        }
        char buf;
        if (read(pipes[target_idx][0], &buf, 1) != 1) {
            perror("read");
            goto error;
        }
    }

    for (int it = 0; it < iters; it++) {
        int target_idx = (it * 37 + 13) % num_fds;
        if (write(pipes[target_idx][1], "x", 1) != 1) {
            perror("write");
            goto error;
        }

        struct timespec t_start, t_poll_end, t_find_end;
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        int ret = poll(pfds, num_fds, -1);
        clock_gettime(CLOCK_MONOTONIC, &t_poll_end);

        if (ret < 0) {
            perror("poll");
            goto error;
        }

        int ready_fd = -1;
        for (int i = 0; i < num_fds; i++) {
            if (pfds[i].revents & POLLIN) {
                ready_fd = pfds[i].fd;
                break;
            }
        }
        clock_gettime(CLOCK_MONOTONIC, &t_find_end);
        (void)ready_fd;

        char buf;
        if (read(pipes[target_idx][0], &buf, 1) != 1) {
            perror("read");
            goto error;
        }

        syscall_samples[it] = timespec_diff_us(&t_start, &t_poll_end);
        total_samples[it] = timespec_diff_us(&t_start, &t_find_end);
    }

    compute_stats(syscall_samples, iters, syscall_stats);
    compute_stats(total_samples, iters, total_stats);

    free(pfds);
    free(syscall_samples);
    free(total_samples);
    return 0;

error:
    free(pfds);
    free(syscall_samples);
    free(total_samples);
    return -1;
}

static int benchmark_epoll(int num_fds, int (*pipes)[2], int iters,
                           benchmark_stats_t *syscall_stats,
                           benchmark_stats_t *total_stats) {
    int epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("epoll_create1");
        return -1;
    }

    struct epoll_event ev;
    struct epoll_event events[MAX_EVENTS];
    double *syscall_samples = malloc(sizeof(double) * iters);
    double *total_samples = malloc(sizeof(double) * iters);

    if (!syscall_samples || !total_samples) {
        perror("malloc");
        close(epfd);
        free(syscall_samples);
        free(total_samples);
        return -1;
    }

    for (int i = 0; i < num_fds; i++) {
        ev.events = EPOLLIN;
        ev.data.fd = pipes[i][0];
        if (epoll_ctl(epfd, EPOLL_CTL_ADD, pipes[i][0], &ev) < 0) {
            perror("epoll_ctl");
            close(epfd);
            free(syscall_samples);
            free(total_samples);
            return -1;
        }
    }

    for (int it = 0; it < WARMUP_ITERATIONS; it++) {
        int target_idx = (it * 37 + 13) % num_fds;
        if (write(pipes[target_idx][1], "w", 1) != 1) {
            perror("write");
            goto error;
        }
        if (epoll_wait(epfd, events, MAX_EVENTS, -1) < 0) {
            perror("epoll_wait");
            goto error;
        }
        char buf;
        if (read(pipes[target_idx][0], &buf, 1) != 1) {
            perror("read");
            goto error;
        }
    }

    for (int it = 0; it < iters; it++) {
        int target_idx = (it * 37 + 13) % num_fds;
        if (write(pipes[target_idx][1], "x", 1) != 1) {
            perror("write");
            goto error;
        }

        struct timespec t_start, t_wait_end, t_find_end;
        clock_gettime(CLOCK_MONOTONIC, &t_start);
        int ret = epoll_wait(epfd, events, MAX_EVENTS, -1);
        clock_gettime(CLOCK_MONOTONIC, &t_wait_end);

        if (ret < 0) {
            perror("epoll_wait");
            goto error;
        }

        int ready_fd = events[0].data.fd;
        clock_gettime(CLOCK_MONOTONIC, &t_find_end);
        (void)ready_fd;

        char buf;
        if (read(pipes[target_idx][0], &buf, 1) != 1) {
            perror("read");
            goto error;
        }

        syscall_samples[it] = timespec_diff_us(&t_start, &t_wait_end);
        total_samples[it] = timespec_diff_us(&t_start, &t_find_end);
    }

    compute_stats(syscall_samples, iters, syscall_stats);
    compute_stats(total_samples, iters, total_stats);

    close(epfd);
    free(syscall_samples);
    free(total_samples);
    return 0;

error:
    close(epfd);
    free(syscall_samples);
    free(total_samples);
    return -1;
}

static void print_section_header(int num_fds) {
    printf("\n");
    printf("========================================================================================\n");
    printf(" Benchmark with %d File Descriptors (1 ready I/O)\n", num_fds);
    printf("========================================================================================\n");
    printf("%-20s %10s %10s %10s %10s %10s %10s\n",
           "Operation", "Avg(us)", "Min(us)", "Median(us)", "P95(us)", "P99(us)", "Max(us)");
    printf("----------------------------------------------------------------------------------------\n");
}

static void print_stats_row(const char *name, const benchmark_stats_t *s) {
    printf("%-20s %10.3f %10.3f %10.3f %10.3f %10.3f %10.3f\n",
           name, s->avg_us, s->min_us, s->median_us, s->p95_us, s->p99_us, s->max_us);
}

int main(int argc, char *argv[]) {
    int iterations = DEFAULT_ITERATIONS;
    if (argc > 1) {
        int parsed = atoi(argv[1]);
        if (parsed > 0) {
            iterations = parsed;
        }
    }

    const int test_cases[] = {10, 100, 1000, 2000};
    const int num_cases = (int)(sizeof(test_cases) / sizeof(test_cases[0]));
    test_result_t results[4];

    printf("========================================================================================\n");
    printf(" I/O Multiplexing Benchmark: poll vs epoll\n");
    printf(" Iterations: %d per test case | Monitored FDs with exactly 1 ready event\n", iterations);
    printf("========================================================================================\n");

    for (int c = 0; c < num_cases; c++) {
        int num_fds = test_cases[c];
        int (*pipes)[2] = malloc(sizeof(int[2]) * num_fds);
        if (!pipes) {
            perror("malloc");
            return 1;
        }

        if (setup_pipes(num_fds, pipes) < 0) {
            free(pipes);
            return 1;
        }

        results[c].num_fds = num_fds;

        if (benchmark_poll(num_fds, pipes, iterations,
                           &results[c].poll_syscall,
                           &results[c].poll_total) < 0) {
            fprintf(stderr, "Failed benchmark_poll for %d fds\n", num_fds);
            cleanup_pipes(num_fds, pipes);
            free(pipes);
            return 1;
        }

        if (benchmark_epoll(num_fds, pipes, iterations,
                            &results[c].epoll_syscall,
                            &results[c].epoll_total) < 0) {
            fprintf(stderr, "Failed benchmark_epoll for %d fds\n", num_fds);
            cleanup_pipes(num_fds, pipes);
            free(pipes);
            return 1;
        }

        print_section_header(num_fds);
        print_stats_row("poll (syscall)", &results[c].poll_syscall);
        print_stats_row("poll (sys+dispatch)", &results[c].poll_total);
        print_stats_row("epoll (syscall)", &results[c].epoll_syscall);
        print_stats_row("epoll (sys+dispatch)", &results[c].epoll_total);

        double speedup_syscall = results[c].poll_syscall.avg_us / results[c].epoll_syscall.avg_us;
        double speedup_total = results[c].poll_total.avg_us / results[c].epoll_total.avg_us;
        printf("----------------------------------------------------------------------------------------\n");
        printf(" Speedup (epoll over poll) -> Syscall: %.2fx | Total (Sys+Dispatch): %.2fx\n",
               speedup_syscall, speedup_total);

        cleanup_pipes(num_fds, pipes);
        free(pipes);
    }

    printf("\n");
    printf("========================================================================================\n");
    printf(" FINAL SUMMARY COMPARISON (Average Latency in microseconds)\n");
    printf("========================================================================================\n");
    printf("%-10s | %-16s %-16s %-12s | %-16s %-16s %-12s\n",
           "Num FDs", "poll sys(us)", "epoll sys(us)", "Sys Speedup",
           "poll tot(us)", "epoll tot(us)", "Tot Speedup");
    printf("----------------------------------------------------------------------------------------\n");

    for (int c = 0; c < num_cases; c++) {
        double sys_speedup = results[c].poll_syscall.avg_us / results[c].epoll_syscall.avg_us;
        double tot_speedup = results[c].poll_total.avg_us / results[c].epoll_total.avg_us;
        printf("%-10d | %-16.3f %-16.3f %-11.2fx | %-16.3f %-16.3f %-11.2fx\n",
               results[c].num_fds,
               results[c].poll_syscall.avg_us,
               results[c].epoll_syscall.avg_us,
               sys_speedup,
               results[c].poll_total.avg_us,
               results[c].epoll_total.avg_us,
               tot_speedup);
    }
    printf("========================================================================================\n");

    return 0;
}
