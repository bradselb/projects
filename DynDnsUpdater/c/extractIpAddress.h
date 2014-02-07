#if !defined DYNDNSUPDATER_EXTRACTIPADDRESS_H
#define DYNDNSUPDATER_EXTRACTIPADDRESS_H

// find a string that looks like an ipv4 address in the given text string.
// if found, copy the ip address from the text to the buffer, buf.
// returns zero if successful, non-zero otherwise.

int extractIpAddress(const char* text, char* buf, int bufsize);

#endif //!defined DYNDNSUPDATER_EXTRACTIPADDRESS_H

