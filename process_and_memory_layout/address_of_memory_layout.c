/*
In this file, I implement a sample to demonstrate the memory layout of a process. Use the command
 `gcc -o memory_layout memory_layout.c` to build and `./memory_layout` to run the program. In the program, you will see the
  memory addresses of the Text, Data, BSS, Heap, and Stack segments.
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


int data = 52;
int bss;

void text_function() {

}

int main() {
    int stack = 5;
    int *heap = calloc(5,sizeof(int));

    printf(" Text: %p\n",text_function);
    printf(" Data: %p\n",&data);
    printf(" BSS: %p\n",&bss);
    printf(" HEAP: %p\n",heap);
    printf(" STACK: %p\n",&stack);

    
}