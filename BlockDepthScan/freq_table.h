#if !defined FREQ_TABLE_H
#define FREQ_TABLE_H

struct freqtab;

struct freqtab* creat_freqtab(void);
void destroy_freqtab(struct freqtab*);

int freqtab_add_observation(struct freqtab*, int value);



//int accumulate_depth_statistics(void* item, void* ctx);

#endif // !defined FREQ_TABLE_H


