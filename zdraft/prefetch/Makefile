all: utils.o cache-locality.o array-access.o affine-expr.o prefetch.so

CXXFLAGS = -rdynamic $(shell llvm-config --cxxflags) -fexceptions -g -O0 -fPIC -I ../eigen-3.4.0
CXX = /afs/ece/project/seth_group/ziqiliu/GCC-12.2.0/bin/g++-12.2.0

ifeq ($(DEBUG), 0)
    CXXFLAGS += -DNDEBUG # Define NDEBUG for non-debug builds
endif

utils.o: utils.cpp utils.h
affine-expr.o: affine-expr.h affine-expr.cpp utils.h
array-access.o: array-access.h array-access.cpp utils.h
cache-locality.o: cache-locality.h cache-locality.cpp array-access.h utils.h
prefetch.o: prefetch.h prefetch.cpp cache-locality.h array-access.h utils.h peeling.h

%.so: %.o 	
	$(CXX) -dylib -shared $^ -o $@

clean: 
	rm -f *.o *~ *.so


.PHONY: all clean