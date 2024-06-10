# for misc
CFLAGS := -c -g -std=c++20 -fconcepts
LDFLAGS := 

misc: misc.o
	g++ $(LDFLAGS) -o $@ $(@:%=%.o)

misc.o: misc.cpp myconcept.hpp myprt.hpp myalgorithm.hpp
	g++ $(CFLAGS) -o $@ $(@:%.o=%.cpp)

# for generic cpp prog
CPPAPPS := align
CPPOBJS := $(CPPAPPS:%=%.o)

$(CPPAPPS): % : %.o
	g++ -o $@ $^

$(CPPOBJS): %.o : %.cpp
	g++ -std=gnu++20 -c -g -o $@ $^

# for generic c prog
CAPPS := get_kernel_ver
COBJS := $(CAPPS:%=%.o)

$(CAPPS): % : %.o
	gcc -o $@ $^

$(COBJS): %.o : %.c
	gcc -std=gnu11 -c -g -o $@ $^



.PHONY : clean
clean:
	rm -f *.o 
	rm -f misc
	rm -f $(CPPAPPS)
	rm -f $(CAPPS)

# example codes:
# get_kernel_ver: get_kernel_ver.o
# 	gcc -o $@ $^
#
# get_kernel_ver.o: get_kernel_ver.c
# 	gcc -std=gnu11 -c -g -o $@ $^
