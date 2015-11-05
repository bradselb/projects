/* compile with the following command line...

$ gcc -o parse_nmea -D_BSD_SOURCE -Wall -O2 -std=c89 parse_nmea.c

*/
#include "nmea.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> /* toupper() */


int main(int argc, char* argv[])
{
    FILE* file;
    int arg_index;
    char* buf = 0;
    const unsigned int bufsize = 256;
    const char* key;
    const char* tokens[32];

    buf = malloc(bufsize);
    if (!buf) {
        goto exit;
    }

    for (arg_index=1; arg_index<argc; ++arg_index) {
        file = fopen(argv[arg_index], "r");
        if (!file) {
            continue;
        }

        while (fgets(buf, bufsize, file)) {
            if (strlen(buf) < 10) continue;
            int len = sizeof tokens / sizeof tokens[0];
            int token_count =  tokenize(buf, tokens, len);

            key = tokens[0];
            if (0 == strncmp(key+3, "RMC", 3) && tokens[2] && 'A' == *tokens[2]) {
                struct RMC* pRMC = parse_rmc(tokens, token_count);
                if (pRMC) free(pRMC);

            } else 
            if (0 == strncmp(key+3, "GGA", 3) && tokens[6] && '0' != *tokens[6] && ' ' != *tokens[6]) {
                struct GGA* pGGA = parse_gga(tokens, token_count);
                if (pGGA) free(pGGA);
            }
        }

        fclose(file);
    }

exit: 
    if (buf) {
        free(buf);
    }
    return 0;
}


