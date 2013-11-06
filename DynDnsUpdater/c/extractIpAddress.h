#if !defined DYNDNSUPDATER_EXTRACTIPADDRESS_H
#define DYNDNSUPDATER_EXTRACTIPADDRESS_H

// find a string that looks like an ip address in the given text string.
// if found, return offsets to the start of the ip address string and
// to one past the end of the ip address string. These are offsets 
// from the begining of the user supplied text string

int extractIpAddress(const char* text, char* buf, int bufsize);

#endif //!defined DYNDNSUPDATER_EXTRACTIPADDRESS_H

