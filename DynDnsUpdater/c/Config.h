#if !defined DYNDNSUPDATERCONFIG_H
#define DYNDNSUPDATERCONFIG_H

// A config file is used to persit information that does not change.
// the config file includes the following information:
// a time period in days - force an update every <period> days
// location of the state file
// a URL to discover my current IP 
// a URL to update my DynDNS entry
// my hostname
// my DynDNS user name
// my DynDNS password
//
// this header file presents the public interface of config file object.

struct Config;

// loadConfig(), readConfig() and createDefaultConfig() all allocate a new
// config object - which MUST be deleted by deleteConfig().
void deleteConfig(struct Config*);

// can load the config from a file
// returns NULL if there was an error.
struct Config* loadConfig(const char* filename);

// returns zero if successful, non zero otherwise.
int saveConfig(const struct Config*, const char* filename);

// low level functions to deal with persitent config
// similar to load and save above.
struct Config* readConfig(int fd);
//int writeConfig(const struct Config*, int fd);// not yet implemented. 

// only need to do this if there is no previously saved config 
// returns NULL if there was an error.
struct Config* createDefaultConfig(void);

// access data members
int getPeriod(const struct Config*);
const char* getStateFilename(const struct Config* config);
const char* getDetectURL(const struct Config* config);
const char* getUpdateURL(const struct Config* config);
const char* getHostname(const struct Config* config);
const char* getUsername(const struct Config* config);
const char* getPassword(const struct Config* config);

// manipulate data members
int setPeriod(struct Config*, int period);
int setStateFilename(struct Config*, const char*);
int setDetectURL(struct Config*, const char*);
int setUpdateURL(struct Config*, const char*);
int setHostname(struct Config*, const char*);
int setUsername(struct Config*, const char*);
int setPassword(struct Config*, const char*);

int compareConfig(const struct Config*, const struct Config*);
#endif //!defined DYNDNSUPDATERCONFIG_H

