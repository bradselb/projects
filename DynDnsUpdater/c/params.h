#if !defined DYNDNSUPDATER_PARAMS_H
#define DYNDNSUPDATER_PARAMS_H

// an object to represent the command line parameter options.


struct params
{
    int debug;
    int help;
    int version;
    const char* cfgfile;
    const char* logfile;
    const char* pidfile;
};

extern const struct params* const params;

int get_cmdline_options(int argc, char** argv);
int print_help(void);
int print_options(void);


#endif // !defined DYNDNSUPDATER_PARAMS_H
