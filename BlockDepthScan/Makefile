EXE = blockscan
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)

BRAD_LIBDIR = ../lib


CFLAGS += -Wall -O2 -std=gnu99 -I$(BRAD_LIBDIR)

all : $(EXE)

$(EXE) : $(OBJECTS)
	$(CC) -o $@ -L$(BRAD_LIBDIR) $^ -lbrad

%.o : %.c
	$(CC) -c $(CFLAGS) $?

clean :
	@rm -f $(OBJECTS) $(EXE)
