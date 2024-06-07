CFLAGS := -std=c11
PROGS := direct

.PNONY: all clean

all: $(PROGS)

$(PROGS):%:%.c
	gcc $(CFLAGS) -o $@ $@.c

clean:
	rm -f $(PROGS)
