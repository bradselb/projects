
#define _POSIX_SOURCE 1

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>

#include "inet.h"


// --------------------------------------------------------------------------
// a bunch of functions that mostly wrap the usual and customary sockets
// stuff. I have taken considerable inspiration from the example code 
// in the most excellent book... 
// Michael Kerrisk, "The Linux Programming Interface", (C) 2010


// --------------------------------------------------------------------------
// a client uses this fctn to establish a TCP/Ip connection to a server. 
int inet_connect_tcp(const char* name, const char* port)
{
    return inet_connect(name, port, SOCK_STREAM);
}


// --------------------------------------------------------------------------
// a client uses this fctn to establish a UDP/Ip connection to a server. 
int inet_connect_udp(const char* name, const char* port)
{
    return inet_connect(name, port, SOCK_DGRAM);
}


// --------------------------------------------------------------------------
// given a string representation of a host address and port number,
// use dns to lookup the ipv4 address, fill in the socket address structure
// with the host address, open a socket and connect to the server.
// the type argument should be one of SOCK_STREAM or SOCK_DGRAM
// returns:
//    the connected socket fd on success or -1 if not sucessful.
// note: 
//    caller is responsible to close the socket.
int inet_connect(const char* name, const char* port, int type)
{
    int sock;
    struct addrinfo hints;
    struct addrinfo* ai;
    int rc;
 
    sock = -1; // fail by default.

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = type;
    // hints.ai_flags = AI_NUMERICSERV;
    // hints.ai_protocol = 0;
    // hints.ai_canonname = NULL;
    // hints.ai_addr = NULL;
    // hints.ai_next = NULL;

    if (!name || !port) {
        fprintf(stderr, "(%s:%d) %s(), invalid args.\n", __FILE__, __LINE__, __FUNCTION__);
        goto out;
    }

    ai = 0;
    rc = getaddrinfo(name, port, &hints, &ai);
    if (rc != 0) {
        fprintf(stderr, "(%s:%d) %s(), getaddrinfo() returned: %d,  %s\n", __FILE__, __LINE__, __FUNCTION__, rc, gai_strerror(rc));
        goto out;
    }

    struct addrinfo* iter;
    for (iter=ai; iter!=NULL; iter=iter->ai_next) {
        sock = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
        if (sock < 0) continue;

        rc = connect(sock, iter->ai_addr, iter->ai_addrlen); 
        if (rc < 0) {
            // failed to connect
            fprintf(stderr, "(%s:%d) %s(), connect() returned: %d\n", __FILE__, __LINE__, __FUNCTION__, rc);
            close(sock);
            sock = -1;
        } else {
            // success!
            break;
        }
    }
    freeaddrinfo(ai);

out:
    return sock;
}


// --------------------------------------------------------------------------
// wait for socket to be ready for write and then write the whole buffer to
// the socket. May need to revisit this if the whole buffer is not going out
// at one. Might need to do the write in a loop.
// On success, this function returns the number of bytes written. On failure,
// the function returns -1 and errno is set appropriately (hmmm,maybe?) 
ssize_t inet_send(int sockfd, const void* buf, size_t bytes_to_send, int timeout)
{
    ssize_t total_bytes_sent;
    ssize_t bytes_sent;
    const char* p = (const char*)buf; // where are we in the buffer?
    //const char* end = (const char*)(buf + bytes_to_send);
    struct pollfd ps;
    int pollrc;

    ps.fd = sockfd;
    ps.events = POLLOUT;
    ps.revents = 0;

    total_bytes_sent = 0;
    while (total_bytes_sent < bytes_to_send) {

        pollrc = poll(&ps, 1, timeout);

        if (pollrc > 0) {
            // ready for write! 
            bytes_sent = write(sockfd, p+total_bytes_sent, bytes_to_send-total_bytes_sent);

            if (bytes_sent > 0) {
                total_bytes_sent += bytes_sent;
            } else if (0 == bytes_sent) {
                // try again?
                fprintf(stderr, "(%s:%d) %s(), write() returned zero.\n", __FILE__, __LINE__, __FUNCTION__);
                continue;
            } else { // if bytes_sent < 0
                // an error during write?
                fprintf(stderr, "(%s:%d) %s(), write() returned: %ld [%s]\n", __FILE__, __LINE__, __FUNCTION__ , bytes_sent, strerror(errno));
                break;
            }

        } else if (0 == pollrc) {
            // timedout
            fprintf(stderr, "(%s:%d) %s(), poll() timeout [%s]\n", __FILE__, __LINE__, __FUNCTION__ , strerror(errno));
            break;
        } else {
            // a polling error.
            fprintf(stderr, "(%s:%d) %s(), poll() returned: %d [%s]\n", __FILE__, __LINE__, __FUNCTION__ , pollrc, strerror(errno));
            break;
        }
    }

    
    return total_bytes_sent;
}


