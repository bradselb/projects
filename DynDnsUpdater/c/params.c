#include "params.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>



struct params g_params; // the commnad line parameters

struct params const * const params = &g_params; // how clients see/access the params. 

static const char* short_options = "dhVc:l:p:";
static struct option long_options[] = {
    {.val='d', .name="debug", .has_arg=no_argument, .flag=0},
    {.val='h', .name="help", .has_arg=no_argument, .flag=0},
    {.val='V', .name="version", .has_arg=no_argument, .flag=0},
    {.val='c', .name="config-file", .has_arg=required_argument, .flag=0},
    {.val='l', .name="log-file", .has_arg=required_argument, .flag=0},
    {.val='p', .name="pid-file", .has_arg=required_argument, .flag=0},
    {0}
};


int get_cmdline_options(int argc, char** argv)
{
    int idx;
    int ch;

    while (0 < (ch = getopt_long(argc, argv, short_options, long_options, 0))) {
        switch (ch) {
            case 'd':
                g_params.debug = 1;
                break;

            case 'h':
                g_params.help = 1;
                break;

            case 'V':
                g_params.version = 1;
                break;

            case 'c':
                g_params.cfgfile = optarg;
                break;

            case 'l':
                g_params.logfile = optarg;
                break;

            case 'p':
                g_params.pidfile = optarg;
                break;

            case '?':
                fprintf(stderr, "ignoring unknown cmdline option, '%c'\n", optopt);
                g_params.help = 1;
                break;

            default:
                break;

        }
    }
    idx = optind;
    return idx;
}


int print_help(void)
{
    int rc = 0;

    printf("\nBrad's Embedded DynDNS updater Client\n");
    printf("-h --help        print this help and exit.\n");
    printf("   --version     print version string and exit.\n");
    printf("-c --config-file <file-path> specify the path to the config file.\n\n");

    return rc;
}


int print_options(void)
{
    int rc = 0;

    printf("\n");
    printf("debug: %d, help: %d, version: %d\n", g_params.debug, g_params.help, g_params.version);
    printf("config file: '%s'\n", g_params.cfgfile);
    printf("log file: '%s'\n", g_params.logfile);
    printf("pid file: '%s'\n", g_params.pidfile);
    printf("\n");

    return rc;
}


