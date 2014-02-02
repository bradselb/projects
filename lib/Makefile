LIB=libbrad.a
SOURCES = $(wildcard *.c)
OBJS := $(SOURCES:%.c=%.o)


CFLAGS += -Wall -O2 -std=gnu99

all : $(LIB)

lib : $(LIB)

$(LIB) : $(OBJS)
	$(AR) cr $@ $^

%.o : %.c
	$(CC) -c $(CFLAGS) $<

clean :
	@rm -f $(OBJS) $(LIB)
