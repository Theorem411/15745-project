set -e
echo "Running My Prefetch Pass..."
opt -enable-new-pm=0 -load ./TapirToyPass.so -tapir-toy-pass ./toy.ll -o ./toy-pf.bc
llvm-dis ./toy-pf.bc
opt -enable-new-pm=0 -load ./PeelToyPass.so -peeling-toy-pass ./toy.ll -o ./toy-peel.bc
llvm-dis ./toy-peel.bc
opt -enable-new-pm=0 -load ./PeelToyPass.so -peeling-toy-pass -pass-remarks-analysis=peeling-toy-pass -pass-remarks=peeling-toy-pass ./cilkfor-trivial.ll -o ./cilkfor-trivial-peel.bc
llvm-dis ./cilkfor-trivial-peel.bc
# working command for lowering into DAC form
clang clang -Xclang -disable-O0-optnone -fcilkplus -O0 -emit-llvm -S cilkfor-trivial-peel.bc -o cilkfor-trivial-peel-lower.ll