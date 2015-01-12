#include "Config.h"
#include "slist.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> // read(), write()

// --------------------------------------------------------------------------
struct Config
{
   int period; // force an update every <period> days
   char* stateFilename; // where is the state file? 
   char* detectHostname; // host to help me discover my current IP 
   char* detectResource; // http resource to discover my current IP 
   char* updateHostname; // where to update my DynDNS entry
   char* updateResource; // where to update my DynDNS entry
   char* hostname;  // my hostname 
   char* authorization;  // my DynDNS username:password base64 encoded
};

// --------------------------------------------------------------------------
static const int DefaultPeriod = 28; // days
static const char* DefaultStateFilename = "/var/tmp/DynDnsUpdaterState.dat";
static const char* DefaultDetectHostname = "checkip.dyndns.org";
static const char* DefaultDetectResource = "/";
static const char* DefaultUpdateHostname = "members.dyndns.org";
static const char* DefaultUpdateResource = "/nic/update";

static const char* DefaultHostname = "test.dyndns.org";
//static const char* DefaultHostname = "test.ath.cx";
//hostname("test.dnsalias.net"),
//hostname("test.dnsalias.org"),
//hostname("test.dnsalias.com"),
//hostname("test.homeip.net"),

// this is what you get if you do the bash shell one liner (example below)..it does not work.
//static const char* DefaultAuthorization = "dGVzdDp0ZXN0Cg=="; // == base64("test:test")
static const char* DefaultAuthorization = "dGVzdDp0ZXN0"; // == base64("test:test")

// how to get the authorizaton string...

// in tcl:
// package require base64
// set username {test}
// set password {test}
// set token [base64::encode "$username:$password"]
// puts "$token"

// in python:
// import base64
// username = r'test'
// password = r'test'
// print(base64.b64encode("%s:%s" %(username, password)))

// this might also work...there is an extra '\n' character
// bash command line...
// $ echo "test:test" | base64 -

// --------------------------------------------------------------------------
#define MAX_STRING_LENGTH 511

// maximum size of buffer used to read/write the whole config file. The magic
// number 8 comes from there being eight elements, eight lines in the file.
// Each line maybe not be any longer than the max string length. Line that are 
// long will get truncated and things will likely, not work as expected. 
#define MAX_BUF_SIZE  (8 * (1 + MAX_STRING_LENGTH))



// --------------------------------------------------------------------------
struct Config* createDefaultConfig(void)
{
    struct Config* config;
    config = malloc(sizeof(struct Config));
    if (config) {
        memset(config, 0, sizeof(struct Config));
        setPeriod(config, DefaultPeriod);
        setStateFilename(config, DefaultStateFilename);
        setDetectHostname(config, DefaultDetectHostname);
        setDetectResource(config, DefaultDetectResource);
        setUpdateHostname(config, DefaultUpdateHostname);
        setUpdateResource(config, DefaultUpdateResource);
        setHostname(config, DefaultHostname);
        setAuthorization(config, DefaultAuthorization);
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
        deleteConfigString(config->detectHostname);
        deleteConfigString(config->detectResource);
        deleteConfigString(config->updateHostname);
        deleteConfigString(config->updateResource);
        deleteConfigString(config->hostname);
        deleteConfigString(config->authorization);
        memset(config, 0, sizeof(struct Config));
        free(config);
    }
}


// --------------------------------------------------------------------------
struct Config* loadConfig(const char* filename)
{
    struct Config* config = 0;
    int fd;

    if (!filename) {
        // do nothing.
    } else if ((fd = open(filename, O_RDONLY)) < 0) {
        // file open failed.
        fprintf(stderr, "'%s' - %s\n", filename, strerror(errno));
    } else {
        config = readConfig(fd);
        close(fd);
    }

    return config;
}

// --------------------------------------------------------------------------
int saveConfig(const struct Config* config, const char* filename)
{
   int rc = -1;
   FILE* file = 0;

   if ( !config || !filename ) {
      rc = -1;
   }
   else if ( 0 == (file = fopen(filename, "w")) ) {
      rc = -1;
   }
   else {
      fprintf(file, "period %d\n", config->period);
      fprintf(file, "State-Filename %s\n", config->stateFilename);
      fprintf(file, "Detect-Hostname %s\n", config->detectHostname);
      fprintf(file, "Detect-Resource %s\n", config->detectResource);
      fprintf(file, "Update-Hostname %s\n", config->updateHostname);
      fprintf(file, "Update-Resource %s\n", config->updateResource);
      fprintf(file, "Hostname %s\n", config->hostname);
      fprintf(file, "Authorization-Token %s\n", config->authorization);
      fclose(file);
      rc = 0;
   }
   return rc;
}


