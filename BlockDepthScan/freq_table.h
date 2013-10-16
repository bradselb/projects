#if !defined FREQ_TABLE_H
#define FREQ_TABLE_H

#include "list_of_pointers.h"

// a freq table is simply a container of freq table entries. 
struct freqtab;

struct freqtab* create_freqtab(void);
void destroy_freqtab(struct freqtab*);

int freqtab_add_observation(struct freqtab*, int value);


#endif // !defined FREQ_TABLE_H

