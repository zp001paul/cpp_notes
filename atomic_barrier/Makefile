CFLAGS := -std=gnu11
PROGS := 
MTPROGS := atomic_arr

.PNONY: all clean 

all: $(PROGS) $(MTPROGS)

$(PROGS):%:%.c
	gcc $(CFLAGS) -o $@ $@.c
$(MTPROGS):%:%.c
	gcc $(CFLAGS) -lpthread -o $@ $@.c

clean:
	rm -f $(PROGS) $(MTPROGS)
