all: test testpass.so #LoopTransformUtils.o

CXXFLAGS = -rdynamic $(shell llvm-config --cxxflags) -fexceptions -g -O0 -fPIC -I ../eigen-3.4.0
CXX = /afs/ece/project/seth_group/ziqiliu/GCC-12.2.0/bin/g++-12.2.0

AffineExpr.o: AffineExpr.cpp AffineExpr.h
	$(CXX) $(CXXFLAGS) -c AffineExpr.cpp -o $@

EigenUtils.o: EigenUtils.cpp EigenUtils.h
	$(CXX) $(CXXFLAGS) -c EigenUtils.cpp -o $@

LoopTransformUtils.o: LoopTransformUtils.cpp LoopTransformUtils.h
	$(CXX) $(CXXFLAGS) -c LoopTransformUtils.cpp -o $@

utils.o: utils.cpp utils.h 
	$(CXX) $(CXXFLAGS) -c utils.cpp -o utils.o

ParserUtils.o: ParserUtils.cpp ParserUtils.h utils.h AffineExpr.h EigenUtils.h
	$(CXX) $(CXXFLAGS) -c ParserUtils.cpp -o $@

PrefetchLoopAnalysis.o: PrefetchLoopAnalysis.cpp PrefetchLoopAnalysis.h ParserUtils.h utils.h
	$(CXX) $(CXXFLAGS) -c PrefetchLoopAnalysis.cpp -o $@

testpass.o: testPass.h testPass.cpp PrefetchLoopAnalysis.h ParserUtils.h LoopTransformUtils.h
	$(CXX) $(CXXFLAGS) -c testPass.cpp -o $@

testpass.so: testpass.o PrefetchLoopAnalysis.o ParserUtils.o utils.o EigenUtils.o AffineExpr.o LoopTransformUtils.o
	$(CXX) -dylib -shared $^ -o $@

test: test.o EigenUtils.o
	$(CXX) $(CXXFLAGS) test.o EigenUtils.o -o $@

test.o: testEigen.cpp EigenUtils.h
	$(CXX) $(CXXFLAGS) -c testEigen.cpp -o $@

clean: 
	rm -f *.o *~ *.so *.bc test

.PHONY: all clean