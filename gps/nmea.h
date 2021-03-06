#ifndef NMEA_H
#define NMEA_H

/* */
struct GGA
{
    int hour;
    int min;
    int sec;
    float lat;
    float lon;
    int quality; /* 0: no fix, 1: GPS fix, 2: DGPS fix */
    int sat_count; /* how many sats were used in the fix */
    float hdop;
    float altitude; /* meters */
    float geoid_height; /* meters above mean sea level */
};

/* Global Nav Satellite System fix data */
struct GNS
{
    int hour;
    int min;
    int sec;
    float lat;
    float lon;
    char mode[4];
    int sat_count; /* Trimble says nr of satellites used */ 
    float hdop;
    float altitude; /* meters */
    float geoid_height; /* meters above mean sea level */
};

/* Active GPS satellites and Dilution of precision*/
struct GSA
{
    char mode; /* m: manual, a: automatic */
    int fix_type; /* 1: no fix, 2: 2D fix, 3: 3D fix */
    int prn[12]; /* Ids of satellites used for fix */
    float pdop; /* meters */
    float hdop; /* meters */
    float vdop; /* meters */
};


/* recommended minimum coordinates  */
struct RMC
{
    int hour;
    int min;
    int sec;
    int day;
    int month;
    int year;
    char status; /* A: active, V: void */
    float lat;
    float lon;
    float speed; /* ground speed in knots */
    float heading; /* track angle in degrees, true */
};

int tokenize(char* s, const char* tokens[], int len);

struct GGA* parse_gga(const char* tokens[], int token_count);
struct GNS* parse_gns(const char* tokens[], int token_count);
struct GSA* parse_gsa(const char* tokens[], int token_count);
struct RMC* parse_rmc(const char* tokens[], int token_count);

#endif

