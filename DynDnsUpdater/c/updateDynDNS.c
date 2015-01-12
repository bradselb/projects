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
static const char* USER_AGENT = "Self-Brad's embedded DynDNS Updater-v0.8";
static const char* DEFAULT_CONFIG_FILE_PATH = "/var/tmp/DynDnsUpdaterConfig.dat";


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


   config = loadConfig(DEFAULT_CONFIG_FILE_PATH);
   if (!config) {
      fprintf(stderr, "Config File does not exist\n");
      fprintf(stderr, "creating a default config, '%s'\n", DEFAULT_CONFIG_FILE_PATH);
      config = createDefaultConfig();
      saveConfig(config, DEFAULT_CONFIG_FILE_PATH);
   }

   if (!config) {
      fprintf(stderr, "failed to allocate Config object\n");
   }
   else if (!(state = newState())) {
      fprintf(stderr, "failed to allocate State object\n");
   }
   else if (0 != (rc = loadState(state, getStateFilename(config)))) {
      fprintf(stderr, "State File does not exist\n");
      fprintf(stderr, "creating a default state file\n");
      setEnabled(state, 1);
      setUpdateTimeNow(state);
      setIp(state, "127.0.0.1");
      setResult(state, "good");
      saveState(state, getStateFilename(config));
   }
   else if (!isEnabled(state)) {
      fprintf(stderr, "not enabled.\n");
   } 
   else if (0 != strncmp(getPrevResult(state), "good", 4)) {
      fprintf(stderr, "result of previous update was '%s', not 'good'\n", getPrevResult(state));
   } 
   else if (0 != (rc = sendHttpRequestAndWaitForReply(getDetectHostname(config), getDetectResource(config), USER_AGENT, 0, reply, replySize))) {
      fprintf(stderr, "getURL() returned: %d\n", rc);
   }
   else if (0 != (rc = extractIpAddress(reply, currentIp, sizeof currentIp / sizeof currentIp[0]))) {
      fprintf(stderr, "failed to extract Ip address from reply text\n");
      fprintf(stderr, "%s\n", reply);
   }
   else if ((daysSinceLastUpdate = daysSince(getUpdateDateTimeString(state))) < 0) {
      fprintf(stderr, "unable to compute days since last update\n"); 
   }
   else if (0 == strcmp(currentIp, getPrevIp(state)) && daysSinceLastUpdate < getPeriod(config)) {
      fprintf(stderr, "current IP Address is: '%s'\n", currentIp);
      fprintf(stderr, "IP has not changed and only %d days have elapsed since last update.\n", daysSinceLastUpdate);
   }
   else { 

      fprintf(stderr, "current IP Address is: '%s'\n", currentIp);

      // add the form info to the update resource 
      int urlbufsize = strlen(getUpdateResource(config)) + strlen("?hostname=") + strlen(getHostname(config)) + strlen("&myip=") + strlen(currentIp) + 10;
      char* urlbuf = malloc(urlbufsize);
      memset(urlbuf, 0, urlbufsize);
      snprintf(urlbuf, urlbufsize, "%s?hostname=%s&myip=%s", getUpdateResource(config), getHostname(config), currentIp);
      fprintf(stderr, "resource string is: %s\n", urlbuf);

      // send the updated ip address to  DynDNS.org
      memset(reply, 0, sizeof reply);
      rc = sendHttpRequestAndWaitForReply(getUpdateHostname(config), urlbuf, USER_AGENT, getAuthorization(config), reply, replySize);
      free(urlbuf);
      urlbuf = 0;

      if ( rc ) {
         setEnabled(state, 0);
         fprintf(stderr, "sendHttpRequestAndWaitForReply() returned: %d", rc);
         fprintf(stderr, "%s\n", reply);
      } else  {
         fprintf(stdout, "Update result is: %s\n",  reply);

         // update the state
         setEnabled(state, 1);
         setUpdateTimeNow(state);
         setIp(state, currentIp);
         // TODO: need to parse the actual result from the reply buffer. 
         setResult(state, reply);

      }
      //TODO: have to pull the actual result from the reply. (see above)
      // saveState(state, getStateFilename(config));
   }

   deleteState(state);
   deleteConfig(config);

    return 0;
}

