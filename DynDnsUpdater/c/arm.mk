SHELL=/bin/bash

TARGET=arm-linux
TOOLS_DIR=${HOME}/buildroot/output/host/usr/bin

CC=${TOOLS_DIR}/arm-linux-gcc
CFLAGS += -I../../lib
CFLAGS += -Wall -O2 -std=gnu99

EXE = updateDynDNS 
ALL_SRCS = $(wildcard *.c) $(wildcard ../../lib/*.c)
EXCLUDE_SRCS=getURL.c
SRCS = $(filter-out $(EXCLUDE_SRCS), $(ALL_SRCS))
OBJS = $(SRCS:%.c=%.o)

CFLAGS += -DDO_NOT_USE_CURL_LIBRARY

all : $(EXE)

$(EXE) : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIB_FLAGS)

%.o : %.cpp
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(OBJS) $(EXE)
