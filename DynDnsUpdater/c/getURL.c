#include "getURL.h" 
#include "HttpRequest.h"
#include "inet.h"

#include <string.h> // memset(), memcpy()
#include <stdio.h> // fprintf()
#include <stdlib.h>  // malloc()
#include <unistd.h> // close()


int sendHttpRequestAndWaitForReply(const char* hostname, const char* resource, const char* agent, const char* auth, char* reply, int bufsize)
{
    int rc = 0;
    struct HttpRequest* request = 0;
    char* request_str = 0; // a buffer to hold the request string
    int request_bufsize = 4096;
    int sock = 0;
    int verbose = 0;

    int timeout_send = 20000; //milliseconds
    int timeout_wait = 20000; //milliseconds

    // allocate a buffer to hold the text of the HTTP request
    request_str = malloc(request_bufsize);
    if (!request_str) {
        fprintf(stderr, "%s(), request_str: %p\n", __FUNCTION__, request_str);
        goto exit;
    }

    if (reply) {
        memset(reply, 0, bufsize);
    }


    // formulate the request. 
    request = http_alloc_request(hostname, resource);
    if (!request) {
        fprintf(stderr, "http_alloc_request() failed.\n");
        goto exit;
    }
    if (agent) {
        rc = http_set_user_agent(request, agent);
    }
    if (auth) {
        rc = http_set_basic_auth(request, auth);
    }

    // ask the HTTP request object to output a plain text string representation.
    rc = http_to_string(request, request_str, request_bufsize);
    if (rc != 0) {
        fprintf(stderr, "%s(), http_to_string() returned: %d\n", __FUNCTION__, rc);
        goto exit;
    }

    // and reassure ourselves that it is right and proper. 
    if (verbose > 0) {
        printf("\n");
        printf("%s", request_str);
        fflush(stdout);
    }

    // connect to the web server. 
    sock = inet_connect_tcp(hostname, "http");
    if (sock < 0) {
        fprintf(stderr, "%s(), inet_connect_tcp() failed.\n", __FUNCTION__);
        goto exit;
    }

    // send the http request to the web server
    size_t bytecount = strlen(request_str); 
    ssize_t result;
    result = inet_send(sock, request_str, bytecount, timeout_send);
    if (result < 0) {
        fprintf(stderr, "%s(), inet_send() failed.\n", __FUNCTION__);
        goto exit;
    }
    
    if (verbose) {
        printf("expected to send %lu bytes.\n", bytecount);
        printf("actually sent %ld bytes.\n", result);
        fflush(stdout);
    }

    // and wait for the web server to reply
    result = inet_wait_reply(sock, reply, bufsize, timeout_wait);
    if (result < 0) {
        fprintf(stderr, "%s(), inet_wait_reply() failed.\n", __FUNCTION__);
        goto exit;
    }
    
    if (verbose){
        printf("received %ld bytes.\n\n", result);
        printf("%s\n", reply);
        fflush(stdout);
    }


exit:
    if (sock > 0) {
        close(sock);
    }

    if (request_str) {
        free(request_str);
    }

    http_free_request(request);

    return rc;
}


