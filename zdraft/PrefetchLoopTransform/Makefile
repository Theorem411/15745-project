all: ToyPass.so AffineExpr.o ArrayAccess.o parserUtils.o utils.o EigenUtils.o PrefetchLoopAnalysis.o #LoopTransformUtils.o PrefetchLoopTransform.o

CXXFLAGS = -rdynamic $(shell llvm-config --cxxflags) -fexceptions -g -O0 -fPIC -I ../eigen-3.4.0
CXX = /afs/ece/project/seth_group/ziqiliu/GCC-12.2.0/bin/g++-12.2.0

ifeq ($(DEBUG), 0)
    CXXFLAGS += -DNDEBUG # Define NDEBUG for non-debug builds
endif

# Rule for toplevel pass
ToyPass.o: ToyPass.cpp ToyPass.h ./Transform/PrefetchLoopTransform.h
	$(CXX) $(CXXFLAGS) -c ToyPass.cpp -o ToyPass.o

# Rule to create ToyPass.so
ToyPass.so: ToyPass.o LoopTransformUtils.o PrefetchLoopTransform.o 
	$(CXX) -dylib -shared $^ -o $@

# Rules for Analysis files
AffineExpr.o: ./Analysis/AffineExpr.cpp ./Analysis/AffineExpr.h
	$(CXX) $(CXXFLAGS) -c ./Analysis/AffineExpr.cpp -o AffineExpr.o
# ArrayAccess.o: ./Analysis/ArrayAccess.cpp ./Analysis/ArrayAccess.h
# 	$(CXX) $(CXXFLAGS) -c ./Analysis/ArrayAccess.cpp -o ArrayAccess.o
parserUtils.o: ./Analysis/parserUtils.cpp ./Analysis/parserUtils.h ./Analysis/utils.h ./Analysis/AffineExpr.h ./Analysis/EigenUtils.h
	$(CXX) $(CXXFLAGS) -c ./Analysis/parserUtils.cpp -o parserUtils.o
EigenUtils.o: ./Analysis/EigenUtils.cpp ./Analysis/EigenUtils.h
	$(CXX) $(CXXFLAGS) -c ./Analysis/EigenUtils.cpp -o EigenUtils.o
utils.o: ./Analysis/utils.cpp ./Analysis/utils.h 
	$(CXX) $(CXXFLAGS) -c ./Analysis/utils.cpp -o utils.o
PrefetchLoopAnalysis.o: ./Analysis/PrefetchLoopAnalysis.cpp ./Analysis/PrefetchLoopAnalysis.h ./Analysis/parserUtils.h ./Analysis/utils.h
	$(CXX) $(CXXFLAGS) -c ./Analysis/PrefetchLoopAnalysis.cpp -o PrefetchLoopAnalysis.o

# Rules for Transform files
LoopTransformUtils.o: ./Transform/LoopTransformUtils.cpp ./Transform/LoopTransformUtils.h
	$(CXX) $(CXXFLAGS) -c ./Transform/LoopTransformUtils.cpp -o LoopTransformUtils.o
PrefetchLoopTransform.o: ./Transform/PrefetchLoopTransform.cpp ./Transform/PrefetchLoopTransform.h ./Transform/LoopTransformUtils.h
	$(CXX) $(CXXFLAGS) -c ./Transform/PrefetchLoopTransform.cpp -o PrefetchLoopTransform.o

# Clean command
clean: 
	rm -f *.o *~ *.so *.bc

.PHONY: all clean