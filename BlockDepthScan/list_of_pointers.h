#if !defined LIST_OF_POINTERS_H
#define LIST_OF_POINTERS_H

// ---------------------------------------------------------------------------
// doubly linked circular list of pointers.
// the container does not make a copy of items pushed onto the list.
// the list object does NOT own the pointers.
// client code owns the items in the list.
// clients are responisble for managing the lifetime of the items in the list.
// that said however, if the client supplies a destructor function to the 
// list destructor, then the list destructor will call the supplied 
// item destructor for each item in the list

struct list;

struct list* create_list(void);
void destroy_list(struct list* head, void (*destructor) (void* item));

int is_list_empty(struct list* head);

void add_item_ascending(struct list* head, void* item, int (*cmp)(const void* item1, const void* item2));
void add_item_descending(struct list* head, void* item, int (*cmp)(const void* item1, const void* item2));
struct list* find_item(struct list* head, void* item, int (*cmp)(const void* , const void*));

void push_front(struct list* head, void* item);
void push_back(struct list* head, void* item);

void* pop_front(struct list* head);
//void* pop_back(struct list* head);

void* peek_front(struct list* head);
int foreach_item_call_fctn(struct list* head, void (*fctn)(void* item, void* ctx), void* ctx);

#endif // !defined LIST_OF_POINTERS_H

