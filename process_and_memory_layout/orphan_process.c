/*
In this file, I implement a sample of an orphan process. Use the command `gcc -o orphan_process orphan_process.c` to build
 and `./orphan_process` to run the program. In the program, you will see the moment when the parent process dies and the child
  process is reparented to another process (the nearest ancestor subreaper).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/prctl.h>

int main()
{

    int pid = fork();
    if (pid == 0)
    {
        for (int i = 0; i < 100; i++)
        {
            printf("child: pid = %d, ppid = %d\n", getpid(), getppid());
            sleep(1);
        }
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            printf("parent: pid = %d\n", getpid());
            sleep(1);
        }
        exit(0);
    }
}