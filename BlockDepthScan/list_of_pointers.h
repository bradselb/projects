#if !defined LIST_OF_POINTERS_H
#define LIST_OF_POINTERS_H

// ---------------------------------------------------------------------------
// doubly linked circular list of pointers.
// the container does make a copy of items pushed onto the list and in general
// does NOT own the pointers -- client code owns the items in the list.  

struct list_node;

struct list_node* allocate_list(void);
void free_list(struct list_node* head, void (*destructor) (void* item));

int is_list_empty(struct list_node* head);

void push_front(struct list_node* head, void* item);
void push_back(struct list_node* head, void* item);

void* pop_front(struct list_node* head);
//void* pop_back(struct list_node* head);

void* peek_front(struct list_node* head);
int foreach_item_call_fctn(struct list_node* head, void (*fctn)(void* item, void* ctx), void* ctx);

#endif // !defined LIST_OF_POINTERS_H

