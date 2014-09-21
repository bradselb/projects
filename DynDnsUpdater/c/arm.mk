SHELL=/bin/bash


TARGET=arm-linux
# PRJROOT=${HOME}/buildroot/outpu
PREFIX=${HOME}/buildroot/output/host/usr/
# SYSROOT=${PREFIX}/${TARGET}

CC=${PREFIX}/bin/arm-linux-gcc
CFLAGS += "-I../../lib"
#CFLAGS=-mcpu=ep9312 -mfix-crunch-d1 -D__MAVERICK__
CFLAGS += -Wall -O2 -std=gnu99
#LDFLAGS=-L${PREFIX}/${TARGET}/lib

EXE = updateDynDNS 
SRCS = $(wildcard *.c) $(wildcard ../../*.c)
OBJS = $(SRCS:%.c=%.o)

all : $(EXE)

$(EXE) : $(OBJS)
	$(CC)  $(LDFLAGS) -o $@ $(OBJS) -lcurl -lrt


%.o : %.cpp
	$(CC) -c $(CFLAGS) $<

clean:
	rm -f $(OBJS) $(EXE)
