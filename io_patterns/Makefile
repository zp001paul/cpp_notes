CFLAGS := -std=c++20
PROGS := buffered sync direct bufwrite_directread

.PNONY: all clean

all: $(PROGS)

$(PROGS):%:%.cpp
	g++ $(CFLAGS) -o $@ $@.cpp

clean:
	rm -f $(PROGS)
