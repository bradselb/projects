#if !defined BLOCK_H
#define BLOCK_H

struct block;

struct block* block_allocate(const char* filename, int row, int col, int depth);
void block_free(struct block*);

int block_set_end_pos(struct block*, int row, int col);

int block_set_start_pos(struct block*, int row, int col);
int block_set_end_pos(struct block*, int row, int col);


#endif // !defined BLOCK_H

