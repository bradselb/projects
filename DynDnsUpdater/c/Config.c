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
   char* detectURL; // where to discover my current IP 
   char* updateURL; // where to update my DynDNS entry
   char* hostname;  // my hostname 
   char* username;  // my DynDNS user name
   char* password;  // my DynDNS password
};

// --------------------------------------------------------------------------
static const int DefaultPeriod = 28; // days
static const char* DefaultStateFilename = "/var/tmp/DynDnsUpdaterState.dat";
static const char* DefaultDetectURL = "http://checkip.dyndns.org";
static const char* DefaultUpdateURL = "http://members.dyndns.org/nic/update";
//static const char* DefaultHostname = "test.dyndns.org";
static const char* DefaultHostname = "test.ath.cx";
static const char* DefaultUserName = "test";
static const char* DefaultPassword = "test";

//hostname("test.dnsalias.net"),
//hostname("test.dnsalias.org"),
//hostname("test.dnsalias.com"),
//hostname("test.homeip.net"),

// --------------------------------------------------------------------------
#define MAX_STRING_LENGTH 511

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
        setPeriod(config, DefaultPeriod);
        setStateFilename(config, DefaultStateFilename);
        setDetectURL(config, DefaultDetectURL);
        setUpdateURL(config, DefaultUpdateURL);
        setHostname(config, DefaultHostname);
        setUsername(config, DefaultUserName);
        setPassword(config, DefaultPassword);
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
// split the content read from the file into a list of individual words
// iterate over the list of words read from the file and pick off key-value pairs
static int parse_config_buf(struct Config* config, const char* buf)
{
    int rc = 0;
    struct slist* head = 0; // a string list.

    head = slist_alloc();
    if (!head) {
        rc = -1;
        goto out;
    }

    int count;
    count = slist_split(head, buf, " \t\n");

    struct slist* node; // a node in the list.
    node =  slist_next(head);
    while (node != head) {
        const char* token;
        token = slist_string(node);
        if (0 == strncmp(token, "period", 5)) {
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
        } else if (0 == strncmp(token, "Detect", 5)) {
            node = slist_next(node);
            token = slist_string(node);
            if (token) {
                setDetectURL(config, token);
                node = slist_next(node);
            }
        } else if (0 == strncmp(token, "Update", 5)) {
            node = slist_next(node);
            token = slist_string(node);
            if (token) {
                setUpdateURL(config, token);
                node = slist_next(node);
            }
        } else if (0 == strncmp(token, "Hostname", 5)) {
            node = slist_next(node);
            token = slist_string(node);
            if (token) {
                setHostname(config, token);
                node = slist_next(node);
            }
        } else if (0 == strncmp(token, "Username", 5)) {
            node = slist_next(node);
            token = slist_string(node);
            if (token) {
                setUsername(config, token);
                node = slist_next(node);
            }
        } else if (0 == strncmp(token, "Password", 5)) {
            node = slist_next(node);
            token = slist_string(node);
            if (token) {
                setPassword(config, token);
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
                          "Detect-URL %s\n"
                          "Update-URL %s\n"
                          "Hostname %s\n"
                          "Username %s\n"
                          "Password %s\n",
                          config->period,
                          config->stateFilename,
                          config->detectURL,
                          config->updateURL,
                          config->hostname,
                          config->username,
                          config->password
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
const char* getDetectURL(const struct Config* config)
{
    return (config ? config->detectURL : 0);
}

// --------------------------------------------------------------------------
const char* getUpdateURL(const struct Config* config)
{
    return (config ? config->updateURL : 0);
}

// --------------------------------------------------------------------------
const char* getHostname(const struct Config* config)
{
    return (config ? config->hostname : 0);
}

// --------------------------------------------------------------------------
const char* getUsername(const struct Config* config)
{
    return (config ? config->username : 0);
}

// --------------------------------------------------------------------------
const char* getPassword(const struct Config* config)
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
int setDetectURL(struct Config* config, const char* src)
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
    } else if (!(buf = malloc(length + 1))) {
        rc = -1;
    } else {
        strncpy(buf, src, length+1);
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
int setUsername(struct Config* config, const char* src)
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
        if (config->username) {
            memset(config->username, 0, strlen(config->username));
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
    } else if (!(buf = malloc(length + 1))) {
        rc = -1;
    } else {
        strncpy(buf, src, length+1);
        if (config->password) {
            memset(config->password, 0, strlen(config->password));
            free(config->password);
        }
        config->password = buf;
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
        0 != strcmp(getDetectURL(cfg0), getDetectURL(cfg1)) ||
        0 != strcmp(getUpdateURL(cfg0), getUpdateURL(cfg1)) ||
        0 != strcmp(getHostname(cfg0), getHostname(cfg1)) ||
        0 != strcmp(getUsername(cfg0), getUsername(cfg1)) ||
        0 != strcmp(getPassword(cfg0), getPassword(cfg1)))
    {
        rc = 1; // the two are different
    }

    else {
        rc = 0; // they are the same.
    }

    return rc;
}


