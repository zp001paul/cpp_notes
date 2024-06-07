CC := gcc
CFLAGS := -std=gnu11 -Wall
CLIBS := -laio
PROGS := libaio_normal libaio_direct libaio_fsync

.PNONY: all clean cp

all: $(PROGS)

$(PROGS):%:%.c
	$(CC) $(CFLAGS) $(CLIBS) -o $@ $@.c

clean:
	rm -f $(PROGS)

cp:
	cp $(PROGS) /win_Downloads/cpp_progs/

