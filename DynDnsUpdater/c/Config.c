#include "Config.h"
#include <stdio.h>
//#include <stdlib.h>

struct Config
{
   int period; // force an update every <period> days
   const char* stateFilename; // where is the state file? 
   const char* detectURL; // where to discover my current IP 
   const char* updateURL; // where to update my DynDNS entry
   const char* hostname;  // my hostname 
   const char* username;  // my DynDNS user name
   const char* password;  // my DynDNS password
};

static const int g_period = 28; // days
static const char* g_stateFilename = "/var/run/DynDnsUpdaterState.dat";
static const char* g_detectURL = "http://checkip.dyndns.org";
static const char* g_updateURL = "http://members.dyndns.org/nic/update";
//static const char* g_hostname = "test.dyndns.org";
static const char* g_hostname = "test.ath.cx";
static const char* g_username = "test";
static const char* g_password = "test";

//hostname("test.dnsalias.net"),
//hostname("test.dnsalias.org"),
//hostname("test.dnsalias.com"),
//hostname("test.homeip.net"),


int initializeConfig(struct Config* config)
{
   int rc = -1;
 
   if ( !config ) {
      rc = -1; // fail
   }
   else {
      config->period = g_period; // days
      config->stateFilename = g_stateFilename;
      config->detectURL = g_detectURL;
      config->updateURL = g_updateURL;
      config->hostname = g_hostname;
      config->username = g_username;
      config->password = g_password;
      rc = 0;
   }

   return rc;
}


int loadConfig(struct Config*, const char* filename)
{
}

int saveConfig(struct Config* config, const char* filename)
{
   int rc = -1;
   FILE* file = 0;

   if ( !config ) {
      rc = -1;
   }
   else if ( 0 == (file = fopen(filename, "w")) ) {
      rc = -1;
   }
   else {
      fprintf(file, "period %d\n", config->period);
      fprintf(file, "State-Filename %s\n", config->stateFilename);
      fprintf(file, "Detect-URL %s\n", config->detectURL);
      fprintf(file, "Update-URL %s\n", config->updateURL);
      fprintf(file, "Hostname %s\n", config->hostname);
      fprintf(file, "Username %s\n", config->username);
      fprintf(file, "Password %s\n", config->password);
      fclose(file);
      rc = 0;
   }
   return rc;
}


int getPeriod(const struct Config* config)
{
    int rc = -1;
    if (config) {
        rc = config->period;
    }
    return rc;
}

int getStateFilename(struct Config* config, char* buf, int bufsize);
{
    int rc = -1;
    if (config && buf && bufsize>0) {
        rc = snprintf(buf, bufsize, "%s", config->stateFilename);
    }
    return rc;
}

int getDetectURL(struct Config* config, char* buf, int bufsize)
{
    int rc = -1;
    if (config && buf && bufsize>0) {
        rc = snprintf(buf, bufsize, "%s", config->detectURL);
    }
    return rc;
}

int getUpdateURL(struct Config* config, char* buf, int bufsize)
{
    int rc = -1;
    if (config && buf && bufsize>0) {
        rc = snprintf(buf, bufsize, "%s", config->updateURL);
    }
    return rc;
}

int getHostname(struct Config* config, char* buf, int bufsize)
{
    int rc = -1;
    if (config && buf && bufsize>0) {
        rc = snprintf(buf, bufsize, "%s", config->hostname);
    }
    return rc;
}

int getUsername(struct Config* config, char* buf, int bufsize)
{
    int rc = -1;
    if (config && buf && bufsize>0) {
        rc = snprintf(buf, bufsize, "%s", config->username);
    }
    return rc;
}

int getPassword(struct Config* config, char* buf, int bufsize)
{
    int rc = -1;
    if (config && buf && bufsize>0) {
        rc = snprintf(buf, bufsize, "%s", config->password);
    }
    return rc;
}


int setPeriod(struct Config* config, int period)
{
    int rc = 0;
    if (!config) {
        rc = -1;
    } else if (period > 0) {
        config->period = period;
        rc = 0;
    } else {
        config->period = g_period;
        rc = 0;
    }
    return rc;
}

int setStateFilename(struct Config* config, const char* filename)
{
    int rc = 0;
    if (!config) {
        rc = -1;
    } else if (filename) {
        config->period = period;
        rc = 0;
    } else {
        config->period = g_period;
        rc = 0;
    }
    return rc;
}

int setDetectURL(struct Config* config, const char*)
int setUpdateURL(struct Config* config, const char*)
int setHostname(struct Config* config, const char*)
int setUsername(struct Config* config, const char*)
int setPassword(struct Config* config, const char*)


#if 0
std::istream& Config::fromStream(std::istream& is)
{
   std::string name;
   std::string value;
   while ( is ) {
      is >> name >> value;
      transform(name.begin(), name.end(),name.begin(), tolower);
      if ( name.empty() || value.empty() ) {
      } else if ( 0 == name.compare("period") ) {
         long t = strtol(value.c_str(),0,0);
         this->period = t;
      } else if ( 0 == name.compare("State-Filename") ) {
         stateFilename.assign(value);
      } else if ( 0 == name.compare("Detect-URL") ) {
         detectURL.assign(value);
      } else if ( 0 == name.compare("Update-URL") ) {
         updateURL.assign(value);
      } else if ( 0 == name.compare("Hostname") ) {
         hostname.assign(value);
      } else if ( 0 == name.compare("Username") ) {
         username.assign(value);
      } else if ( 0 == name.compare("Password") ) {
         password.assign(value);
      }
   }
}
#endif


