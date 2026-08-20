#include<stdio.h>
#include "doubling_link_list.h"

typedef dlink_list queue;


void push(queue **head,queue **tail, int data) {
    insert_head(head,tail,data);
}

int pop(queue **head,queue **tail) {
    return get_and_remove_tail(head,tail);
}

int main() {
    queue *head = NULL;
    queue *tail = NULL;
    push(&head,&tail,6);
    push(&head,&tail,7);
    push(&head,&tail,8);

    printf("%d\n",pop(&head,&tail));
    printf("%d\n",pop(&head,&tail));
    printf("%d\n",pop(&head,&tail));
}