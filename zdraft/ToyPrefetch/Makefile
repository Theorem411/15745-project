all: TapirToyPass.so PeelToyPass.so

CXXFLAGS = -rdynamic $(shell llvm-config --cxxflags) -fexceptions -g -O0 -fPIC -I ../eigen-3.4.0
CXX = /afs/ece/project/seth_group/ziqiliu/GCC-12.2.0/bin/g++-12.2.0

ifeq ($(DEBUG), 0)
    CXXFLAGS += -DNDEBUG # Define NDEBUG for non-debug builds
endif

PeelToyPass.o: PeelToyPass.cpp PeelToyPass.h peeling.h 
TapirToyPass.o: TapirToyPass.cpp TapirToyPass.h

%.so: %.o 	
	$(CXX) -dylib -shared $^ -o $@

clean: 
	rm -f *.o *~ *.so *.bc


.PHONY: all clean