#include "slist.h"

#include <stdio.h> // fputs()
#include <string.h> 
#include <stdlib.h> 

// -------------------------------------------------------------------------
int print(void* ctx, const char* cs)
{
    printf("%s\n", cs);
    return 0;
}


// -------------------------------------------------------------------------
int main(int argc, char** argv)
{
    int rc = 0;
    struct slist* head = 0;

    head = slist_alloc();
    if (!head) {
        fprintf(stderr, "failed to allocate stringlist\n");
        goto end;
    }

    slist_push_back(head, "first");
    slist_push_back(head, "second");
    slist_push_back(head, "third");

    slist_foreach(head, print, 0);

    while (slist_is_not_empty(head)) {
        slist_pop_front(head);
    }

    // should be able to iterate over an empty list...
    slist_foreach(head, print, 0);

    slist_split(head, "this is a short list of strings", " ");
    slist_foreach(head, print, 0);

    // use the iteration feature to calculate size and total string length
    size_t length = 0;
    int count = 0;
    struct slist* node = head;
    while (head != (node = slist_next(node))) {
        count += 1;
        length += strlen(slist_string(node));
    }
    printf("The combined string length of all %d strings is %lu.\n", count, length); 

    char* buf = 0;
    rc = slist_join(head, &buf, " ");
    if (0 == rc) {
        printf("the joined list is: '%s'\n", buf);
        free(buf);
    }


    slist_free(head);


end: 
    return rc;
}
