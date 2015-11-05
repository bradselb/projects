#include "nmea.h"

#define _BSD_SOURCE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> /* toupper() */

/* ---------------------------------------------------------------------- */
/* some helpful web sites: */
/* http://freenmea.net/docs */
/* http://www.trimble.com/OEM_ReceiverHelp */
int parse_date_time(const char* s, int* first, int* mid, int* last);
int parse_lat_lon(const char* s, const char* nsew, float* degrees);
int tokenize(char* s, const char* tokens[], int len);

/* ---------------------------------------------------------------------- */
struct GGA* parse_gga(const char* tokens[], int token_count)
{
    struct GGA* gga=0;
    int hour, minute, second;
    int fix_quality, sat_count;
    float lat, lon, hdop, altitude, geoid_height;
    const char* s = tokens[0];

    if (0 == strncmp(s+3, "GGA", 3) && 12 < token_count) {
        parse_date_time(tokens[1], &hour, &minute, &second);
        parse_lat_lon(tokens[2], tokens[3], &lat);
        parse_lat_lon(tokens[4], tokens[5], &lon);

        fix_quality = atoi(tokens[6]);
        sat_count = atoi(tokens[7]);
        hdop = atof(tokens[8]);
        altitude = atof(tokens[9]);
        /* tokens[10] is units of altitude, always? M for meters? */
        geoid_height = atof(tokens[11]);
        /* tokens[12] is units of height, always? M for meters? */
        /* tokens[13] is the time, in seconds, since the differential GPS data update */
        /* tokens[14] is the station ID of the differntial data source */
        /* tokens[15] is the checksum sans asterisk */


        fprintf(stdout, "GGA %02d:%02d:%02d %.6f , %.6f %d, %d, %.2f, %.1f(%s), %.1f(%s) \n",
                hour, minute, second, lat, lon, fix_quality, sat_count, hdop, altitude, tokens[10], geoid_height, tokens[12]);


        if (0 != (gga = malloc(sizeof(struct GGA)))) {
            memset(gga, 0, sizeof(struct GGA));
            gga->hour = hour;
            gga->min = minute;
            gga->sec = second;
            gga->lat = lat;
            gga->lon = lon;
            gga->quality = fix_quality;
            gga->sat_count = sat_count;
            gga->hdop = hdop;
            gga->altitude = altitude;
            gga->geoid_height = geoid_height;
        }
    }

    return gga;
}


/* ---------------------------------------------------------------------- */
struct GNS* parse_gns(char* sentence)
{
    struct GNS* gns = 0;
    /*
                int hour, minute, second;
                const char* fix_quality;
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
    */
    return gns;
}


/* ---------------------------------------------------------------------- */
struct GSA* parse_gsa(char* sentence)
{
    struct GSA* gsa = 0;
    return gsa;
}


/* ---------------------------------------------------------------------- */
struct GSV* parse_gsv(char* sentence)
{
    struct GSV* gsv = 0;
    return gsv;
}


/* ---------------------------------------------------------------------- */
struct RMC* parse_rmc(const char* tokens[], int token_count)
{
    struct RMC* rmc = 0;
    int hour, minute, second;
    int day, month, year;
    float lat, lon, speed, heading;
    char status; // A: active, V: void
    const char* s = tokens[0];

    if (0 == strncmp(s+3, "RMC", 3) && 9 < token_count) {
        parse_date_time(tokens[1], &hour, &minute, &second);
        status = *(tokens[2]);
        parse_lat_lon(tokens[3], tokens[4], &lat);
        parse_lat_lon(tokens[5], tokens[6], &lon);
        speed = atof(tokens[7]); /* knots */
        heading = atof(tokens[8]);
        parse_date_time(tokens[9], &day, &month, &year);
        year += 2000;

        fprintf(stdout, "RMC %02d:%02d:%02d %.6f , %.6f  %.1f(knots), %.1f(degrees) %02d-%02d-%d\n",
                hour, minute, second, lat, lon, speed, heading, day, month, year);



        if (0 != (rmc = malloc(sizeof(struct RMC)))) {
            memset(rmc, 0, sizeof(struct RMC));
            rmc->hour = hour;
            rmc->min = minute;
            rmc->sec = second;
            rmc->day = day;
            rmc->month = month;
            rmc->year = year;
            rmc->status = status;
            rmc->lat = lat;
            rmc->lon = lon;
            rmc->speed = speed;
            rmc->heading = heading;
        }
    }

    return rmc;
}




/* ---------------------------------------------------------------------- */
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


/* ---------------------------------------------------------------------- */
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


/* ---------------------------------------------------------------------- */
const char* g_empty = " ";
int tokenize(char* s, const char* tokens[], int len)
{
    int token_count; // how many tokens in the array.
    const char* token;
    static const char* delims = ",*";

    if (!tokens) {
        len = 0;
    }

    fprintf(stdout, "%s", s);

    token_count = 0;
    while (token_count < len && 0 != (token = strsep(&s, delims))) {
        if (0 == *token) {
            token = g_empty;
        }
        tokens[token_count] = token;
        ++token_count;
    }

    // fill the unused slots in the token array with null pointer.
    for (int i=token_count; i<len; ++i) {
        tokens[i] = 0;
    }

    return token_count;
}


