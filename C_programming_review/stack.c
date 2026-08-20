#include <stdio.h>
#include "singly_link_list.h"

typedef link_list stack;

void push(stack **head, int data)
{
    insert_front(head, data);
}

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