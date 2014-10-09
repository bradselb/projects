#if !defined MY_INET_H
#define MY_INET_H

int inet_connect_tcp(const char* hostname, const char* port);
int inet_connect_udp(const char* hostname, const char* port);
int inet_connect(const char* hostname, const char* port, int type);

// the rest of these are for use by servers.
int inet_bind(const char* name, const char* port);

int inet_listen(int sockfd, int backlog);

int inet_accept(int sockfd);


#endif // !defined MY_INET_H

