#if !defined DYNDNSUPDATER_TIMEHELPERS_H
#define DYNDNSUPDATER_TIMEHELPERS_H

#include <stddef.h>

size_t getTimeNow(char* outbuf, int size);

int daysSince(const char* datetime);

#endif //!defined DYNDNSUPDATER_TIMEHELPERS_H

