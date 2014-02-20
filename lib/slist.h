#if !defined(STRINGLIST_H)
#define STRINGLIST_H

// a doubly linked list of null terminated strings. 
struct slist;

// allocate the head node of the list. call push function to add nodes.
struct slist* slist_alloc();
// free the entire list - including all of the strings.
// failure to call  this function results in memory leaks!
void slist_free(struct slist* head);

// add a string to the front or back of the list.
int slist_push_front(struct slist* head, const char* s);
int slist_push_back(struct slist* head, const char* s);

// pop a node off the list.
int slist_pop_front(struct slist* head);
int slist_pop_back(struct slist* head);

// get the string contained in the node
const char* slist_string(struct slist* node);

// iterate over the the list.
struct slist* slist_next(struct slist* node);
struct slist* slist_prev(struct slist* node);

// for each string in the list, call the function and pass some
// context object and the string as the arguments to the function.
int slist_foreach(struct slist* head, int (*fctn)(void* ctx, const char* cs), void* ctx);


// split the delimited string into a list of strings.
// add the strings to the end of the list.
// return the number of strings added to the list.
int slist_split(struct slist* head, const char* cs, const char* delims);

// join the strings in the list forming one big string. 
// each string is separated by the  delim string. 
// on success, this function allocates a buffer from the heap
// and returns zero. it returns non-zero on failure. 
// the buffer allcated by the function MUST be freed by the caller!
int slist_join(struct slist* head, char** buf, const char* delim);


#endif //!defined(STRINGLIST_H)
