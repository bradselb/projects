/* compile with the following command line...

$ gcc -o parse_nmea -D_BSD_SOURCE -Wall -O2 -std=c89 parse_nmea.c

*/
#include "nmea.h"
#include "checksum.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> /* toupper() */
//#include <time.h>


int main(int argc, char* argv[])
{
    FILE* file;
    int arg_index;
    char* buf = 0;
    const unsigned int bufsize = 256;
    const char* tokens[32];
    const int len = sizeof tokens / sizeof tokens[0];
    int token_count;
    struct GGA* gga;
    struct GNS* gns;
    struct GSA* gsa;
    struct RMC* rmc;

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

            if (!verify_checksum(buf)) break;


            fprintf(stdout, "%s", buf);

            token_count =  tokenize(buf, tokens, len);

            if (0 !=(gga = parse_gga(tokens, token_count))) {
                fprintf(stdout, "   GGA %02d:%02d:%02d %.6f , %.6f %d, %d, %.2f, %.1f(%s), %.1f(%s) \n",
                        gga->hour, gga->min, gga->sec, gga->lat, gga->lon, gga->quality, gga->sat_count, gga->hdop, gga->altitude, tokens[10], gga->geoid_height, tokens[12]);

                free(gga);
            }
            else if (0 != (gns = parse_gns(tokens, token_count))) {
                fprintf(stdout, "   GNS %02d:%02d:%02d %.6f , %.6f %s, %d, %.2f, %.1f(%s), %.1f(%s) \n",
                        gns->hour, gns->min, gns->sec, gns->lat, gns->lon, gns->mode, gns->sat_count, gns->hdop, gns->altitude, "m", gns->geoid_height, "m");

                free(gns);
            }
            else if (0 != (gsa = parse_gsa(tokens, token_count))) {
                int i;
                int prn;

                fprintf(stdout, "   GSA %c, %d, ", gsa->mode, gsa->fix_type);
                i = 0;
                while (i<12 && 0 != (prn = gsa->prn[i])) {
                    fprintf(stdout,"%d, ", prn);
                    ++i;
                }
                fprintf(stdout, "%.1f, %.1f, %.1f\n", gsa->pdop, gsa->hdop, gsa->vdop);

                free(gsa);
            }
            else if (0 != (rmc = parse_rmc(tokens, token_count))) {
                    fprintf(stdout, "   RMC %02d:%02d:%02d  %.6f , %.6f  %.1f(knots), %.1f(degrees) %02d-%02d-%02d\n",
                            rmc->hour, rmc->min, rmc->sec, rmc->lat, rmc->lon, rmc->speed, rmc->heading, rmc->day, rmc->month, rmc->year);

                    free(rmc);
            } 
        } // while

        fclose(file);
    }

exit: 
    if (buf) {
        free(buf);
    }
    return 0;
}


