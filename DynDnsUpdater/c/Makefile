SHELL=/bin/bash

EXE = updateDynDNS
SRCS = $(wildcard *.c)
OBJS = $(SRCS:%.c=%.o)

CFLAGS += -Wall -O2 -std=gnu99

all : $(EXE)

$(EXE) : $(OBJS)
	$(CC)  $(LDFLAGS) -o $@ $(OBJS) -lcurl -lrt


%.o : %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(EXE)
