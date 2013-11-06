SHELL=/bin/bash

TARGET=arm-linux
PRJROOT=/home/brad/ucsc-ex/emblin/build-arm-linux/
PREFIX=${PRJROOT}/BUILD/toolchain
SYSROOT=${PREFIX}/${TARGET}

CC=${PREFIX}/bin/arm-linux-gcc
CXX=${PREFIX}/bin/arm-linux-g++
CPPFLAGS="-I${PREFIX}/${TARGET}/include"
CFLAGS=-mcpu=ep9312 -mfix-crunch-d1 -D__MAVERICK__
LDFLAGS=-L${PREFIX}/${TARGET}/lib

EXE = updateDynDNS 
OBJS = Config.o getURL.o State.o timeHelpers.o extractIpAddress.o updateDynDNS.o

all : $(EXE)

$(EXE) : $(OBJS)
	$(CC)  $(LDFLAGS) -o $@ $(OBJS) -lcurl -lrt


%.o : %.cpp
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(EXE)
