#if !defined BLOCK_H
#define BLOCK_H

// ---------------------------------------------------------------------------
// a block represents a block of code. Each block has a start and end position
// and exists at a nesting depth. The outer most scope in a file (global scope)
// is defined to have depth==0. Each successive nested block has an 
// incrementally larger depth than its enclosing block. 
struct block;

// ---------------------------------------------------------------------------
struct block* allocate_block(const char* filename, int row, int col, int depth);
void free_block(struct block*);

void block_set_end_pos(struct block*, int row, int col);

int is_block_sane(struct block* block);

int block_get_start_row(struct block*);
int block_get_start_col(struct block*);
int block_get_end_row(struct block*);
int block_get_end_col(struct block*);
int block_get_span(struct block*);
int block_get_depth(struct block*);

int block_printf(struct block* block);

#endif // !defined BLOCK_H

