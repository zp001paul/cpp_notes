CFLAGS := -std=c++20
PROGS := buffered sync direct bufwrite_directread fcntl

.PNONY: all clean cp

all: $(PROGS)

$(PROGS):%:%.cpp
	g++ $(CFLAGS) -o $@ $@.cpp

clean:
	rm -f $(PROGS)

cp:
	cp $(PROGS) /win_Downloads/cpp_progs/