// --------------------------------------------------------------------------
// given a buffer, assume the content is  key-value word pairs. Split the
// content in the buffer into discrete words and iterate over the list of 
// words and pick off specific key-value pairs. Fill the supplied Config 
// object with the values so found in the buffer contents.
static int parse_config_buf(struct Config* config, const char* buf)
{
    int rc = 0;
    struct slist* head = 0; // a string list.

    head = slist_alloc();
    if (!head) {
        rc = -1;
        goto out;
    }

    //int count;
    //count = 
    slist_split(head, buf, " \t\n");

    struct slist* node; // a node in the list.
    node =  slist_next(head);
    while (node != head) {
        const char* token;
        token = slist_string(node);
        if (0 == strncmp(token, "period", 6)) {
            node = slist_next(node);
            token = slist_string(node);
            if (token) {
                int period = strtol(token, 0, 0);
                setPeriod(config, period);
                node = slist_next(node);
            }
        } else if (0 == strncmp(token, "State", 5)) {
            node = slist_next(node);
            token = slist_string(node);
            if (token) {
                setStateFilename(config, token);
                node = slist_next(node);
            }
        } else if (0 == strncmp(token, "Detect-Hostname", 10)) {
            node = slist_next(node);
            token = slist_string(node);
            if (token) {
                setDetectHostname(config, token);
                node = slist_next(node);
            }
        } else if (0 == strncmp(token, "Detect-Resource", 10)) {
            node = slist_next(node);
            token = slist_string(node);
            if (token) {
                setDetectResource(config, token);
                node = slist_next(node);
            }
        } else if (0 == strncmp(token, "Update-Hostname", 10)) {
            node = slist_next(node);
            token = slist_string(node);
            if (token) {
                setUpdateHostname(config, token);
                node = slist_next(node);
            }
        } else if (0 == strncmp(token, "Update-Resource", 10)) {
            node = slist_next(node);
            token = slist_string(node);
            if (token) {
                setUpdateResource(config, token);
                node = slist_next(node);
            }
        } else if (0 == strncmp(token, "Hostname", 8)) {
            node = slist_next(node);
            token = slist_string(node);
            if (token) {
                setHostname(config, token);
                node = slist_next(node);
            }
        } else if (0 == strncmp(token, "Authorization", 4)) {
            node = slist_next(node);
            token = slist_string(node);
            if (token) {
                setAuthorization(config, token);
                node = slist_next(node);
            }
        } else {
            // ignore unrecognized key word.
            node = slist_next(node);
        }
    } // while 

out:
    slist_free(head);

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
    char* buf = 0; // a read buffer
    size_t bufsize; // allocated size

    // allocate a read buffer
    bufsize = MAX_BUF_SIZE;
    buf = malloc(bufsize);
    if (!buf) {
        goto out;
    }
    memset(buf, 0, bufsize);

    // read the whole file into the read buffer.
    // size_t fd_read(int fd, void* buf, size_t bufsize) // returns length.
    {
    ssize_t bytes_read; // how many bytes read this time
    size_t length; // how many bytes are in the bufffer
    length = 0;
    while(length < bufsize && 0 < (bytes_read = read(fd, (buf+length), (bufsize-length)-1))) {
        length += bytes_read;
    }
    }

    // allocate the config object that this fctn constructs
    config = malloc(sizeof(struct Config));
    if (!config) {
        goto out;
    }
    memset(config, 0, sizeof(struct Config));

    // initialize the config object from the buffer contents.
    int ec;
    ec = parse_config_buf(config, buf);
    if (ec) {
        free(config);
        config = 0;
    }

out:
    if (buf) {
        memset(buf, 0, bufsize);
        free(buf);
    }

    return config;
}


