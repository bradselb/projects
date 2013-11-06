SHELL=/bin/bash

EXE = updateDynDNS 
OBJS = Config.o getURL.o State.o timeHelpers.o extractIpAddress.o updateDynDNS.o

all : $(EXE)

$(EXE) : $(OBJS)
	$(CC)  $(LDFLAGS) -o $@ $(OBJS) -lcurl -lrt


%.o : %.c
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

clean:
	rm -f *.o $(EXE)
