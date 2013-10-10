#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "list_of_pointers.h"
#include "block.h"

// -------------------------------------------------------------------------
// use the ascii code instead of the charater so that when I run this program 
// on this file, it does not count the characters as blocks. bad smell?
#define LEFT_CURLY_BRACE 0x7b
#define RIGHT_CURLY_BRACE 0x7d

// -------------------------------------------------------------------------
void show_block(void* item, void* unused)
{
    struct block* block = (struct block*)item;
    print_block(block);
}

// -------------------------------------------------------------------------
void free_block_wrapper(void* item)
{
    struct block* block = (struct block*)item;
    free_block(block);
}

// -------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    FILE* file;
    int line, column;
    int depth;
    int c;

    struct block* block;
    struct list* stack = 0;
    struct list* block_list = 0;
    

    for (int i=1; i<argc; ++i) {
        file = fopen(argv[i], "r");
        if (file) {
            line = 1;
            column = 0;
            depth = 0;
            stack = create_list();
            block_list = create_list();
            printf("file: %s\n", argv[i]);
            while (EOF != (c = fgetc(file))) {
                ++column;
                if ('\n' == c) {
                    ++line;
                    column = 0;
                } else if (LEFT_CURLY_BRACE == c) {
                    // start of a new block
                    block = allocate_block(argv[i], line, column, depth);
                    push_front(stack, block);
                    ++depth;
                } else if (RIGHT_CURLY_BRACE == c) {
                    // close current block
                    block = pop_front(stack);
                    set_block_end_pos(block, line, column);
                    push_back(block_list, block);
                    --depth;
                }
            }
            fclose(file);

            // show all the blocks in the list
            foreach_item_call_fctn(block_list, show_block, 0);
            fflush(stdout);

            destroy_list(block_list, free_block_wrapper);
            destroy_list(stack, free_block_wrapper);
        } else {
            fprintf(stderr, "'%s', %s\n", argv[i], strerror(errno));
        }
    }


    return 0;
}

