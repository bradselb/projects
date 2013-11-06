// use posix regular expressions in GNU libc to find an IP address in the text
#include "extractIpAddress.h"

#include <regex.h>
#include <stdio.h>
#include <string.h> // memset()


int extractIpAddress(const char* text, char* buf, int bufsize)
{
   int rc =0;
   const char* pattern = "([0-9]+[.][0-9]+[.][0-9]+[.][0-9]+)";

   // a buffer for error messages.
   const int maxmsglen = 128;
   char errmsg[maxmsglen];
   memset(errmsg, 0, maxmsglen);

   regex_t re;
   const int nmatch = 3; // somewhat arbitrary choice.
   regmatch_t match[nmatch];

   if ( 0 != (rc = regcomp(&re, pattern, REG_EXTENDED|REG_NEWLINE)) ) {
      regerror(rc, &re, errmsg, maxmsglen);
      fprintf(stderr, "(%s:%d) regcomp() returned: %d, %s\n", __FILE__, __LINE__, rc, errmsg);
      rc = -1;
   }
   else if ( 0 != (rc = regexec(&re, text, nmatch, match, 0)) ) {
      // rc can be one of 0, REG_NOMATCH, REG_ESPACE
      // REG_NOMATCH is probably the most useful, next to success ==0
      regerror(rc, &re, errmsg, maxmsglen);
      fprintf(stderr, "(%s:%d) regexec() returned: %d, %s\n", __FILE__, __LINE__, rc, errmsg);
      rc = -1;
   }
   else if ( -1 == match[0].rm_so  || -1 == match[0].rm_eo ) {
      fprintf(stderr, "(%s:%d) regexec() returned: %d, start offset: %d, end offset: %d\n", 
                        __FILE__, __LINE__, rc, match[0].rm_so, match[0].rm_eo);
      rc = -1;
   }
   else {
      // success! copy ip address from text buffer to output buffer.
      char* dest = buf;
      char* p = (char*)text + match[0].rm_so;
      char* end = (char*)text + match[0].rm_eo;
      if ( bufsize < (end - p) ) {
         rc = -2;
      } else {
         while ( p != end ) {
            *dest++ = *p++;
         } // while
         *dest = 0;
         rc = 0; // success!
      }
   }

   regfree(&re);

   return rc;
}
