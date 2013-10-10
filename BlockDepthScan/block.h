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

void set_block_end_pos(struct block*, int row, int col);

int is_block_sane(struct block* block);

int get_block_start_row(struct block*);
int get_block_start_col(struct block*);
int get_block_end_row(struct block*);
int get_block_end_col(struct block*);
int get_block_span(struct block*);
int get_block_depth(struct block*);

int cmp_block_start_line(const struct block* ,const struct block*); 
int cmp_block_end_line(const struct block* ,const struct block*); 
int cmp_block_depth(const struct block* ,const struct block*); 
int cmp_block_span(const struct block* ,const struct block*); 

int print_block(struct block* block);

#endif // !defined BLOCK_H

