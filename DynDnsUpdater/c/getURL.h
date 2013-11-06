#if !defined GETURL_H
#define GETURL_H

// get URL 
// copies content of page at URL to user supplied buffer. 
// caller owns the buffer. 

int getURL(char* buf, int bufsize, const char* url, const char* userAgent, const char* user, const char* pass );

#endif //!defined GETURL_H
