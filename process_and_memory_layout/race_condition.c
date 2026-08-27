/*
As we can see, this situation is the new process will destroy the logic that child process still run, that mean
  a new process make the data space of child process become wrong. Therefore, the parent process can't reveive the exactly result.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/prctl.h>
#include <signal.h>
#include <sys/mman.h>
#include <pthread.h>
#include <sys/resource.h>

int main()
{
    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if (pid == 0)
    {
        close(fd[0]);

        printf("child calculates the value: \n");
        /*
        Something logic, then imagine the child process have the result is 25
        */
        int n = 25;
        execlp("ls", "ls", NULL);

        write(fd[1], &n, 4);

        close(fd[1]);
    }
    else
    {

        close(fd[1]);

        int n;
        read(fd[0], &n, 4);
        wait(NULL);

        printf("parent received: %d\n", n);

        close(fd[0]);
    }
}