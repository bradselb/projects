#include "params.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main(int argc, char** argv)
{
    int index;

    index = get_cmdline_options(argc, argv);
    print_options();

    printf("additional command line parameters:\n");
    for (int i=index; i<argc; ++i) {
        printf("%s\n", argv[i]);
    }

    return 0;
}

// sample runtime...
#if 0
$ 
$ gcc -o test_params -Wall -O2 -std=c99 -I.. test_params.c ../params.c
$ ./test_params --config test.conf -l test.log --version -h --debug frogs eat flies -ptest.pid

debug: 1, help: 1, version: 1
config file: 'test.conf'
log file: 'test.log'
pid file: 'test.pid'

additional command line parameters:
frogs
eat
flies
$ 
#endif 

