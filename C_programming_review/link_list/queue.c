/*
    This file i use the doubly linklist structure to implement simple function of queue datatype
    The way to run: Compile this file and doubly_link_list.c file together
*/

#include <stdio.h>
#include "doubly_link_list.h"

typedef dlink_list queue;


/*
    This function push the head of queue 
*/
void push(queue **head, queue **tail, int data)
{
    insert_head(head, tail, data);
}

/*
    This function get the element from tail of queue and remove it
*/
int pop(queue **head, queue **tail)
{
    return get_and_remove_tail(head, tail);
}

int main()
{
    queue *head = NULL;
    queue *tail = NULL;
    push(&head, &tail, 6);
    push(&head, &tail, 7);
    push(&head, &tail, 8);

    printf("%d\n", pop(&head, &tail));
    printf("%d\n", pop(&head, &tail));
    printf("%d\n", pop(&head, &tail));
}