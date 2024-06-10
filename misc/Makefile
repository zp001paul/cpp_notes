# for misc
CFLAGS := -c -g -std=c++20 -fconcepts
LDFLAGS := 

misc: misc.o
	g++ $(LDFLAGS) -o $@ $(@:%=%.o)

misc.o: misc.cpp myconcept.hpp myprt.hpp myalgorithm.hpp
	g++ $(CFLAGS) -o $@ $(@:%.o=%.cpp)

# for generic cpp prog
CPPAPPS := get_kernel_ver
CPPOBJS := $(CAPPS:%=%.o)

$(CPPAPPS): % : %.o
	g++ -o $@ $^

$(CPPOBJS): %.o : %.c
	g++ -std=c++20 -c -g -o $@ $^

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
	rm -f get_kernel_ver


# example codes:
# get_kernel_ver: get_kernel_ver.o
# 	gcc -o $@ $^
#
# get_kernel_ver.o: get_kernel_ver.c
# 	gcc -std=gnu11 -c -g -o $@ $^
