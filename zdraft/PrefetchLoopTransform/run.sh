opt -enable-new-pm=0 -load ./ToyPass.so -toy-pass ../microbench/cilkfor-sidebyside.ll -o ./cilkfor-sidebyside-pf.bc
llvm-dis ./cilkfor-sidebyside-pf.bc
