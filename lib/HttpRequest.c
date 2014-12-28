#include "HttpRequest.h"

#include <string.h>
#include <stdlib.h> // malloc(), free()
#include <stdio.h> // snprintf()

static const int MAX_STRLEN = 255;
static const int DEFAULT_PORT = 80;

static int allocate_and_copy_string(char** dst, const char* src);
static int destroy_string(char* s);

struct HttpRequest
{
    char* host;
    char* resource;
    char* user_agent;
    char* authorization;
    int port;
    int is_initialized;
};


struct HttpRequest* http_alloc_request(const char* host, const char* resource)
{
    struct HttpRequest* request = 0;
    int rc_host, rc_rsrc;

    //fprintf(stderr, "%s()\n", __FUNCTION__);

    request = malloc(sizeof(struct HttpRequest));
    if (request) {
        memset(request, 0, sizeof(struct HttpRequest));

        rc_host = http_set_host(request, host);
        http_set_port(request, DEFAULT_PORT);
        rc_rsrc = http_set_resource(request, resource);
        
        if (0 == rc_host && 0 == rc_rsrc) {
            request->is_initialized = 1;
        }
    }

    return request;
}


void http_free_request(struct HttpRequest* request)
{
    //fprintf(stderr, "%s()\n", __FUNCTION__);

    if (request) {
        destroy_string(request->host);
        destroy_string(request->resource);
        destroy_string(request->user_agent);
        destroy_string(request->authorization);
        memset(request, 0, sizeof *request);
        free(request);
    }
}


int http_set_host(struct HttpRequest* request, const char* host)
{
    int rc = -1;

    //fprintf(stderr, "%s()\n", __FUNCTION__);
 
    if (request) {
        rc = allocate_and_copy_string(&request->host, host);
    }

    return rc;
}


int http_set_port(struct HttpRequest* request, int  port)
{
    int rc = -1;

    //fprintf(stderr, "%s()\n", __FUNCTION__);
 
    if (request) {
        request->port = port;
        rc = 0;
    }

    return rc;
}


int http_set_resource(struct HttpRequest* request, const char* resource)
{
    int rc = -1;

    //fprintf(stderr, "%s()\n", __FUNCTION__);
 
    if (request) {
        rc = allocate_and_copy_string(&request->resource, resource);
    }

    return rc;
}


int http_set_user_agent(struct HttpRequest* request, const char* user_agent)
{
    int rc = -1;

    //fprintf(stderr, "%s()\n", __FUNCTION__);
 
    if (request) {
        rc = allocate_and_copy_string(&request->user_agent, user_agent);
    }

    return rc;
}


int http_set_basic_auth(struct HttpRequest* request, const char* auth)
{
    int rc = -1;

    //fprintf(stderr, "%s()\n", __FUNCTION__);
 
    if (request && auth) {
        rc = allocate_and_copy_string(&request->authorization, auth);
    }

    return rc;
}



int http_to_string(struct HttpRequest* request, char* buf, unsigned int bufsize)
{
    int rc = -1;
    unsigned int length = 0; // length of string in the buffer
    unsigned int size; // size of space remaining in the buffer
    int count; // how many characters were copied to the buffer this time.

    //fprintf(stderr, "%s()\n", __FUNCTION__);

    if (request && request->is_initialized && buf && bufsize > 0) {
        memset(buf, 0, bufsize);

        size = bufsize - (length + 1);
        count = snprintf(buf+length, size, "GET %s HTTP/1.0\r\n", request->resource);
        if (0 < count && count < size) {
            length += count;
        } else {
            goto exit;
        }

        size = bufsize - (length + 1);
        count = snprintf(buf+length, size, "Host: %s:%d\r\n", request->host, request->port);
        if (0 < count && count < size) {
            length += count;
        } else {
            goto exit;
        }

        if (request->user_agent) {
            size = bufsize - (length + 1);
            count = snprintf(buf+length, size, "User-Agent: %s\r\n", request->user_agent);
            if (0 < count && count < size) {
                length += count;
            } else {
                goto exit;
            }
        }

        if (request->authorization) {
            size = bufsize - (length + 1);
            count = snprintf(buf+length, size, "Authorization: Basic %s\r\n", request->authorization);
            if (0 < count && count < size) {
                length += count;
            } else {
                goto exit;
            }
        }

        size = bufsize - (length + 1);
        count = snprintf(buf+length, size, "\r\n");
        if (0 < count && count < size) {
            length += count;
        } else {
            goto exit;
        }

        rc = 0;
    }


exit: 

    return rc;
}


static int allocate_and_copy_string(char** dst, const char* src)
{
    int rc;
    char* buf = 0;
    unsigned int bufsize = 0;

    //fprintf(stderr, "%s(), src: '%s'\n", __FUNCTION__, src);

    if (src) {
        bufsize = strnlen(src, MAX_STRLEN) + 1;
    }

    if (dst && *dst) {
        destroy_string(*dst);
        *dst = 0;
    }

    if (NULL == dst || NULL == src) {
        rc = -1;
    } else if (NULL == (buf = malloc(bufsize))) {
        rc = -1;
    } else {
        memset(buf, 0, bufsize);
        strncpy(buf, src, bufsize-1);
        *dst = buf;
        rc = 0;
    }

    return rc;
}


static int destroy_string(char* s)
{
    int rc = -1;

    //fprintf(stderr, "%s()\n", __FUNCTION__);

    if (s) {
        memset(s, 0, strnlen(s, MAX_STRLEN) + 1);
        free(s);
        rc = 0;
    }
    return rc;
}
