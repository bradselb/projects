#include "getURL.h" 

#include <string.h> // memset(), memcpy()
#include <stdio.h> // fprintf()


#if defined USE_CURL_LIBRARY

#define CURL_NO_OLDIES 1
#include <curl/curl.h>
#include <curl/easy.h>



// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
struct UserData
{
   char* begin;
   char* end;
   size_t size;
};

// --------------------------------------------------------------------------
int initUserData(struct UserData* ud, char* buffer, int size)
{
   int rc = 0;
   if ( !ud || !buffer || size < 2 ) {
      rc = -1; // fail.
   } else {
      memset(buffer, 0, size);
      ud->begin = buffer;
      ud->end = buffer;
      ud->size = size;
      rc = 0;
   }
   return rc;
}

// --------------------------------------------------------------------------
size_t appendUserData(struct UserData* ud, char* p, size_t n)
{
   size_t count = 0;

   if ( p && ud && ud->begin && ud->end ) {
      while ( count < n && (ud->end - ud->begin) + 1 < ud->size ) {
         *(ud->end)++  = *p++;
         ++count;
      }
   }
   return count;
}


// --------------------------------------------------------------------------
size_t response(char* ptr, size_t size, size_t n, void* userdata)
{
   size_t bytesHandled;
   
   bytesHandled = appendUserData((struct UserData*)(userdata), ptr, n*size);
   
   return bytesHandled;
}



// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
// --------------------------------------------------------------------------
int getURL(char* buf, int bufsize, const char* url, const char* agent, const char* user, const char* pass)
{
   int rc = 0;
   
   struct UserData userData;

   CURLcode ec;
   CURL* curl = 0;
   //curl = curl_easy_init();

   if ( initUserData(&userData, buf, bufsize) ) {
      fprintf(stderr,"(%s:%d) %s\n", __FILE__, __LINE__, "");
   }
   else if ( 0 == (curl=curl_easy_init()) ) {
      rc = -1;
      fprintf(stderr,"(%s:%d) %s\n", __FILE__, __LINE__, "curl initialization failed");
   }
   else if ( CURLE_OK != (ec = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L)) ) {
      rc =ec;
      fprintf(stderr,"(%s:%d) %s\n", __FILE__, __LINE__,curl_easy_strerror(ec));
   }
   else if ( CURLE_OK != (ec = curl_easy_setopt(curl, CURLOPT_URL, url)) ) {
      rc = 1;
      fprintf(stderr,"(%s:%d) %s\n", __FILE__, __LINE__, curl_easy_strerror(ec));
   }
   else if ( CURLE_OK != (ec = curl_easy_setopt(curl, CURLOPT_USERAGENT, agent)) ) {
      rc = 2;
      fprintf(stderr,"(%s:%d) %s\n", __FILE__, __LINE__, curl_easy_strerror(ec));
   }
   else if ( CURLE_OK != (ec = curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_0)) ) {
      rc = 3;
      fprintf(stderr,"(%s:%d) %s\n", __FILE__, __LINE__, curl_easy_strerror(ec));
   }
   else if ( user && CURLE_OK != (ec = curl_easy_setopt(curl, CURLOPT_USERNAME, user)) ) {
      rc = 5;
      fprintf(stderr,"(%s:%d) %s\n", __FILE__, __LINE__, curl_easy_strerror(ec));
   }
   else if ( pass && CURLE_OK != (ec = curl_easy_setopt(curl, CURLOPT_PASSWORD, pass)) ) {
      rc = 6;
      fprintf(stderr,"(%s:%d) %s\n", __FILE__, __LINE__, curl_easy_strerror(ec));
   }
   else if ( user && pass && CURLE_OK != (ec = curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC)) ) {
      rc = 7;
      fprintf(stderr,"(%s:%d) %s\n", __FILE__, __LINE__, curl_easy_strerror(ec));
   }
// can ignore headers.
//   else if ( CURLE_OK != (ec = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header)) ) {
//      rc = 8;
//      fprintf(stderr,"(%s:%d) %s\n", __FILE__, __LINE__, curl_easy_strerror(ec));
//   }
   else if ( CURLE_OK != (ec = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response)) ) {
      rc = 9;
      fprintf(stderr,"(%s:%d) %s\n", __FILE__, __LINE__, curl_easy_strerror(ec));
   }
   else if ( CURLE_OK != (ec = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &userData)) ) {
      rc = 10;
      fprintf(stderr,"(%s:%d) %s\n", __FILE__, __LINE__, curl_easy_strerror(ec));
   }
   else if ( CURLE_OK != (ec = curl_easy_perform(curl)) ) {
      rc = 11;
      fprintf(stderr,"(%s:%d) %s\n", __FILE__, __LINE__, curl_easy_strerror(ec));
   }
   else {
      rc = 0; // success!
      fprintf(stderr,"(%s:%d) %s\n", __FILE__, __LINE__, "OK");
   }

   curl_easy_cleanup(curl);

   return rc;
}

#else
#include <stdlib.h>  // malloc()
#include <unistd.h> // close()

#include "HttpRequest.h"
#include "hexdump.h"
#include "inet.h"

// this is a stub that can be substituted for the above....for developemnt or test for example. 
int getURL(char* buf, int bufsize, const char* host, const char* agent, const char* user, const char* pass)
{
   int rc = 0;
   fprintf(stderr, "(%s:%d) %s(), host: %s, agent: %s\n", __FILE__, __LINE__, __FUNCTION__, host, agent);
   if (buf) {
      if (0 == strncmp(host, "checkip", 6)) {
         strncpy(buf, "<html><head><title>Current IP Check</title></head><body>Current IP Address: 198.87.22.17</body></html>", bufsize);
      } else if (0 == strncmp(host, "members", 7)) {
         strncpy(buf, "good 142.21.17.42", bufsize);
      }
   }
   return rc; 
}


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

    //rc = http_set_port(request, 80);
    if (agent) {
        rc = http_set_user_agent(request, agent);
    }

    if (auth) {
        rc = http_set_basic_auth(request, auth);
    }

    rc = http_to_string(request, request_str, request_bufsize);
    if (rc != 0) {
        fprintf(stderr, "%s(), http_to_string() returned: %d\n", __FUNCTION__, rc);
        goto exit;
    }

    // and reassure ourselves that it is right and proper. 
    if (verbose > 0) {
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

    // send the http request to the web server
    size_t bytecount = strlen(request_str); 
    ssize_t result;
    result = inet_send(sock, request_str, bytecount, timeout_send);
    if (result < 0) {
        fprintf(stderr, "%s(), inet_send() failed.\n", __FUNCTION__);
        goto exit;
    } else if (verbose) {
        fprintf(stderr, "expected to send %lu bytes.\n", bytecount);
        fprintf(stderr, "actually sent %ld bytes.\n", result);
    }

    // and wait for the web server to reply
    result = inet_wait_reply(sock, reply, bufsize, timeout_wait);
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

    if (request_str) {
        free(request_str);
    }

    http_free_request(request);

 return rc;
}

#endif

// --------------------------------------------------------------------------

