#include "Config.h"
#include "State.h"
#include "getURL.h"
#include "extractIpAddress.h"
#include "timeHelpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// update this with major revisions. 
// req'd format is: "Company-model-version"
static const char* g_UserAgent = "Self-Brad's embedded DynDNS Updater-v0.2";


int main(int argc, char* argv[])
{
   int rc = 0;
   struct Config* config = 0;
   State_t state = 0;
   const int replySize = 512;
   char reply[replySize];
   char currentIp[32];
   int daysSinceLastUpdate;

   memset(reply, 0, sizeof reply);
   memset(currentIp, 0, sizeof currentIp);

   config = newConfig();
   //TODO apply cmd line args to config

   if (!config) {
      fprintf(stderr, "failed to allocate Config object\n");
   }
   else if (!(state = newState())) {
      fprintf(stderr, "failed to allocate State object\n");
   }
   else if ( 0 != (rc = loadState(state, getStateFilename(config)))) {
      fprintf(stderr, "State File does not exist\n");
      fprintf(stderr, "creating a default state file\n");
      setEnabled(state, 0);
      setUpdateTimeNow(state);
      setIp(state, "127.0.0.1");
      setResult(state, "good");
      saveState(state, getStateFilename(config));
   }
   else if ( !isEnabled(state) ) {
      fprintf(stderr, "not enabled.\n");
   } 
   else if ( 0 != strcmp(getPrevResult(state), "good") ) {
      fprintf(stderr, "result of previous update was '%s', not 'good'\n", getPrevResult(state));
   } 
//int getURL(char* buf, int bufsize, const char* url, const char* userAgent, const char* user, const char* pass );
   else if ( 0 != (rc = getURL(reply, replySize, getDetectURL(config), g_UserAgent, 0, 0)) ) {
      fprintf(stderr, "getURL(%s) returned: %d\n", getDetectURL(config), rc);
   }
   else if ( 0 != (rc = extractIpAddress(reply, currentIp, sizeof currentIp / sizeof currentIp[0])) ) {
      fprintf(stderr, "failed to extract Ip address from reply text\n");
      fprintf(stderr, "%s\n", reply);
   }
   else if ( (daysSinceLastUpdate = daysSince(getUpdateDateTimeString(state))) < 0 ) {
      fprintf(stderr, "unable to compute days since last update\n"); 
   }
   else if ( 0 == strcmp(currentIp, getPrevIp(state)) && daysSinceLastUpdate < getPeriod(config) ) {
      fprintf(stderr, "IP has not changed\n");
   }
   else { 

      fprintf(stderr, "current IP Address is: '%s'\n", currentIp);

      // add the form info to the update URL 
      //stringstream ss;
      //ss << getUpdateURL(config) << "?hostname=" << getHostname(config) << "&myip=" << currentIp;
      //string url(ss.str());

      int urlbufsize = strlen(getUpdateURL(config)) + strlen("?hostname=") + strlen(getHostname(config)) + strlen("&myip=") + strlen(currentIp) + 10;
      char* urlbuf = (char*)malloc(urlbufsize);
      memset(urlbuf, 0, urlbufsize);
      snprintf(urlbuf, urlbufsize, "%s?hostname=%s&myip=%s", getUpdateURL(config), getHostname(config), currentIp);
      fprintf(stderr, "URL is: %s\n", urlbuf);

      memset(reply, 0, sizeof reply);
      // send the updated ip address to  DynDNS.org
//int getURL(char* buf, int bufsize, const char* url, const char* userAgent, const char* user=0, const char* pass=0 );
      rc = getURL(reply, replySize, urlbuf, g_UserAgent, getUsername(config), getPassword(config));
      free(urlbuf);
      urlbuf = 0;

      if ( rc ) {
         setEnabled(state, 0);
         fprintf(stderr, "getURL() returned: %d", rc);
         fprintf(stderr, "%s\n", reply);
      } else {
         fprintf(stdout, "Update result is: %s",  reply);
         //stringstream is(reply);
         //string result;
         //is >> result;
         //char result[12];

         // update the state
         setEnabled(state, 1);
         setUpdateTimeNow(state);
         setIp(state, currentIp);
         //setResult(state, result);

      }
      // persit state to file. 
      saveState(state, getStateFilename(config));
   }

    if (state) {
        deleteState(state);
    }

    if (config) {
        deleteConfig(config);
    }

    return 0;
}

#if 0 
namespace {
int strsubstr(char* dest, int destsize, const char* begin, const char* end)
{
   int rc = 0;
   if ( !(dest && begin && end) ) {
      rc = -1;
   } else if ( destsize < (end - begin) ) {
      rc = -2;
   } else {
      char* src = begin;
      while ( src != end ) {
         *dest++ = *src++;
      }
      *dest = 0;
      rc = 0; // success!
   }
   return rc;
}
}; // namespace.

#endif
