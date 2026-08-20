/*
    This file i use the linklist structure to implement simple function of stack datatype
    The way to run: Compile this file and singly_link_list.c file together
*/

#include <stdio.h>
#include "singly_link_list.h"

typedef link_list stack;
/*
    Push the element to the top of stack
*/
void push(stack **head, int data)
{
    insert_front(head, data);
}

/*
    Get the element in the top of stack and remove it
*/
int pop(stack **head)
{
    return get_and_remove_first_element(head);
}

int main()
{
    stack *head;
    push(&head, 5);
    push(&head, 6);
    push(&head, 7);
    printf("%d", pop(&head));
    printf("%d", pop(&head));
}