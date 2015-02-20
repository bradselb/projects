SHELL=/bin/bash

EXE = parse_nmea
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)

CFLAGS += -Wall -O2 -std=c99 -D_BSD_SOURCE 

all : $(EXE)

$(EXE) : $(OBJECTS)
	$(CC) -o $@ $(LDFLAGS) $^

%.o : %.c
	$(CC) -c $(CFLAGS) $?

clean :
	@rm -f $(OBJECTS) $(EXE)
