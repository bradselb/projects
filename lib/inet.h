#if !defined MY_INET_H
#define MY_INET_H

#include <stdlib.h> // ssize_t

int inet_connect_tcp(const char* hostname, const char* port);
int inet_connect_udp(const char* hostname, const char* port);
int inet_connect(const char* hostname, const char* port, int type);

// timeout is in milliseconds.
ssize_t inet_send(int sockfd, const void* buf, size_t bytecount, int timeout);
ssize_t inet_wait_reply(int sockfd, void* buf, size_t bufsize, int timeout);


// the rest of these are for use by servers.
int inet_bind(const char* name, const char* port);

int inet_listen(int sockfd, int backlog);

int inet_accept(int sockfd);


#endif // !defined MY_INET_H