// --------------------------------------------------------------------------
// returns number of bytes written on success less than zero otherwise.
int writeConfig(const struct Config* config, int fd)
{
    int rc;
    int byte_count;
    int bufsize;
    char* buf;

    buf = 0;

    if (!config) {
        rc = -1;
        goto out_final;
    }

    bufsize = MAX_BUF_SIZE; // output should NEVER be truncated.
    buf = malloc(bufsize);
    if (!buf) {
        rc = -2;
        goto out_final;
    }
    memset(buf, 0, bufsize);


    // the compiler will catenate all these adjacent strings into one long string.
    byte_count = snprintf(buf, bufsize,
                          "period %d\n"
                          "State-Filename %s\n"
                          "Detect-Hostname %s\n"
                          "Detect-Resource %s\n"
                          "Update-Hostname %s\n"
                          "Update-Resource %s\n"
                          "Hostname %s\n"
                          "Authorization %s\n",
                          config->period,
                          config->stateFilename,
                          config->detectHostname,
                          config->detectResource,
                          config->updateHostname,
                          config->updateResource,
                          config->hostname,
                          config->authorization
                         );

    if ((byte_count < 0) || (!(byte_count<bufsize))) {
        // an error or the output was truncated.
        rc = -3;
        goto out_free_buf;
    } 
    
    rc = write(fd, buf, byte_count+1);
    if (rc < 0) {
        // write failed.
        fprintf(stderr, "(%s:%d) %s() - %s\n", __FILE__, __LINE__, __FUNCTION__, strerror(errno));
        goto out_free_buf;
    }



out_free_buf:
    if (buf) {
        memset(buf, 0, bufsize);
        free(buf);
    }

out_final:
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
const char* getStateFilename(const struct Config* config)
{
    return (config ? config->stateFilename : 0);
}

// --------------------------------------------------------------------------
const char* getDetectHostname(const struct Config* config)
{
    return (config ? config->detectHostname : 0);
}

// --------------------------------------------------------------------------
const char* getDetectResource(const struct Config* config)
{
    return (config ? config->detectResource : 0);
}

// --------------------------------------------------------------------------
const char* getUpdateHostname(const struct Config* config)
{
    return (config ? config->updateHostname : 0);
}

// --------------------------------------------------------------------------
const char* getUpdateResource(const struct Config* config)
{
    return (config ? config->updateResource : 0);
}

// --------------------------------------------------------------------------
const char* getHostname(const struct Config* config)
{
    return (config ? config->hostname : 0);
}

// --------------------------------------------------------------------------
const char* getAuthorization(const struct Config* config)
{
    return (config ? config->authorization : 0);
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
        config->period = DefaultPeriod;
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
        src = DefaultStateFilename;
    }

    length = strnlen(src, MAX_STRING_LENGTH);

    if (!config) {
        rc = -1;
    } else if (!(buf = malloc(length + 1))) {
        rc = -1;
    } else {
        strncpy(buf, src, length+1);
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
int setDetectHostname(struct Config* config, const char* src)
{
    int rc;
    size_t length;
    char* buf;

    length = strnlen(src, MAX_STRING_LENGTH);

    if (!config || !src) {
        rc = -1;
    } else if (!(buf = malloc(length + 1))) {
        rc = -1;
    } else {
        strncpy(buf, src, length+1);
        if (config->detectHostname) {
            free(config->detectHostname);
        }
        config->detectHostname = buf;
        // now the config object owns the buffer
        rc = 0;
    }
    return rc;
}

// --------------------------------------------------------------------------
int setDetectResource(struct Config* config, const char* src)
{
    int rc;
    size_t length;
    char* buf;

    length = strnlen(src, MAX_STRING_LENGTH);

    if (!config || !src) {
        rc = -1;
    } else if (!(buf = malloc(length + 1))) {
        rc = -1;
    } else {
        strncpy(buf, src, length+1);
        if (config->detectResource) {
            free(config->detectResource);
        }
        config->detectResource = buf;
        // now the config object owns the buffer
        rc = 0;
    }
    return rc;
}

// --------------------------------------------------------------------------
int setUpdateHostname(struct Config* config, const char* src)
{
    int rc;
    size_t length;
    char* buf;

    length = strnlen(src, MAX_STRING_LENGTH);

    if (!config || !src) {
        rc = -1;
    } else if (!(buf = malloc(length + 1))) {
        rc = -1;
    } else {
        strncpy(buf, src, length+1);
        if (config->updateHostname) {
            free(config->updateHostname);
        }
        config->updateHostname = buf;
        // now the config object owns the buffer
        rc = 0;
    }
    return rc;
}

// --------------------------------------------------------------------------
int setUpdateResource(struct Config* config, const char* src)
{
    int rc;
    size_t length;
    char* buf;

    length = strnlen(src, MAX_STRING_LENGTH);

    if (!config || !src) {
        rc = -1;
    } else if (!(buf = malloc(length + 1))) {
        rc = -1;
    } else {
        strncpy(buf, src, length+1);
        if (config->updateResource) {
            free(config->updateResource);
        }
        config->updateResource = buf;
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
    } else if (!(buf = malloc(length + 1))) {
        rc = -1;
    } else {
        strncpy(buf, src, length+1);
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
int setAuthorization(struct Config* config, const char* src)
{
    int rc;
    size_t length;
    char* buf;

    length = strnlen(src, MAX_STRING_LENGTH);

    if (!config || !src) {
        rc = -1;
    } else if (!(buf = malloc(length + 1))) {
        rc = -1;
    } else {
        strncpy(buf, src, length+1);
        if (config->authorization) {
            memset(config->authorization, 0, strlen(config->authorization));
            free(config->authorization);
        }
        config->authorization = buf;
        // now the config object owns the buffer
        rc = 0;
    }
    return rc;
}

// --------------------------------------------------------------------------
int compareConfig(const struct Config* cfg0, const struct Config* cfg1)
{
    int rc;

    if (!cfg0 || !cfg1)
    {
        rc = -1; // not able to compare the two.
    }

    else if (getPeriod(cfg0) != getPeriod(cfg1) ||
        0 != strcmp(getStateFilename(cfg0), getStateFilename(cfg1)) ||
        0 != strcmp(getDetectHostname(cfg0), getDetectHostname(cfg1)) ||
        0 != strcmp(getDetectResource(cfg0), getDetectResource(cfg1)) ||
        0 != strcmp(getUpdateHostname(cfg0), getUpdateHostname(cfg1)) ||
        0 != strcmp(getUpdateResource(cfg0), getUpdateResource(cfg1)) ||
        0 != strcmp(getHostname(cfg0), getHostname(cfg1)) ||
        0 != strcmp(getAuthorization(cfg0), getAuthorization(cfg1))
        )
    {
        rc = 1; // the two are different
    }

    else {
        rc = 0; // they are the same.
    }

    return rc;
}


