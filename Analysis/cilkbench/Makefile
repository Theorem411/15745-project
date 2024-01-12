# CLANG = /afs/ece/project/seth_group/ziqiliu/uli-opencilk-project/build/bin/clang
# OPT = /afs/ece/project/seth_group/ziqiliu/uli-opencilk-project/build/bin/opt
CFILES = $(wildcard *.cpp)

LLFILES = $(CFILES:.cpp=.ll)

all: $(LLFILES)

%.bc: %.cpp
	@clang++ -Xclang -disable-O0-optnone -ftapir=serial -fopencilk -O0 --opencilk-resource-dir=/afs/ece/project/seth_group/ziqiliu/cheetah/build -emit-llvm -c $< -o $@
	@opt -mem2reg -loop-simplify -loop-rotate -instcombine -lcssa $@ -o $@   
# -simplifycfg (undo -loop-simplify)
%.ll: %.bc
	@echo "Compiling $@..."
	@llvm-dis $< -o $@

clean: 
	rm -f *.bc *~ *.o *.ll

.PHONY: all clean
