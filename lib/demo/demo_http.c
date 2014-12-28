
#include "HttpRequest.h"
#include "hexdump.h"

#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
//#include <ctype.h>

int main(int argc, char* argv[])
{
    int rc;
    struct HttpRequest* request;
    char* buf;
    int bufsize = 4096;

    request = http_alloc_request("tycho.usno.navy.mil", "/timer.html");
    if (!request) 
        fprintf(stderr, "http_alloc_request() failed.\n");

    //rc = http_set_port(request, 8245);
    rc = http_set_user_agent(request, "NutScrape 2.0 (Linux)");
    rc = http_set_basic_auth(request, "YnJhZGxleTpzZWxicmVkZQ==");

    buf = malloc(bufsize);
    fprintf(stderr, "%s(), buf: %p\n", __FUNCTION__, buf);

    rc = http_to_string(request, buf, bufsize);
    fprintf(stderr, "%s(), http_to_string() returned: %d\n", __FUNCTION__, rc);

    hexdump(buf, 256);
    printf("\n");

    printf("%s", buf);
    fflush(stdout);

    free(buf);

    http_free_request(request);

    return 0;
}

