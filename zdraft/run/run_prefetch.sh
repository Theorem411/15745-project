set -e

cd ../microbench
rm -f ../microbench/${1}.ll
echo "Compiling microbenmarks..."
make ../microbench/${1}.ll

if [ "$2" == "scev" ]; then
    echo "Running/Printing Scalar Evolution Analysis..."
    cd ../run
    opt -enable-new-pm=0 -analyze -scalar-evolution ../microbench/${1}.ll 
else 
    echo "Running My Prefetch Pass..."
    cd ../prefetch
    make
    cd ../run
    opt -enable-new-pm=0 -load ../prefetch/prefetch.so -insert-prefetch ../microbench/${1}.ll -o ../microbench/${1}-pf.ll
fi
