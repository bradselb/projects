SHELL=/bin/bash

TARGET=arm-linux
TOOLS_DIR=${HOME}/buildroot/output/host/usr/bin

CC=${TOOLS_DIR}/arm-linux-gcc
CFLAGS += -I../../lib
CFLAGS += -Wall -O2 -std=gnu99

EXE = updateDynDNS 
ALL_SRCS = $(wildcard *.c) $(wildcard ../../lib/*.c)
EXCLUDE_SRCS =
SRCS = $(filter-out $(EXCLUDE_SRCS), $(ALL_SRCS))
OBJS = $(SRCS:%.c=%.o)

all : $(EXE)

$(EXE) : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS)

%.o : %.c
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(OBJS) $(EXE)
