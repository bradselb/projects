#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
        block->begin.row = row;
        block->begin.col = col;
        block->depth = depth;
    }
    return block;
}


// ---------------------------------------------------------------------------
void free_block(struct block* block)
{
    if (block) {
        memset(block, 0, sizeof *block);
        free(block);
    }
}


// ---------------------------------------------------------------------------
void block_set_end_pos(struct block* block, int row, int col)
{
    if (block) {
        block->end.row = row;
        block->end.col = col;
    }
}

// ---------------------------------------------------------------------------
int is_block_sane(struct block* block)
{
    int v = 0;
    v = (block && block->filename && !block->depth<0);
    return v;
}

// ---------------------------------------------------------------------------
int block_get_start_row(struct block* block)
{
    int row = 0;
    return row;
}

// ---------------------------------------------------------------------------
int block_get_start_col(struct block* block)
{
    int row = 0;
    return row;
}

// ---------------------------------------------------------------------------
int block_get_end_row(struct block* block)
{
    int row = 0;
    return row;
}

// ---------------------------------------------------------------------------
int block_get_end_col(struct block* block)
{
    int row = 0;
    return row;
}

// ---------------------------------------------------------------------------
int block_get_span(struct block* block)
{
    int row = 0;
    return row;
}

// ---------------------------------------------------------------------------
int block_get_depth(struct block* block)
{
    int row = 0;
    return row;
}


// ---------------------------------------------------------------------------
int block_printf(struct block* block)
{
    int rc = 0;
    int nr;
    int span;

    if (block) {
        nr = 0;
        span = block->end.row - block->begin.row;
        printf("file: %s\n", block->filename);
        printf("begin: (%d, %d)\n", block->begin.row, block->begin.col);
        printf("end:   (%d, %d)\n", block->end.row, block->end.col);
        printf("nr: %d, span: %d, depth: %d\n", nr, span, block->depth);
        printf("\n");
    }
    return rc;
}
