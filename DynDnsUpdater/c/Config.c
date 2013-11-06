#include "Config.h"
#include <stdio.h>
//#include <stdlib.h>

static const int g_period = 28; // days
static const char* g_stateFilename = "/var/tmp/DynDnsUpdaterState.dat";
//static const char* g_detectURL = "http://checkip.dyndns.org";
static const char* g_detectURL = "http://automation.whatismyip.com/n09230945.asp";
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


