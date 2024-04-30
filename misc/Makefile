CFLAGS += -c -std=c++20 -I/usr/local/include/fmt/include 
LDFLAGS += -L/usr/local/lib -lfmt

# .PHONY: default
# default: misc misc.o

misc: misc.o
	g++ $(LDFLAGS) -o $@ $(@:%=%.o)

misc.o: misc.cpp
	g++ $(CFLAGS) -o $@ $(@:%.o=%.cpp)


clean:
	rm -f *.o misc