// --------------------------------------------------------------------------
// waits up to timout milliseconds for the socket to be readable and reads
// upto bufsize bytes into the user supplied buffer. 
// On success, this function returns the number of bytes written. On failure,
// the function returns -1 and errno is set appropriately (hmmm,maybe?) 
ssize_t inet_wait_reply(int sockfd, void* buf, size_t bufsize, int timeout)
{
    ssize_t total_bytes_read;
    struct pollfd ps;
    int done;

    ps.fd = sockfd;
    ps.events = POLLIN;
    ps.revents = 0;
    
    done = 0;
    total_bytes_read = 0;
    while (!done) {
        int pollrc;

        // see if the server has replied.
        pollrc = poll(&ps, 1, timeout);

        if (pollrc > 0) {
            // ready!
            total_bytes_read += read(sockfd, buf, bufsize);
            // not really sure how this is going to work yet. 
            // just bail out of the loop now and hope we get everything.
            done = 1; 

        } else if (0 == pollrc) {
            // timedout.
            fprintf(stderr, "(%s:%d) %s(), poll() timeout [%s]\n", __FILE__, __LINE__, __FUNCTION__ , strerror(errno));
            break;

        } else if (pollrc < 0) {
            // an error.
            fprintf(stderr, "(%s:%d) %s(), poll() returned: %d [%s]\n", __FILE__, __LINE__, __FUNCTION__ , pollrc, strerror(errno));
            total_bytes_read = pollrc;
            break;
        }
    }

    return total_bytes_read;
}


// --------------------------------------------------------------------------
// a server process uses this function to bind it's service to a given 
// ip address and port number. 
// The name parameter specifies the hostname or ip address to use and the 
// port parameter is used to specify which port or service name to bind to. 
// so, for example, we can say things like...
//    fd = inet_bind("localhost", "http");
//    fd = inet_bind("192.168.1.201", "16875");
// you can call this function with one of the arguments set to "0" to tell
// and the function will still do the right thing. 
// So for example, if you say...
//    fd = inet_bind("0", "16875");
// it means that you will take incomming connections on any suitable interface
// I guess, this means that if your machine is multi-homed, it can serve on both
// interfaces? 
// you can say someting like: 
//    fd = inet_bind("localhost", "0");
// and the function to choose an ephemeral port for your service. You can 
// use netstat --listen --tcp    or, simply netstat -lt to discover what port
// was picked for you (TODO: do this programmatically)
//
// this function takes the hostname / ip address and port number or service 
// and returns a socket file descriptor if successful. it returns negative
// values otherwise. 
int inet_bind(const char* name, const char* port)
{
    int sock;
    struct addrinfo hints;
    struct addrinfo* ai;
    int rc;
 
    sock = -1; // fail by default.

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    // hints.ai_flags = AI_NUMERICHOST | AI_PASSIVE;
    hints.ai_flags = AI_PASSIVE;
    // hints.ai_protocol = 0;
    // hints.ai_canonname = NULL;
    // hints.ai_addr = NULL;
    // hints.ai_next = NULL;

    if (!name || !port) {
        fprintf(stderr, "(%s:%d) %s(), invalid args.\n", __FILE__, __LINE__, __FUNCTION__);
        sock = -1;
        goto out;
    }


    ai = 0;
    rc = getaddrinfo(name, port, &hints, &ai);
    if (rc != 0) {
        fprintf(stderr, "(%s:%d) %s(), getaddrinfo() returned: %d,  %s\n", __FILE__, __LINE__, __FUNCTION__, rc, gai_strerror(rc));
        goto out;
    }

    struct addrinfo* iter;
    for (iter=ai; iter!=NULL; iter=iter->ai_next) {
        sock = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol);
        if (sock < 0) continue;

        rc = bind(sock, iter->ai_addr, iter->ai_addrlen); 
        if (rc < 0) {
            // failed to connect
            fprintf(stderr, "(%s:%d) %s(), bind() returned: %d\n", __FILE__, __LINE__, __FUNCTION__, rc);
            close(sock);
            sock = -1;
        } else {
            // success!
            struct sockaddr_in* sa = (struct sockaddr_in*)iter->ai_addr;
            const char* host_name = 0;
            int port_nr = 0;
            char* buf;
            int bufsize = 1024;
            buf = malloc(bufsize);
            if (buf) {
                memset(buf, 0, bufsize);
                host_name = inet_ntop(iter->ai_family, &sa->sin_addr, buf, bufsize);
                port_nr = ntohs(sa->sin_port);
                fprintf(stderr, "bind() success (%s:%d)\n", host_name, port_nr);
                free(buf);
            }
            break;
        }
    }

    freeaddrinfo(ai);

out:
     return sock;
}



// --------------------------------------------------------------------------
int inet_listen(int sockfd, int backlog)
{
    int rc; 
    rc = listen(sockfd, backlog);
    return rc;
}


// --------------------------------------------------------------------------
int inet_accept(int sockfd)
{
    int peer;
    struct sockaddr_in peer_sa;
    socklen_t peer_sa_size = sizeof(struct sockaddr_in);

    peer = accept(sockfd, (struct sockaddr*)&peer_sa, &peer_sa_size);
    if (peer < 0) {
        //fprintf(stderr, "(%s:%d) %s(), accept() returned: %d\n", __FILE__, __LINE__, __FUNCTION__, peer);
    }

    return peer;
}
