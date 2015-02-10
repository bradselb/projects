/* compile with the following command line...

$ gcc -o parse_nmea -D_BSD_SOURCE -Wall -O2 -std=c89 parse_nmea.c

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> /* toupper() */

int parse_date_time(const char* s, int* first, int* mid, int* last);
int parse_lat_lon(const char* s, const char* nsew, float* degrees);

int main(int argc, char* argv[])
{
    FILE* file;
    int arg_index;
    char* buf = 0;
    const unsigned int bufsize = 256;
    const char* delims = ",";
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
            char* empty = " ";
            char* p = &buf[0];
            while (0 != (token = strsep(&p, delims))) {
                if (0 == *token) {
                    token = empty;
                }
                tokens[token_index] = token;
                ++token_index;
            }

            key = tokens[0];

            if (0 == strncmp(key, "$GPRMC", 6) && tokens[2] && 'A' == *tokens[2]) {
                int day, month, year, hour, minute, second;
                float lat, lon, speed, course;

                parse_date_time(tokens[1], &hour, &minute, &second);
                parse_date_time(tokens[9], &day, &month, &year);
                year += 2000;

                parse_lat_lon(tokens[3], tokens[4], &lat);
                parse_lat_lon(tokens[5], tokens[6], &lon);

                speed = 1.852 * atof(tokens[7]); /* convert knots to Km/Hr */
                course = atof(tokens[8]);

                fprintf(stdout, "GPRMC  %02d-%02d-%d  %02d:%02d:%02d   %f , %f  %.1f (Km/Hr), %.1f (degrees)\n",
                        day, month, year, hour, minute, second, lat, lon, speed, course);
/*
                fprintf(stdout, "GPRMC  %02d-%02d-%d, %02d:%02d:%02d, %f%s, %f%s\n",
                        day, month, year, hour, minute, second, lat, tokens[4], lon, tokens[6]);
*/
            } else 
            if (0 == strncmp(key, "$GPGGA", 6) && tokens[6] && '0' != *tokens[6] && ' ' != *tokens[6]) {
                int hour, minute, second;
                int fix_quality, satellites;
                float lat, lon, hdop, altitude, geoid_height;
                
                parse_date_time(tokens[1], &hour, &minute, &second);
                parse_lat_lon(tokens[2], tokens[3], &lat);
                parse_lat_lon(tokens[4], tokens[5], &lon);

                fix_quality = atoi(tokens[6]);
                satellites = atoi(tokens[7]);
                hdop = atof(tokens[8]);
                altitude = atof(tokens[9]);
                /* tokens[10] is units of altitude, always? M for meters? */
                geoid_height = atof(tokens[11]);
                /* tokens[12] is units of height, always? M for meters? */

                fprintf(stdout, "GPGGA              %02d:%02d:%02d   %f , %f   %d, %2d, %.1f, %.1f%s, %.1f%s \n",
                        hour, minute, second, lat, lon, fix_quality, satellites, hdop, altitude,tokens[10], geoid_height, tokens[12]);
            } else 
            if (0 == strncmp(key+3, "GNS",3) && tokens[6] && 'N' != *tokens[6] && ' ' != *tokens[6]) {
                int hour, minute, second;
                char* fix_quality;
                int satellites;
                float lat, lon, hdop, altitude, geoid_height;
                
                parse_date_time(tokens[1], &hour, &minute, &second);
                parse_lat_lon(tokens[2], tokens[3], &lat);
                parse_lat_lon(tokens[4], tokens[5], &lon);
                fix_quality = tokens[6];
                satellites = atoi(tokens[7]);
                hdop = atof(tokens[8]);
                altitude = atof(tokens[9]);
                geoid_height = atof(tokens[10]);

                fprintf(stdout, "%s              %02d:%02d:%02d   %f , %f  %s, %2d, %.1f, %.1f , %.1f\n",
                        (key+1), hour, minute, second, lat, lon, fix_quality, satellites, hdop, altitude, geoid_height);
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

/* s is either time in the form hhmmss.s   */
/* or, the date in the form DDMMYY . Century is assumed to be 2000   */
int parse_date_time(const char* s, int* first, int* mid, int* last)
{
    int rc;
    char buf[8];
    int buflen = sizeof buf / sizeof buf[0];
    int val;
    char* p;

    if (!s || strlen(s) < 6) {
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

/* s is the ddmm.mmmmmm string, */
/* nsew is one of "N", "S", "E", "W" */
int parse_lat_lon(const char* s, const char* nsew, float* degrees)
{
    int rc;
    char buf[16];
    int buflen = sizeof buf / sizeof buf[0];
    char* p;
    int dp_index;
    float val;

    rc = -1;
    if (!s || !nsew || strlen(s) < 6) {
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

    /* North and East are positive. South and West are negative */
    if ('S' == toupper(*nsew) || 'W' == toupper(*nsew)) {
        val = -1.0 * val;
    }

    if (degrees) *degrees = val;


    rc = 0;

exit:
    return rc;
}

