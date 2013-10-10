#include <stdlib.h>
#include <string.h> 

#include "list_of_pointers.h"


// ---------------------------------------------------------------------------
struct list
{
    struct list* next;
    struct list* prev;
    void* item;
};


// ---------------------------------------------------------------------------
static struct list* create_node(void* item)
{
    struct list* node;
    node = malloc(sizeof *node);
    if (node) {
        node->next = 0;
        node->prev = 0;
        node->item = item;
    }
    return node;
}


// ---------------------------------------------------------------------------
static void* free_node(struct list* node)
{
    void *item;

    item = 0;
    if (node) {
        item = node->item;
        node->next = 0;
        node->prev = 0;
        node->item = 0;
        free(node);
    }
    return item;
}


// ---------------------------------------------------------------------------
struct list* create_list(void)
{
    struct list* head;
    head = create_node(0);
    if (head) {
        head->next = head;
        head->prev = head;
    }
    return head;
}


// ---------------------------------------------------------------------------
// this function frees each node on the list and, if a destructor function
// is supplied, it will call the destructor on the pointer contained at each
// node as it is destroyed. Of course, the destructor function need not 
// be a destructor. It might simply display a message. 
// This allows one to push items onto several lists and only destroy them once.
// Obviously, it also requires care.
void destroy_list(struct list* head, void (*destructor) (void*))
{
    void* item;

    while (!is_list_empty(head)) {
        item = pop_front(head);
        if (destructor) destructor(item);
    }
    free_node(head);
}


// ---------------------------------------------------------------------------
int is_list_empty(struct list* head)
{
    int is_empty = 1;
    if (head && head != head->next) {
        is_empty = 0;
    }
    return is_empty;
}

// ---------------------------------------------------------------------------
void push_front(struct list* head, void* item)
{
    struct list* node;

    if (head && item && (node = create_node(item))) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }
}

// ---------------------------------------------------------------------------
void push_back(struct list* head, void* item)
{
    struct list* node;

    if (head && item && (node = create_node(item))) {
        node->next = head;
        node->prev = head->prev;
        head->prev->next = node;
        head->prev = node;
    }
}


// ---------------------------------------------------------------------------
// If there exists at least one item in the list, returns the item in the 
// first node and frees the node. If the list is empty, returns zero.
void* pop_front(struct list* head)
{
    void *item;
    struct list* node;

    item = 0;
    if (head) {
        node = head->next;
        if (node != head) {
            node->next->prev = head;
            head->next = node->next;
            item = free_node(node);
        }
    }
    return item;
}


// ---------------------------------------------------------------------------
void* peek_front(struct list* head)
{
    void *item;

    item = 0;
    if (head) {
        item = head->next->item;
    }
    return item;
}


// ---------------------------------------------------------------------------
// for each item int eh list, call the function (if one is supplied). 
// returns the number of elements on the list. Note that teven if no
// function is supplied, the list is always traversed and the count 
// retuned. 
int foreach_item_call_fctn(struct list* head, void (*fctn)(void* item, void* ctx), void* ctx)
{
    int count = 0;
    struct list* node;

    if (head) {
        node = head->next;
        while (node != head) {
            //printf("head: %p, node: %p\n", head, node);
            if (fctn) {
                fctn(node->item, ctx);
            }
            ++count;
            node = node->next;
        }
    }

    return count;
}

