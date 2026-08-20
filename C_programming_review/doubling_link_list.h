typedef struct dlink_list
{
    int data;
    struct dlink_list *next;
    struct dlink_list *prev;
} dlink_list;



void insert_head(dlink_list **head, dlink_list **tail, int data);
void insert_tail(dlink_list **head, dlink_list **tail, int data);
int get_and_remove_head(dlink_list **head, dlink_list **tail);
int get_and_remove_tail(dlink_list **head, dlink_list **tail);
void traverse_head_to_tail(dlink_list *head, dlink_list *tail);
void traverse_tail_to_head(dlink_list *head, dlink_list *tail);