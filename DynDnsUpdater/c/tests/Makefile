SHELL=/bin/bash

EXE = testConfig
SRCS = $(wildcard *.c) ../Config.c
OBJS = $(SRCS:%.c=%.o)

BRAD_LIBDIR = ../../../lib

CFLAGS += -Wall -O2 -std=gnu99 -I.. -I$(BRAD_LIBDIR)
LDFLAGS += -L$(BRAD_LIBDIR)

all : $(EXE)

lib :
	make -C $(BRAD_LIBDIR) $<

$(EXE) : $(OBJS)
	$(CC)  $(LDFLAGS) -o $@ $(OBJS) -lcurl -lrt -lbrad

%.o : %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(EXE)
