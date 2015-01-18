#if !defined GETURL_H
#define GETURL_H

// this function does a complete HTTP transaction.
// for example, if you wanted to get the URL...
// http://http://www.dot.ca.gov/cgi-bin/roadscell.cgi?roadnumber=108
// then, 
//   1. allocate a buffer large enough to hold the WHOLE http response 
//   2. and call this function with
//      hostname = "www.dot.ca.gov",
//      resource = "/roadscell.cgi?roadnumber=108"
//      agent = 0, auth = 0, and
//      reply = buffer allocated in #1 and bufsize = associate buffer size.
//
// returns non-zero on failure, otherwise returns zero.
int sendHttpRequestAndWaitForReply(const char* hostname, const char* resource, const char* agent, const char* auth, char* reply, int bufsize);

#endif //!defined GETURL_H
