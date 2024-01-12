# opt -enable-new-pm=0 -load ./testpass.so -test-pass ./cilkbench/2d-loop.ll -o ./cilkbench/2d-loop-pf.bc
opt -enable-new-pm=0 -load ./testpass.so -test-pass ./cilkbench/spMV.ll -o ./cilkbench/spMV-pf.bc
llvm-dis ./cilkbench/2d-loop-pf.bc
# rm -f ./cilkbench/2d-loop-pf.bc

# for file in ./cilkbench/*.ll; do
#     base_name=$(basename "$file" .ll)

#     opt -enable-new-pm=0 -load ./testpass.so -test-pass "$file" -o "./cilkbench/${base_name}-pf.bc"
#     llvm-dis "./cilkbench/${base_name}-pf.bc"
#     rm -f "./cilkbench/${base_name}-pf.bc"
# done

# clang+