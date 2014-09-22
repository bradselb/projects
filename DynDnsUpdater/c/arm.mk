SHELL=/bin/bash

TARGET=arm-linux
TOOLS_DIR=${HOME}/buildroot/output/host/usr/bin

CC=${TOOLS_DIR}/arm-linux-gcc
CFLAGS += -I../../lib
#CFLAGS=-mcpu=ep9312 -mfix-crunch-d1 -D__MAVERICK__
CFLAGS += -Wall -O2 -std=gnu99

EXE = updateDynDNS 
SRCS = $(wildcard *.c) $(wildcard ../../lib/*.c)
OBJS = $(SRCS:%.c=%.o)

ifdef DO_NOT_USE_CURL_LIBRARY
LIB_FLAGS=
CFLAGS += -DDO_NOT_USE_CURL_LIBRARY
endif

ifndef DO_NOT_USE_CURL_LIBRARY
LIB_FLAGS = -lcurl -lrt
endif


all : $(EXE)

$(EXE) : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIB_FLAGS)

%.o : %.cpp
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(OBJS) $(EXE)
