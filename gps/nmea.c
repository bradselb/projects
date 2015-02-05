
/* some helpful web sites: */
/* http://freenmea.net/docs */
/* http://www.trimble.com/OEM_ReceiverHelp/V4.44/en/ */

/* */
struct GGA
{
    int hour;
    int min;
    int sec;
    float lat;
    float lon;
    int quality; /* 0: no fix, 1: GPS fix, 2: DGPS fix */
    int num_sats_in_view;
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
    char mode[8];
    int num_sats_in_view;
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
    float mag_variation; /* degrees */
};
