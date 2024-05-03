# CFLAGS += -c -std=c++20 -I/usr/local/include/fmt/include 
# LDFLAGS += -L/usr/local/lib -lfmt
CFLAGS += -c -std=c++20 -fconcepts
LDFLAGS = 

# .PHONY: default
# default: misc misc.o

misc: misc.o
	g++ $(LDFLAGS) -o $@ $(@:%=%.o)

misc.o: misc.cpp myconcept.hpp myprt.hpp myalgorithm.hpp
	g++ $(CFLAGS) -o $@ $(@:%.o=%.cpp)


clean:
	rm -f *.o misc
