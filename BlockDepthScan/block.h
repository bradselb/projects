#if !defined BLOCK_H
#define BLOCK_H

struct block;
struct container;

struct block* allocate_block(const char* filename, int row, int col, int depth);
void free_block(struct block*);

int block_set_end_pos(struct block*, int row, int col);

int block_get_start_row(struct block*);
int block_get_start_col(struct block*);
int block_get_end_row(struct block*);
int block_get_end_col(struct block*);
int block_get_span(struct block*);
int block_get_depth(struct block*);


// blocks can be added to containers
struct container* allocate_container(void);
void free_container(struct container* container);

int push_front(struct container*, struct block* block);
int push_back(struct container*, struct block* block);

struct block* pop_front(struct containeir*);
struct block* pop_back(struct container*);



#endif // !defined BLOCK_H

