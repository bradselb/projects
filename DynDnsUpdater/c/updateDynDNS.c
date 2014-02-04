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
static const char* g_UserAgent = "Self-Brad's embedded DynDNS Updater-v0.7";


// --------------------------------------------------------------------------
int main(int argc, char* argv[])
{
   int rc = 0;
   struct Config* config = 0;
   struct State* state = 0;
   char currentIp[32];
   int daysSinceLastUpdate;
   const int replySize = 512;
   char reply[replySize];

   memset(reply, 0, sizeof reply);
   memset(currentIp, 0, sizeof currentIp);

   config = createDefaultConfig();

   if (!config) {
      fprintf(stderr, "failed to allocate Config object\n");
   }
   else if (!(state = newState())) {
      fprintf(stderr, "failed to allocate State object\n");
   }
   else if ( 0 != (rc = loadState(state, getStateFilename(config)))) {
      fprintf(stderr, "State File does not exist\n");
      fprintf(stderr, "creating a default state file\n");
      setEnabled(state, 1);
      setUpdateTimeNow(state);
      setIp(state, "127.0.0.1");
      setResult(state, "good");
      saveState(state, getStateFilename(config));
   }
   else if ( !isEnabled(state) ) {
      fprintf(stderr, "not enabled.\n");
   } 
   else if ( 0 != strncmp(getPrevResult(state), "good", 4) ) {
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
      fprintf(stderr, "current IP Address is: '%s'\n", currentIp);
      fprintf(stderr, "IP has not changed and only %d days have elapsed since last update.\n", daysSinceLastUpdate);
   }
   else { 

      fprintf(stderr, "current IP Address is: '%s'\n", currentIp);

      // add the form info to the update URL 
      int urlbufsize = strlen(getUpdateURL(config)) + strlen("?hostname=") + strlen(getHostname(config)) + strlen("&myip=") + strlen(currentIp) + 10;
      char* urlbuf = malloc(urlbufsize);
      memset(urlbuf, 0, urlbufsize);
      snprintf(urlbuf, urlbufsize, "%s?hostname=%s&myip=%s", getUpdateURL(config), getHostname(config), currentIp);
      fprintf(stderr, "URL is: %s\n", urlbuf);

      // send the updated ip address to  DynDNS.org
      memset(reply, 0, sizeof reply);
      rc = getURL(reply, replySize, urlbuf, g_UserAgent, getUsername(config), getPassword(config));
      free(urlbuf);
      urlbuf = 0;

      if ( rc ) {
         setEnabled(state, 0);
         fprintf(stderr, "getURL() returned: %d", rc);
         fprintf(stderr, "%s\n", reply);
      } else {
         fprintf(stdout, "Update result is: %s\n",  reply);

         // update the state
         setEnabled(state, 1);
         setUpdateTimeNow(state);
         setIp(state, currentIp);
         setResult(state, reply);

      }
      // persit state to file. 
      saveState(state, getStateFilename(config));
   }

   deleteState(state);
   deleteConfig(config);

    return 0;
}

