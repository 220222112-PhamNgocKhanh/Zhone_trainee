#include <stdio.h>
#include <stdlib.h>

typedef struct link_list
{
    int data;
    struct link_list *next;
} link_list;

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

void free_list(link_list **head) {
    while(head != NULL) {
        link_list *temp = (*head);
        (*head) = (*head)->next;
        free(temp);
    }
}


int main()
{
    link_list *head = NULL;
    insert_back(&head, 5);
    insert_back(&head, 5);
    insert_back(&head, 5);
    insert_back(&head, 5);
    insert_back(&head, 5);
    delete_all_element(&head, 5);
    traverse(head);
    return 0;
}