#include <stdio.h>
#include <stdlib.h>
#include "doubling_link_list.h"

void insert_head(dlink_list **head, dlink_list **tail, int data)
{
    if ((*head) == NULL)
    {
        dlink_list *temp = malloc(sizeof(dlink_list));
        temp->data = data;
        temp->next = NULL;
        temp->prev = NULL;
        (*head) = temp;
        (*tail) = temp;
    }
    else
    {
        dlink_list *temp = malloc(sizeof(dlink_list));
        temp->data = data;
        temp->next = (*head);
        temp->prev = NULL;
        (*head)->prev = temp;
        (*head) = temp;
    }
}

void insert_tail(dlink_list **head, dlink_list **tail, int data)
{
    if ((*head) == NULL)
    {
        dlink_list *temp = malloc(sizeof(dlink_list));
        temp->data = data;
        temp->next = NULL;
        temp->prev = NULL;
        (*head) = temp;
        (*tail) = temp;
    }
    else
    {
        dlink_list *temp = malloc(sizeof(dlink_list));
        temp->data = data;
        temp->next = NULL;
        temp->prev = (*tail);
        (*tail)->next = temp;
        (*tail) = temp;
    }
}

int get_and_remove_head(dlink_list **head, dlink_list **tail)
{
    if ((*head) != NULL)
    {

        if ((*head) == (*tail))
        {
            int data = (*head)->data;
            free((*head));
            (*head) = NULL;
            (*tail) = NULL;
            return data;
        }
        else
        {
            dlink_list *temp = (*head);
            int data = temp->data;
            (*head) = (*head)->next;
            (*head)->prev = NULL;
            free(temp);
            return data;
        }
    }

    return 0;
}

int get_and_remove_tail(dlink_list **head, dlink_list **tail)
{
    if ((*head) != NULL)
    {

        if ((*head) == (*tail))
        {
            int data = (*head)->data;
            free((*head));
            (*head) = NULL;
            (*tail) = NULL;
            return data;
        }
        else
        {
            dlink_list *temp = (*tail);
            int data = temp->data;
            (*tail) = (*tail)->prev;
            (*tail)->next = NULL;
            free(temp);
            return data;
        }
    }

    return 0;
}

void traverse_head_to_tail(dlink_list *head, dlink_list *tail)
{
    if (head == NULL)
    {
        printf("the list is empty");
    }
    else
    {
        while (head != NULL)
        {
            printf("%d ", head->data);
            head = head->next;
        }
    }
}

void traverse_tail_to_head(dlink_list *head, dlink_list *tail)
{
    if (head == NULL)
    {
        printf("the list is empty");
    }
    else
    {
        while (tail != NULL)
        {
            printf("%d ", tail->data);
            tail = tail->prev;
        }
    }
}

// int main()
// {

//     dlink_list *head = NULL;
//     dlink_list *tail = NULL;

//     insert_head(&head, &tail, 5);
//     insert_head(&head, &tail, 4);
//     insert_head(&head, &tail, 3);
//     insert_head(&head, &tail, 2);
//     traverse_head_to_tail(head, tail);
//     printf("\n");
//     traverse_tail_to_head(head, tail);
// }