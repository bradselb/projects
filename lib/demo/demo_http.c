// gcc -Wall -std=gnu99 -o demo_http -I.. demo_http.c ../HttpRequest.c ../inet.c ../hexdump.c

#include "HttpRequest.h"
#include "hexdump.h"
#include "inet.h"

#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h> // close()

int main(int argc, char* argv[])
{
    int rc;
    struct HttpRequest* request;
    char* request_str = 0; // a buffer to hold the request string
    char* reply = 0;
    int bufsize = 4096;
    int sock = 0;
    int verbose = 1;

    const char* hostname = "tycho.usno.navy.mil";
    const char* resource = "/timer.html";

    // allocate a buffer for the request string
    request_str = malloc(bufsize);
    if (!request_str) {
        fprintf(stderr, "%s(), request_str: %p\n", __FUNCTION__, request_str);
        goto exit;
    }

    // allocate a buffer for the text of the reply
    reply = malloc(bufsize);
    if (!reply) {
        fprintf(stderr, "%s(), reply: %p\n", __FUNCTION__, reply);
        goto exit;
    }
    memset(reply, 0, bufsize);


    // formulate the request. 
    request = http_alloc_request(hostname, resource);
    if (!request) {
        fprintf(stderr, "http_alloc_request() failed.\n");
        goto exit;
    }

    //rc = http_set_port(request, 80);
    rc = http_set_user_agent(request, "Brad Selbrede's inet library (Linux v0.3)");
    rc = http_set_basic_auth(request, "YnJhZGxleTpzZWxicmVkZQ==");

    rc = http_to_string(request, request_str, bufsize);
    if (rc != 0) {
        fprintf(stderr, "%s(), http_to_string() returned: %d\n", __FUNCTION__, rc);
        goto exit;
    }

    // and reassure ourselves that it is right and proper. 
    if (verbose > 1) {
        hexdump(request_str, 256);
    }
    printf("\n");
    printf("%s", request_str);
    fflush(stdout);


    // connect to the web server. 
    sock = inet_connect_tcp(hostname, "http");
    if (sock < 0) {
        fprintf(stderr, "%s(), inet_connect_tcp() failed.\n", __FUNCTION__);
        goto exit;
    }

    // send the http request
    size_t bytecount = strlen(request_str); 
    ssize_t result;
    result = inet_send(sock, request_str, bytecount, 20000);
    if (result < 0) {
        fprintf(stderr, "%s(), inet_send() failed.\n", __FUNCTION__);
        goto exit;
    } else if (verbose) {
        fprintf(stderr, "expected to send %lu bytes.\n", bytecount);
        fprintf(stderr, "actually sent %ld bytes.\n", result);
    }


    result = inet_wait_reply(sock, reply, bufsize, 30000);
    if (result < 0) {
        fprintf(stderr, "%s(), inet_wait_reply() failed.\n", __FUNCTION__);
        goto exit;
    } else if (verbose){
        fprintf(stderr, "received %ld bytes.\n\n", result);
    }

    printf("%s\n", reply);



exit:
    if (sock > 0) {
        close(sock);
    }

    if (reply) {
        free(reply);
    }

    if (request_str) {
        free(request_str);
    }

    http_free_request(request);

    return 0;
}

