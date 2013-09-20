#include <stdlib.h>
#include <string.h> 

#include "list_container.h"


// ---------------------------------------------------------------------------
struct list_node
{
    struct list_node* next;
    struct list_node* prev;
    void* item;
}


// ---------------------------------------------------------------------------
static struct list_node* create_node(void* item)
{
    struct list_node* node;
    node = malloc(sizeof *node);
    if (node) {
        node->next = 0;
        node->prev = 0;
        node->item = item;
    }
    return node;
}


// ---------------------------------------------------------------------------
static void* free_node(struct list_node* node)
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
struct list_node* allocate_list(void)
{
    struct list_node* head;
    head = create_node(0);
    if (head) {
        head->next = head;
        head->prev = head;
    }
    return head;
}


// ---------------------------------------------------------------------------
void free_list(struct list_node* head)
{
    void* item;

    while (!is_list_empty(head)) {
        item = pop_front(head);
    }
    item = free_node(head);
}


// ---------------------------------------------------------------------------
int is_list_empty(struct list_node* head)
{
    int is_empty = 1;
    if (head && head != head->next) {
        is_empty = 0;
    }
    return is_empty;
}

// ---------------------------------------------------------------------------
void push_front(struct list_node* head, void* item)
{
    struct list_node* node;

    if (head && item && (node = create_node(item))) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }
}

// ---------------------------------------------------------------------------
void push_back(struct list_node* head, void* item)
{
    struct list_node* node;

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
void* pop_front(struct list_node* head)
{
    void *item;
    struct list_node* node;

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
void* peek_front(struct list_node* head)
{
    void *item;

    item = 0;
    if (head) {
        item = head->next->item;
    }
    return item;
}

