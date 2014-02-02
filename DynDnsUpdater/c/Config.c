#include "Config.h"
#include "tokenize.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h> // read(), write()

// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
static const int g_period = 28; // days
static const char* g_stateFilename = "/var/tmp/DynDnsUpdaterState.dat";
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

// --------------------------------------------------------------------------
#define MAX_STRING_LENGTH 1023

// maximum size of buffer used to read/write the whole config file. The magic
// number 8 comes from there being seven elements, seven lines in the file.
// Each line maybe a little longer than the max string length.
#define MAX_BUF_SIZE  (8 * (1 + MAX_STRING_LENGTH))



// --------------------------------------------------------------------------
struct Config* createDefaultConfig(void)
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

// --------------------------------------------------------------------------
static void deleteConfigString(char* s)
{
    if(s) {
        memset(s, 0, strnlen(s, MAX_STRING_LENGTH));
        free(s);
    }
}

// --------------------------------------------------------------------------
void deleteConfig(struct Config* config)
{
    if (config) {
        deleteConfigString(config->stateFilename);
        deleteConfigString(config->detectURL);
        deleteConfigString(config->updateURL);
        deleteConfigString(config->hostname);
        deleteConfigString(config->username);
        deleteConfigString(config->password);
        memset(config, 0, sizeof(struct Config));
        free(config);
    }
}


// --------------------------------------------------------------------------
struct Config* loadConfig(const char* filename)
{
    struct Config* config = 0;
    return config;
}

// --------------------------------------------------------------------------
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


// --------------------------------------------------------------------------
static int is_file_size_less_than(int fd, ssize_t size)
{
    int rc = 0; // no it is not. 

    int err;
    struct stat st;

    memset(&st, 0, sizeof st);
    
    err = fstat(fd, &st);
    if (0 == err && st.st_size < size) {
        // success.
        rc = 1;
    }
    return rc;
}


// --------------------------------------------------------------------------
// expectations: 
//  1) the file descriptor is associated with a valid open plain text file
//  2) the content of the file is roughly conformat to that which is written 
//     by the function saveConfig() above. That is, each line contains a simple 
//     key-value pair delimited by whitespace only (implies whitespace not 
//     allowed in either the key string nor in the value. 
//  3) key-value pairs may be arranged in any order.
//  4) whole file fits in one buffer...rethink this.
//
// plan: 
// read the whole file in at once.
// tokenize the buffer and extract argv and argc
// iterate over the argv and compare each key
struct Config* readConfig(int fd)
{
    struct Config* config = 0;
    char* buf = 0;
    size_t bufsize; // allocated size
    ssize_t bytes_read; // how many bytes read this time
    size_t length; // how many bytes are in the bufffer

    char* tokbuf;
    size_t tokbufsize;
    int tokcount;
    //int err;

    if (!is_file_size_less_than(fd, MAX_BUF_SIZE)) {
        // file too big.
        goto out_final;
    }

    bufsize = MAX_BUF_SIZE;
    buf = malloc(bufsize);
    if (!buf) {
        goto out_free_buf;
    }

    memset(buf, 0, bufsize);

    // read the whole file into the read buffer.
    length = 0;
    while(length < bufsize && 0 < (bytes_read = read(fd, (buf+length), (bufsize-length)-1))) {
        length += bytes_read;
    }

    tokcount = tokenize(buf, bufsize, "= \t:;\n", &tokbufsize);
    tokbuf = buf; // sugar. 

    // create an array of pointers to the individual tokens in the buffer.
    size_t argvsize = (tokcount + 1) * sizeof(char*);
    char** argv = malloc(argvsize);
    if (!argv) {
        goto out_free_buf;
    }
    memset(argv, 0, argvsize);


    int argc = init_argv_from_tokenbuf(argv, tokbuf, tokbufsize, tokcount);
    if (argc != tokcount) {
        // failed to extract argv?
        goto out_free_argv;
    }


    // allocate a config object.
    config = malloc(sizeof(struct Config));
    if (!config) {
        goto out_free_argv;
    }
    memset(config, 0, sizeof(struct Config));



    // iterate over the tokens and pick off key-value pairs

    char** token;
    token = argv;
    while (token && *token) {
        if (0 == strncmp(*token, "period", 5)) {
            ++token;
            if (token && *token) {
                int period = strtol(*token, 0, 0);
                setPeriod(config, period);
                ++token;
            }
        } else if (0 == strncmp(*token, "State", 5)) {
            ++token;
            if (token && *token) {
                setStateFilename(config, *token);
                ++token;
            }
        } else if (0 == strncmp(*token, "Detect", 5)) {
            ++token;
            if (token && *token) {
                setDetectURL(config, *token);
                ++token;
            }
        } else if (0 == strncmp(*token, "Update", 5)) {
            ++token;
            if (token && *token) {
                setUpdateURL(config, *token);
                ++token;
            }
        } else if (0 == strncmp(*token, "Hostname", 5)) {
            ++token;
            if (token && *token) {
                setHostname(config, *token);
                ++token;
            }
        } else if (0 == strncmp(*token, "Username", 5)) {
            ++token;
            if (token && *token) {
                setUsername(config, *token);
                ++token;
            }
        } else if (0 == strncmp(*token, "Password", 5)) {
            ++token;
            if (token && *token) {
                setPassword(config, *token);
                ++token;
            }        
        } else {
            // unrecognized key word. garbage?
            ++token;
        }
    } // while 


out_free_argv:
    if (argv) {
        memset(argv, 0, argvsize);
        free(argv);
    }

out_free_buf:
    if (buf) {
        memset(buf, 0, bufsize);
        free(buf);
    }

out_final:
    return config;
}


// --------------------------------------------------------------------------
int writeConfig(const struct Config* config, int fd)
{
    int rc = -1;
    return rc;
}


// --------------------------------------------------------------------------
int getPeriod(const struct Config* config)
{
    int rc = -1;
    if (config) {
        rc = config->period;
    }
    return rc;
}

// --------------------------------------------------------------------------
const char* getStateFilename(struct Config* config)
{
    return (config ? config->stateFilename : 0);
}

// --------------------------------------------------------------------------
const char* getDetectURL(struct Config* config)
{
    return (config ? config->detectURL : 0);
}

// --------------------------------------------------------------------------
const char* getUpdateURL(struct Config* config)
{
    return (config ? config->updateURL : 0);
}

// --------------------------------------------------------------------------
const char* getHostname(struct Config* config)
{
    return (config ? config->hostname : 0);
}

// --------------------------------------------------------------------------
const char* getUsername(struct Config* config)
{
    return (config ? config->username : 0);
}

// --------------------------------------------------------------------------
const char* getPassword(struct Config* config)
{
    return (config ? config->password : 0);
}


// --------------------------------------------------------------------------
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




// TODO:
// GOOD Grief!!! 
// Factor out the common function !!!



// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
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

// --------------------------------------------------------------------------
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



