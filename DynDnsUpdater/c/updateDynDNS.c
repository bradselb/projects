#include "Config.h"
#include "State.h"
#include "getURL.h"
#include "extractIpAddress.h"
#include "findReply.h"
#include "timeHelpers.h"
#include "params.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// update this with major revisions. 
#define  VERSION  "v0.96 (22-Apr-2015)"

// req'd format is: "Company-model-version"
static const char* USER_AGENT = "BradSelb-Embedded DynDNS Updater-" VERSION;
static const char* DEFAULT_CONFIG_FILE_PATH = "/var/tmp/DynDnsUpdaterConfig.dat";


// --------------------------------------------------------------------------
// local functions
static char* createResourceQueryString(const struct Config* config, const char* ip);


// --------------------------------------------------------------------------
int main(int argc, char* argv[])
{
   int rc;
   const char* config_file_path;
   struct Config* config = 0;
   struct State* state = 0;
   char currentIp[32];
   int daysSinceLastUpdate;
   char* resource = 0; // the HTTP resource string with query appended
   const int responseSize = 512;
   char response[responseSize];

   rc = 1; // return failure by default. 

   memset(response, 0, sizeof response);
   memset(currentIp, 0, sizeof currentIp);


   get_cmdline_options(argc, argv);

   if (params->help) {
      print_help();
      goto exit;
   }

   if (params->version) {
      printf("%s\n", USER_AGENT);
      goto exit;
   }

   if (params->cfgfile) {
      config_file_path = params->cfgfile;
   } else {
      config_file_path = DEFAULT_CONFIG_FILE_PATH;
   }



   config = loadConfig(config_file_path);
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
      fprintf(stderr, "creating a default state file.\n");
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
   else if (0 != (rc = sendHttpRequestAndWaitForReply(getDetectHostname(config), getDetectResource(config), USER_AGENT, 0, response, responseSize))) {
      fprintf(stderr, "sendHttpRequestAndWaitForReply() returned: %d\n", rc);
   }
   else if (0 != (rc = extractIpAddress(response, currentIp, sizeof currentIp / sizeof currentIp[0]))) {
      fprintf(stderr, "failed to extract Ip address from response text\n");
      fprintf(stderr, "%s\n", response);
   }
   else if (0 == (resource = createResourceQueryString(config, currentIp))) {
      fprintf(stderr, "failed to create update resource query\n");
   }
   else if ((daysSinceLastUpdate = daysSince(getUpdateDateTimeString(state))) < 0) {
      fprintf(stderr, "unable to compute days since last update\n"); 
   }
   else if (0 == strcmp(currentIp, getPrevIp(state)) && daysSinceLastUpdate < getPeriod(config)) {
      fprintf(stderr, "current IP Address is: '%s'\n", currentIp);
      fprintf(stderr, "IP has not changed and only %d days have elapsed since last update.\n", daysSinceLastUpdate);
   }
   else { 
      fprintf(stdout, "previous IP Address is: '%s'\n", getPrevIp(state));
      fprintf(stdout, "current  IP Address is: '%s'\n", currentIp);
      rc = 0; // report success.


      memset(response, 0, sizeof response);
      rc = sendHttpRequestAndWaitForReply(getUpdateHostname(config), resource, USER_AGENT, getAuthorization(config), response, responseSize);
      //fprintf(stderr, "%s\n", response);

      if (rc) {
         setEnabled(state, 0);
      } else  {
         // update the state
         const char* reply = 0;
         reply = findReply(response, responseSize);
         setEnabled(state, 1);
         setUpdateTimeNow(state);
         setIp(state, currentIp);
         setResult(state, reply);
      }
      saveState(state, getStateFilename(config));
   }


exit:

   if (resource) {
      free(resource);
   }
   deleteState(state);
   deleteConfig(config);

   return rc;
}

// --------------------------------------------------------------------------
// We have to include some form data in the http request that we send to
// the IP address. This function will append the query to the base resource
// if successful, this function allocates a buffer from the heap! 
// caller **MUST** free the returned buffer!!!
// returns the augmented resource if and returns zero if not successful.
char* createResourceQueryString(const struct Config* config, const char* ip)
{
    char* buf = 0;
    int bufsize;
    
    bufsize = strlen(getUpdateResource(config))
            + strlen("?hostname=") + strlen(getHostname(config))
            + strlen("&myip=") + strlen(ip)
            + 10; // gratuitous fudge factor.

    buf = malloc(bufsize);
    if (buf) {
        memset(buf, 0, bufsize);
        snprintf(buf, bufsize, "%s?hostname=%s&myip=%s", getUpdateResource(config), getHostname(config), ip);
    }
    return buf;
}

