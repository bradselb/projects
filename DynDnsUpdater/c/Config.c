#include "Config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Config
{
   int period; // force an update every <period> days
   char* stateFilename; // where is the state file? 
   char* detectURL; // where to discover my current IP 
   char* updateURL; // where to update my DynDNS entry
   char* hostname;  // my hostname 
   char* username;  // my DynDNS user name
   char* password;  // my DynDNS password
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

static const int MAX_STRING_LENGTH = 4095;
//static const int MAX_STRING_BUF_SIZE = MAX_STRING_LENGTH + 1;



struct Config* newConfig(void)
{
    struct Config* config;
    config = malloc(sizeof(struct Config));
    if (config) {
        memset(config, 0, sizeof(struct Config));
        setPeriod(config, g_period);
        setStateFilename(config, g_stateFilename);
        setDetectURL(config, g_detectURL);
        setUpdateURL(config, g_updateURL);
        setHostname(config, g_hostname);
        setUsername(config, g_username);
        setPassword(config, g_password);
    }
    return config;
}

static void deleteConfigString(char* s)
{
    if(s) {
        memset(s, 0, strlen(s));
        free(s);
    }
}

void deleteConfig(struct Config* config)
{
    if (config) {
        deleteConfigString(config->stateFilename);
        deleteConfigString(config->detectURL);
        deleteConfigString(config->updateURL);
        deleteConfigString(config->hostname);
        deleteConfigString(config->username);
        deleteConfigString(config->password);
        memset(config, 0, sizeof *config);
        free(config);
    }
}


int loadConfig(struct Config* config, const char* filename)
{
    int rc = -1;
    return rc;
}

int saveConfig(const struct Config* config, const char* filename)
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

const char* getStateFilename(struct Config* config)
{
    return (config ? config->stateFilename : 0);
}

const char* getDetectURL(struct Config* config)
{
    return (config ? config->detectURL : 0);
}

const char* getUpdateURL(struct Config* config)
{
    return (config ? config->updateURL : 0);
}

const char* getHostname(struct Config* config)
{
    return (config ? config->hostname : 0);
}

const char* getUsername(struct Config* config)
{
    return (config ? config->username : 0);
}

const char* getPassword(struct Config* config)
{
    return (config ? config->password : 0);
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
    int rc;
    size_t length;
    char* buf;
    const char* src = filename;

    if (!filename) {
        src = g_stateFilename;
    }

    length = strnlen(src, MAX_STRING_LENGTH);

    if (!config) {
        rc = -1;
    } else if (!(buf = malloc(length + sizeof *buf))) {
        rc = -1;
    } else {
        strncpy(buf, src, length);
        if (config->stateFilename) {
            free(config->stateFilename);
        }
        config->stateFilename = buf;
        // now the config object owns the buffer
        rc = 0;
    }
    return rc;
}

int setDetectURL(struct Config* config, const char* src)
{
    int rc;
    size_t length;
    char* buf;

    length = strnlen(src, MAX_STRING_LENGTH);

    if (!config || !src) {
        rc = -1;
    } else if (!(buf = malloc(length + sizeof *buf))) {
        rc = -1;
    } else {
        strncpy(buf, src, length);
        if (config->detectURL) {
            free(config->detectURL);
        }
        config->detectURL = buf;
        // now the config object owns the buffer
        rc = 0;
    }
    return rc;
}

int setUpdateURL(struct Config* config, const char* src)
{
    int rc;
    size_t length;
    char* buf;

    length = strnlen(src, MAX_STRING_LENGTH);

    if (!config || !src) {
        rc = -1;
    } else if (!(buf = malloc(length + sizeof *buf))) {
        rc = -1;
    } else {
        strncpy(buf, src, length);
        if (config->updateURL) {
            free(config->updateURL);
        }
        config->updateURL = buf;
        // now the config object owns the buffer
        rc = 0;
    }
    return rc;
}

int setHostname(struct Config* config, const char* src)
{
    int rc;
    size_t length;
    char* buf;

    length = strnlen(src, MAX_STRING_LENGTH);

    if (!config || !src) {
        rc = -1;
    } else if (!(buf = malloc(length + sizeof *buf))) {
        rc = -1;
    } else {
        strncpy(buf, src, length);
        if (config->hostname) {
            free(config->hostname);
        }
        config->hostname = buf;
        // now the config object owns the buffer
        rc = 0;
    }
    return rc;
}

int setUsername(struct Config* config, const char* src)
{
    int rc;
    size_t length;
    char* buf;

    length = strnlen(src, MAX_STRING_LENGTH);

    if (!config || !src) {
        rc = -1;
    } else if (!(buf = malloc(length + sizeof *buf))) {
        rc = -1;
    } else {
        strncpy(buf, src, length);
        if (config->username) {
            free(config->username);
        }
        config->username = buf;
        // now the config object owns the buffer
        rc = 0;
    }
    return rc;
}

int setPassword(struct Config* config, const char* src)
{
    int rc;
    size_t length;
    char* buf;

    length = strnlen(src, MAX_STRING_LENGTH);

    if (!config || !src) {
        rc = -1;
    } else if (!(buf = malloc(length + sizeof *buf))) {
        rc = -1;
    } else {
        strncpy(buf, src, length);
        if (config->password) {
            free(config->password);
        }
        config->password = buf;
        // now the config object owns the buffer
        rc = 0;
    }
    return rc;
}



