#include "timeHelpers.h"

//#define _XOPEN_SOURCE /* glibc2 needs this */
#include <time.h> 
#include <string.h> // memset()


   // format is: 28-Feb-2011_18:34:09
static const char* g_TimeFormat = "%d-%b-%Y_%T";
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

   time_t timenow = time(0);

   struct tm tm;
   strptime(datetime, g_TimeFormat, &tm);

   time_t timepast = mktime(&tm);
   if ( 0 < timepast ) {
      days = (timenow - timepast) / g_SecondsPerDay;// yes, integer division.  
   }

   return days;
}
