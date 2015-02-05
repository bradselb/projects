#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_date_time(const char* s, int* first, int* mid, int* last);

int main(int argc, char* argv[])
{
    FILE* file;
    int arg_index;
    char* buf = 0;
    const unsigned int bufsize = 256;
    const char* delims = ",$*\r\n";
    char* token;
    char* key;
    int token_index;
    char* tokens[16];

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
            token_index = 0;
            memset(tokens, 0, sizeof tokens);
            token = strtok(buf, delims);
            while (token) {
                tokens[token_index] = token;
                ++token_index;
                token = strtok(0, delims);
            }
            key = tokens[0];
            if (0 == strncmp(key, "GPRMC", 5) && 'A' == tokens[2][0]) {

                fprintf(stdout, "Date:%s, Time:%s, lat: %s%s, lon: %s%s, Heading: %s, speed: %s\n",
                        tokens[9], tokens[1], tokens[3],tokens[4], tokens[5], tokens[6], tokens[8], tokens[7]);
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


/* strategy */
int parse_date_time(const char* s, int* first, int* mid, int* last)
{
    int rc;
    char buf[32];
    int buflen = sizeof buf / sizeof buf[0];
    int val;
    char* p;


    rc = 0;
    memset(buf, 0, sizeof buf);
    strncpy(buf, s, buflen);

    /* extract year or seconds from last position*/
    p = buf + 4;
    val = atoi(p);
    if (last) *last = val;
    *p = 0;

    /* month or minutes*/
    p = buf + 2;
    val = atoi(p);
    if (mid) *mid = val;
    *p = 0;

    /* day or hour in first position*/
    p = buf + 0;
    val = atoi(p);
    if (first) *first = val;

    return rc;
}


