#include <stdio.h>
#include <string.h>
#include <ctype.h>

// --------------------------------------------------------------------------
int hexdump(const void* p, int size)
{
    unsigned char* buf = (unsigned char*)p;
	int k = 0;
	for (int i=0; i<size; ++i) {
		if (0 == i%16) {
			if (0 < i) {
				printf("| ");
				for (int j=k; j<i; ++j) {
					if(isprint(buf[j]))
						printf("%c", buf[j]);
					else
						printf(".");
				}
				k = i;
			}
			printf("\n%04x | ", i);
		}
		printf("%02x ", buf[i]);
	} // for

	// need to insert spaces when size%16 != 0
	for (int i=size%16; i%16; ++i) {
		printf("   ");
	}

	// now, finish up the line by prining the ascii for the tail
	printf("| ");
	for (int j=k; j<size; ++j) {
		if(isprint(buf[j]))
			printf("%c", buf[j]);
		else
			printf(".");
	}

	return 0;
}


