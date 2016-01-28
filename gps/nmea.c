#define _BSD_SOURCE 1

#include "nmea.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> /* toupper() */
//#include <time.h>

/* ---------------------------------------------------------------------- */
/* some helpful web sites: */
/* http://freenmea.net/docs */
/* http://www.trimble.com/OEM_ReceiverHelp */
/* http://aprs.gids.nl/nmea */
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
struct GSA* parse_gsa(const char* tokens[], int token_count)
{
    struct GSA* gsa = 0;
    char mode;
    int i, j;
    int fix_type;
    int prn[12];
    float pdop, hdop, vdop;
    const char* s = tokens[0];

    if (0 == strncmp(s+3, "GSA", 3) && 12 < token_count) {
        mode = *tokens[1];
        fix_type = atoi(tokens[2]);
        for (i=0; i<12; ++i) {
            prn[i] = atoi(tokens[3+i]);
        }
        pdop = atof(tokens[15]);
        hdop = atof(tokens[16]);
        vdop = atof(tokens[17]);

        if (0 != (gsa = malloc(sizeof(struct GSA)))) {
            memset(gsa, 0, sizeof(struct GSA));
            gsa->mode = mode;
            gsa->fix_type = fix_type;
            for (i=0, j=0; i<12; ++i) {
                if (prn[i] != 0) {
                    gsa->prn[j] = prn[i];
                    ++j;
                }
            }
            gsa->pdop = pdop;
            gsa->hdop = hdop;
            gsa->vdop = vdop;
        }
    }

    return gsa;
}


/* ---------------------------------------------------------------------- */
struct GNS* parse_gns(const char* tokens[], int token_count)
{
    struct GNS* gns = 0;
    int hour, minute, second;
    int sat_count;
    float lat, lon, hdop, altitude, geoid_height;
    const char* s = tokens[0];

    lat = 0.0; lon = 0.0;
    if (0 == strncmp(s+3, "GNS", 3) && 12 < token_count) {
        parse_date_time(tokens[1], &hour, &minute, &second);
        parse_lat_lon(tokens[2], tokens[3], &lat);
        parse_lat_lon(tokens[4], tokens[5], &lon);

        sat_count = atoi(tokens[7]);
        hdop = atof(tokens[8]);
        altitude = atof(tokens[9]);
        geoid_height = atof(tokens[10]);
        /* tokens[11] is age of differential data */
        /* tokens[12] is the station ID of the differntial data source */
        /* tokens[13] is the checksum sans asterisk */

        if (0 != (gns = malloc(sizeof(struct GNS)))) {
            memset(gns, 0, sizeof(struct GNS));
            gns->hour = hour;
            gns->min = minute;
            gns->sec = second;
            gns->lat = lat;
            gns->lon = lon;
            //gns->mode = mode;
            gns->sat_count = sat_count;
            gns->hdop = hdop;
            gns->altitude = altitude;
            gns->geoid_height = geoid_height;

            int len = sizeof gns->mode;
            memset(gns->mode, 0, len);
            strncpy(gns->mode, tokens[6], len);

        }
    }
    return gns;
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

    if (first) *first = 0;
    if (mid) *mid = 0;
    if (last) *last = 0;

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
    if (degrees) *degrees = 0.0;
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
    val = (val/60.0) + atoi(buf);

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


