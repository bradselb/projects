#include "checksum.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>


int verify_checksum(const char* nmea_sentence)
{
    int is_good;
    uint8_t checksum;
    char* p;
    char* begin;
    char* end;


    is_good = 0; // no it is not good.

    if (!nmea_sentence) {
        goto EXIT;
    }

    // point to char after the '$'
    begin = strchr(nmea_sentence, '$');
    if (!begin) {
        goto EXIT;
    }
    begin = begin + 1;

    // point to the '*' at the end of the message
    end = strchr(nmea_sentence, '*');
    if (!end) {
        goto EXIT;
    }

    checksum = 0;
    p = begin;
    while (p != end) {
        checksum ^= *p++;
    }
    // now, p points to the '*' do not include the '*'
    // in the checksum. The checksum should be equal to the 
    // value represented by the two hex characters after the '*'.

    is_good = (0 == (checksum ^ strtoul(++p, 0, 16)));



EXIT:
    return is_good;
}
