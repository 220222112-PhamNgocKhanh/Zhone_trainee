/*
    Define the structure of singly linklist
*/
typedef struct link_list
{
    int data;
    struct link_list *next;
} link_list;

void insert_back(link_list **head, int data);
void insert_front(link_list **head, int data);
void delete_first_element(link_list **head, int data);
int get_and_remove_first_element(link_list **head);
void delete_all_element(link_list **head, int data);
void traverse(link_list *head);
void free_list(link_list **head);