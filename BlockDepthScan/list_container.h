#if !defined LIST_CONTAINER_H
#define LIST_CONTAINER_H

// ---------------------------------------------------------------------------
// doubly linked circular list of pointers.
// the container does make a copy of items pushed onto the list and in general
// does NOT own the pointers -- client code owns the items in the list.  

struct list_node;

struct list_node* allocate_list(void);
void free_list(struct list_node* head);

int is_list_empty(struct list_node* head);

void push_front(struct list_node* head, void* item);
void push_back(struct list_node* head, void* item);

void* pop_front(struct list_node* head);
//void* pop_back(struct list_node* head);

void* peek_front(struct list_node* head);

#endif //!defined LIST_CONTAINER_H

