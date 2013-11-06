#if !defined DYNDNSUPDATERCONFIG_H
#define DYNDNSUPDATERCONFIG_H

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

int initializeConfig(struct Config*);
int saveConfig(struct Config*, const char* filename);


#endif //!defined DYNDNSUPDATERCONFIG_H

