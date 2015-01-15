#if !defined DYNDNSUPDATERCONFIG_H
#define DYNDNSUPDATERCONFIG_H

// A config file is used to persit information that does not change.
// the config file includes the following information:
// my hostname
// my DynDNS authorization token 
// a time period in days - force an update every <period> days
//
// this header file presents the public interface of config file object.

struct Config;

// loadConfig(), readConfig() and createDefaultConfig() all allocate
//  a new config object - which MUST be deleted by deleteConfig().
void deleteConfig(struct Config*);

// read the config from a file
// returns NULL if there was an error.
struct Config* loadConfig(const char* filename);

// writes the config to a file
// returns zero if successful, non zero otherwise.
int saveConfig(const struct Config*, const char* filename);

// low level function similar to load() 
struct Config* readConfig(int fd);

// low level function similar to save().
// returns number of bytes written on success less than zero otherwise.
int writeConfig(const struct Config*, int fd);

// only need to do this if there is no previously saved config 
// returns NULL if there was an error.
struct Config* createDefaultConfig(void);

// access data members
int getPeriod(const struct Config*);
const char* getStateFilename(const struct Config* config);
const char* getDetectHostname(const struct Config* config);
const char* getDetectResource(const struct Config* config);
const char* getUpdateHostname(const struct Config* config);
const char* getUpdateResource(const struct Config* config);
const char* getHostname(const struct Config* config);
const char* getAuthorization(const struct Config* config);

// manipulate data members
int setPeriod(struct Config*, int period);
int setStateFilename(struct Config*, const char*);
int setDetectHostname(struct Config*, const char*);
int setDetectResource(struct Config*, const char*);
int setUpdateHostname(struct Config*, const char*);
int setUpdateResource(struct Config*, const char*);
int setHostname(struct Config*, const char*);
int setAuthorization(struct Config*, const char*);

int compareConfig(const struct Config*, const struct Config*);
#endif //!defined DYNDNSUPDATERCONFIG_H

