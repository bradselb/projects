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

// construct a default Config object
struct Config* newConfig(void);
void deleteConfig(struct Config*);

int loadConfig(struct Config*, const char* filename);
int saveConfig(const struct Config*, const char* filename);

int readConfig(struct Config*, int fd);
int writeConfig(const struct Config*, int fd);

int getPeriod(const struct Config*);
const char* getStateFilename(struct Config* config);
const char* getDetectURL(struct Config* config);
const char* getUpdateURL(struct Config* config);
const char* getHostname(struct Config* config);
const char* getUsername(struct Config* config);
const char* getPassword(struct Config* config);

int setPeriod(struct Config*, int period);
int setStateFilename(struct Config*, const char*);
int setDetectURL(struct Config*, const char*);
int setUpdateURL(struct Config*, const char*);
int setHostname(struct Config*, const char*);
int setUsername(struct Config*, const char*);
int setPassword(struct Config*, const char*);

#endif //!defined DYNDNSUPDATERCONFIG_H

