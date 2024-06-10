CC := gcc
LDFLAGS := -laio -lpthread
CFLAGS := -std=gnu11 -c -g

CAPPS := libaiocp
COBJS := $(CAPPS:%=%.o)

$(CAPPS): % : %.o
	$(CC) $(LDFLAGS) -o $@ $^

$(COBJS): %.o : %.c
	gcc $(CFLAGS) -o $@ $^

chunk_diff: % : %.o
	$(CC) $(LDFLAGS) -lcrypto -o $@ $^

chunk_diff.o: %.o : %.c
	gcc $(CFLAGS) -o $@ $^

clean:
	rm -f $(CAPPS) 
	rm -f $(COBJS)
	rm -f $(CPPAPPS) 
	rm -f $(CPPOBJS)
