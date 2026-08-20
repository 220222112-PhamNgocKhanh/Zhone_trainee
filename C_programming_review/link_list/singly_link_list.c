#include <stdio.h>
#include <stdlib.h>
#include "singly_link_list.h"

/*
    Inserts a new node with the specified data at the end of the list
 */
void insert_back(link_list **head, int data)
{
    if ((*head) == NULL)
    {
        link_list *temp = malloc(sizeof(link_list));
        temp->next = NULL;
        temp->data = data;
        (*head) = temp;
        return;
    }
    else
    {
        link_list *cur = *head;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }

        link_list *temp = malloc(sizeof(link_list));
        temp->next = NULL;
        temp->data = data;
        cur->next = temp;
    }
}

/*
    Inserts a new node with the specified data at the beginning of the list
 */
void insert_front(link_list **head, int data)
{
    if ((*head) == NULL)
    {
        link_list *temp = malloc(sizeof(link_list));
        temp->next = NULL;
        temp->data = data;
        (*head) = temp;
        return;
    }
    else
    {
        link_list *temp = malloc(sizeof(link_list));
        temp->next = (*head);
        temp->data = data;
        (*head) = temp;
    }
}

/*
    Deletes the first node found that contains the specified data
 */
void delete_first_element(link_list **head, int data)
{

    if (*head == NULL)
    {
        return;
    }
    if ((*head)->data == data)
    {
        link_list *temp = (*head);
        (*head) = (*head)->next;
        free(temp);
        return;
    }
    link_list *cur = *head;
    link_list *temp = (*head)->next;
    while (temp != NULL)
    {
        if (temp->data == data)
        {
            cur->next = temp->next;
            free(temp);
            temp = cur->next;
            return;
        }
        else
        {
            cur = cur->next;
            temp = temp->next;
        }
    }
}

/*
    Removes the first node from the list and returns its data
 */
int get_and_remove_first_element(link_list **head)
{
    if ((*head == NULL))
    {
    }
    else
    {
        link_list *temp = *head;
        (*head) = (*head)->next;
        int data = temp->data;
        free(temp);
        return data;
    }
}

/*
    Deletes all nodes in the list that match the specified data
 */
void delete_all_element(link_list **head, int data)
{
    if (*head == NULL)
    {
        return;
    }
    while ((*head) != NULL && (*head)->data == data)
    {
        link_list *temp = (*head);
        (*head) = (*head)->next;
        free(temp);
    }
    if ((*head) != NULL)
    {
        link_list *cur = *head;
        link_list *temp = (*head)->next;
        while (temp != NULL)
        {
            if (temp->data == data)
            {
                cur->next = temp->next;
                free(temp);
                temp = cur->next;
            }
            else
            {
                cur = cur->next;
                temp = temp->next;
            }
        }
    }
}

/*
    Traverses and prints all elements in the linked list
 */
void traverse(link_list *head)
{
    if (head == NULL)
    {
        printf("Link List is empty");
    }
    else
    {
        while (head != NULL)
        {
            printf("%d  ", head->data);
            head = head->next;
        }
    }
}

/*
    Frees all allocated memory for the entire linked list
 */
void free_list(link_list **head)
{
    while (head != NULL)
    {
        link_list *temp = (*head);
        (*head) = (*head)->next;
        free(temp);
    }
}

// int main()
// {
//     link_list *head = NULL;

//     insert_back(&head, 5);
//     // insert_back(&head, 4);
//     // insert_back(&head, 3);
//     // delete_all_element(&head, 5);
//     traverse(head);
//     printf("\ntest remove first element\n");
//     printf("%d\n", get_and_remove_first_element(&head));
//     traverse(head);
//     return 0;
// }