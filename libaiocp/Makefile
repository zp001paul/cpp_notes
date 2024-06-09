CC := gcc
LDFLAGS := -laio -lpthread
.PHONY : all clean

all: libaiocp.o libaiocp

libaiocp.o: $(@:%.o=%.c) $(@:%.o=%.h)
	$(CC) -c -g -o $@ $(@:%.o=%.c)
libaiocp: $(@:%=%.o)
	$(CC) $(LDFLAGS) -o $@ $(@:%=%.o)
clean:
	rm -f libaiocp.o libaiocp
