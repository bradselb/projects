#include "getURL.h" 

#include <string.h> // memset(), memcpy()
#include <stdio.h> //fprintf()


#if !defined DO_NOT_USE_CURL_LIBRARY

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

#else  // use a stub
int getURL(char* buf, int bufsize, const char* url, const char* agent, const char* user, const char* pass)
{
   int rc = -1;
   return rc; 
}


#endif

// --------------------------------------------------------------------------

