#include "Config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

// this works.
//gcc -Wall -O2 -std=gnu99 -o testConfig -I../ -I../../../lib/ -L../../../lib/ testConfig.c ../Config.c  -lbrad


static const char* CONFIG_FILE_PATH = "./DynDnsUpdater.conf";


int main(int argc, char** argv)
{
    int rc = 0;
    //int test_count;
    struct Config* cfg0 = 0;
    struct Config* cfg1 = 0;
    int fd;

    cfg0 = createDefaultConfig();

    // verify that compare is reflexive.
    if (0 != compareConfig(cfg0, cfg0)) {
        fprintf(stderr, "FAIL - compareConfig is reflexive\n");
        rc = 1; // first test failed
    }

    // write the default config out to a disk file.
    rc = saveConfig(cfg0, CONFIG_FILE_PATH);

    fd = open(CONFIG_FILE_PATH, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "FAIL - open file '%s'\n", CONFIG_FILE_PATH);
        perror(0);
        rc = -1;
        goto out;
    }

    // read the previously saved default config back into a new config object
    cfg1 = readConfig(fd);
    close(fd);
    if (!cfg1) {
        fprintf(stderr, "FAIL - readConfig()\n");
        rc = -2;
        goto out;
    }

    // compare what was written to what was read
    if (0 != compareConfig(cfg0, cfg1)) {
        fprintf(stderr, "FAIL - default persistence test\n");
        rc = 2; // second test failed
        goto out; // not really much point in continuing.
    }
    deleteConfig(cfg1); 


    cfg1 = loadConfig(CONFIG_FILE_PATH);
    if (0 != compareConfig(cfg0, cfg1)) {
        fprintf(stderr, "FAIL - loadConfig()\n");
        rc = 3;
        goto out; // not really much point in continuing.
    }
 
    if (0 != compareConfig(cfg0, cfg1) || 0 != compareConfig(cfg1, cfg0)) {
        fprintf(stderr, "FAIL - compare config is symmetric\n");
        rc = 4;
    }
    
    // change period in one config and verify that the two configs are different.
    setPeriod(cfg1, 1+getPeriod(cfg0));
    if (1 != compareConfig(cfg0, cfg1)) {
        fprintf(stderr, "FAIL - setPeriod() or getPeriod()\n");
        rc = 5;
    }
    setPeriod(cfg1, getPeriod(cfg0));


    setStateFilename(cfg1, "this_is_garbage_sdfjkhqfuiwrhquiorhroghorfg");
    if (1 != compareConfig(cfg0, cfg1)) {
        fprintf(stderr, "FAIL - setStateFilename()\n");
        rc = 6; // failed test number
    }
    setStateFilename(cfg1, getStateFilename(cfg0));


    setPassword(cfg1, "this_is_garbage_sdfjkhqfuiwrhquiorhroghorfg");
    if (1 != compareConfig(cfg0, cfg1)) {
        fprintf(stderr, "FAIL - setPassword()\n");
        rc = 7; // failed test number
    }
    setPassword(cfg1, getPassword(cfg0));


    setUsername(cfg1, "this_is_garbage_sdfjkhqfuiwrhquiorhroghorfg");
    if (1 != compareConfig(cfg0, cfg1)) {
        fprintf(stderr, "FAIL - setUsername()\n");
        rc = 8; // failed test number
    }
    setUsername(cfg1, getUsername(cfg0));


    setHostname(cfg1, "this_is_garbage_sdfjkhqfuiwrhquiorhroghorfg");
    if (1 != compareConfig(cfg0, cfg1)) {
        fprintf(stderr, "FAIL - setHostname()\n");
        rc = 9; // failed test number
    }
    setHostname(cfg1, getHostname(cfg0));


    setUpdateURL(cfg1, "https://update.this_is_garbage.com:8899/otherstuf.cgi?do=get&say=no");
    if (1 != compareConfig(cfg0, cfg1)) {
        fprintf(stderr, "FAIL - setUpdateURL()\n");
        rc = 10; // failed test number
    }
    setUpdateURL(cfg1, getUpdateURL(cfg0));


    setDetectURL(cfg1, "https://update.this_is_garbage.com:8899/otherstuf.cgi?do=get&say=no");
    if (1 != compareConfig(cfg0, cfg1)) {
        fprintf(stderr, "FAIL - setDetectURL()\n");
        rc = 11; // failed test number
    }
    setDetectURL(cfg1, getDetectURL(cfg0));


out:
    if (0 == rc) {
        fprintf(stderr, "PASS - Config unit test.\n");
    }

    deleteConfig(cfg1);
    deleteConfig(cfg0);

    return rc;
}
