/*
In this file, I implement a sample of a zombie process. Use the command `gcc -o zombie_process zombie_process.c` to build
 and `./zombie_process` to run the program. In the program, you will see the moment when the child process dies
  while the parent process is still running, causing the child process to become a zombie process.

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

int main()
{
    int i;
    int pid = fork();

    if (pid == 0)
    {
        for (i = 0; i < 20; i++)
            printf("Child process\n");
    }
    else
    {
        printf("Parent process\n");
        while (1)
            ;
    }

    return 0;
}