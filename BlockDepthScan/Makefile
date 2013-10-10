EXE = blockscan
SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:%.c=%.o)

CFLAGS += -Wall -O2 -std=gnu99 

all : $(EXE)

$(EXE) : $(OBJECTS)
	$(CC) -o $@ $^

%.o : %.c
	$(CC) -c $(CFLAGS) $?

clean :
	@rm -f $(OBJECTS) $(EXE)
