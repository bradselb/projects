#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_date_time(const char* s, int* first, int* mid, int* last);
int parse_lat_lon(const char* s, float* degrees);

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
    char* tokens[32];

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
                int day, month, year, hour, minute, second;
                float lat, lon;

                parse_date_time(tokens[1], &hour, &minute, &second);
                parse_date_time(tokens[9], &day, &month, &year);
                year += 2000;

                parse_lat_lon(tokens[3], &lat);
                parse_lat_lon(tokens[5], &lon);
/*
                fprintf(stdout, "%02d-%02d-%d %02d:%02d:%02d %f %s, %f %s  (speed: %s, heading: %s)\n",
                        day, month, year, hour, minute, second, lat, tokens[4], lon, tokens[6], tokens[7], tokens[8]);
*/
                fprintf(stdout, "%02d-%02d-%d, %02d:%02d:%02d, %f%s, %f%s\n",
                        day, month, year, hour, minute, second, lat, tokens[4], lon, tokens[6]);
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


int parse_date_time(const char* s, int* first, int* mid, int* last)
{
    int rc;
    char buf[8];
    int buflen = sizeof buf / sizeof buf[0];
    int val;
    char* p;

    if (!s) {
        rc = -1;
        goto exit;
    }

    buf[buflen-1] = 0;
    strncpy(buf, s, buflen-1);

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

    rc = 0;

exit:
    return rc;
}


int parse_lat_lon(const char* s, float* degrees)
{
    int rc;
    char buf[16];
    int buflen = sizeof buf / sizeof buf[0];
    char* p;
    int dp_index;
    float val;

    rc = -1;
    if (!s) {
        goto exit;
    }

    buf[buflen-1] = 0;
    strncpy(buf, s, buflen-1);

    /* find the decimal point */
    dp_index = strcspn(buf, ".");
    if (dp_index < 2) {
        goto exit;
    }

    /* point to the minutes part */
    p = buf + (dp_index - 2);
    val = atof(p);
    *p = 0;
    /* convert minutes to decimal degrees and add full degrees */
    val = (val/60.0) + atof(buf);

    if (degrees) *degrees = val;


    rc = 0;

exit:
    return rc;
}

