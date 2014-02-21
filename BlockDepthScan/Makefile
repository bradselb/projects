SHELL=/bin/bash

EXE = blockscan
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)

BRAD_LIBDIR = ../lib


CFLAGS += -Wall -O2 -std=gnu99 -I$(BRAD_LIBDIR)
LDFLAGS += -L$(BRAD_LIBDIR)

all : $(EXE)

lib :
	make -C $(BRAD_LIBDIR) $<

$(EXE) : $(OBJECTS)
	$(CC) -o $@ $(LDFLAGS) $^ -lbrad

%.o : %.c
	$(CC) -c $(CFLAGS) $?

clean :
	@rm -f $(OBJECTS) $(EXE)
