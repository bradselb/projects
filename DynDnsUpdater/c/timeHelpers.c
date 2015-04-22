#define _XOPEN_SOURCE 

//#define DEBUG 1

#include "timeHelpers.h"

#include <string.h> // memset()
#include <time.h>

#ifdef DEBUG
#include <stdio.h> // fprintf()
#endif

// format is: 28-Feb-2011_18:34:09
static const char* g_TimeFormat = "%d-%b-%Y %T %Z";
static const int g_SecondsPerDay = 24 * 60 * 60; // (hr/day) * (min/hr) * (sec/min)



size_t getTimeNow(char* buf, int bufsize)
{
   memset(buf, 0, bufsize);
   time_t timenow = time(0);
   size_t n = strftime(buf, bufsize, g_TimeFormat , localtime(&timenow));
   return n;
}




// calculate days between the time represented by the string 
// and now. 
int daysSince(const char* datetime)
{
   int days = -1;
   time_t timenow;
   time_t timepast;
   struct tm tm;

   timenow = time(0);

   memset(&tm, 0, sizeof tm);
   tm.tm_isdst = -1; // no info about DST.
   strptime(datetime, g_TimeFormat, &tm);
   timepast = mktime(&tm);
   if (0 < timepast) {
      double seconds = difftime(timenow, timepast);
      days = seconds / g_SecondsPerDay;

#ifdef DEBUG
      char buf[32];
      unsigned bufsize = sizeof buf;
      memset(buf, 0, bufsize);
      strftime(buf, bufsize, g_TimeFormat , localtime(&timenow));
      fprintf(stderr, "time now: %s\n", buf);
      memset(buf, 0, bufsize);
      strftime(buf, bufsize, g_TimeFormat , localtime(&timepast));
      fprintf(stderr, "time past: %s\n", buf);
      //fprintf(stderr, "time past: %s\n", asctime(&tm));
      fprintf(stderr, "time difference is: %.3f seconds (%.3f days)\n", seconds, seconds/g_SecondsPerDay);
#endif
   }

   return days;
}
