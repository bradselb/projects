#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "list_of_pointers.h"
#include "block.h"

// 
void show_block(void* item)
{
    struct block* block = (struct block*)item;
    block_printf(block);
}

int main(int argc, char* argv[])
{
    FILE* file;
    int line, column;
    int depth;
    int c;

    struct block* block;
    struct list_node* stack = 0;
    struct list_node* block_list = 0;
    

    for (int i=1; i<argc; ++i) {
        file = fopen(argv[i], "r");
        if (file) {
            line = 1;
            column = 0;
            depth = 0;
            stack = allocate_list();
            block_list = allocate_list();
            printf("file: %s\n", argv[i]);
            while (EOF != (c = fgetc(file))) {
                ++column;
                if ('\n' == c) {
                    ++line;
                    column = 0;
                } else if ('{' == c) {
                    // start of a new block
                    ++depth;
                    block = allocate_block(argv[i], line, column, depth);
                    push_front(stack, block);
                    printf("start, line: %d, col: %d, depth: %d\n", line, column, depth);
                } else if ('}' == c) {
                    block = pop_front(stack);
                    block_set_end_pos(block, line, column);
                    push_back(block_list, block);
                    printf("  end, line: %d, col: %d, depth: %d\n", line, column, depth);
                    --depth;
                }
            }
            fclose(file);

            call_fctn_foreach_item(block_list, show_block);
            fflush(stdout);

            free_list(block_list, free_block);
            free_list(stack, free_block);
        } else {
            fprintf(stderr, "'%s', %s\n", argv[i], strerror(errno));
        }
    }


    return 0;
}

