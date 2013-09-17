#include "block.h"

// ---------------------------------------------------------------------------
struct position
{
    int row;
    int col;
};


struct block
{
    const char* filename;
    struct position begin;
    struct position end;
    int depth;
    struct block* next;
    struct block* prev;
};


// this is the dummy head node of a doubly linked list.
struct container 
{
    struct block* next;
    struct block* prev;
};


// ---------------------------------------------------------------------------
const char* UNKNOWN_FILENAME = "unknown";

// ---------------------------------------------------------------------------
struct block* allocate_block(const char* filename, int row, int col, int depth)
{
    struct block* block;

    block = malloc(sizeof(struct block));
    if (!block) {
        // a serious error.
    } else {
        if (filename) {
            block->filename = filename;
        } else {
            block->filename = UNKNOWN_FILENAME;
        }
        block->start_pos.row = row;
        block->starrt_pos.col = col;
        block->depth = depth;
    }
    return block;
}


// ---------------------------------------------------------------------------
void free_block(struct block* block)
{
    if (block) {
        free(block);
    }
}

// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
int block_set_end_pos(struct block* block, int row, int col)
{
}

