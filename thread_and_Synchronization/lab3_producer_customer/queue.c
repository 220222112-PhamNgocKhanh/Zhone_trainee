/*
    This file i use the doubly linklist structure to implement simple function of queue datatype
    The way to run: Compile this file and doubly_link_list.c file together
*/

#include <stdio.h>
#include "queue.h"

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
