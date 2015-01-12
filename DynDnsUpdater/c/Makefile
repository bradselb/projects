SHELL=/bin/bash

EXE = updateDynDNS
ALL_SRCS = $(wildcard *.c) $(wildcard ../../lib/*.c)
EXCLUDE_SRCS=
SRCS = $(filter-out $(EXCLUDE_SRCS), $(ALL_SRCS))
OBJS = $(SRCS:%.c=%.o)

MY_LIB = brad
MY_LIBDIR = ../../lib

CFLAGS += -Wall -O2 -std=gnu99 -I$(MY_LIBDIR)
#LDFLAGS += -L$(MY_LIBDIR)

all : $(EXE)

lib :
	make -C $(MY_LIBDIR) $<

$(EXE) : $(OBJS)
	$(CC)  $(LDFLAGS) -o $@ $(OBJS)
#	$(CC)  $(LDFLAGS) -o $@ $(OBJS) -l$(MY_LIB)

%.o : %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(EXE)
