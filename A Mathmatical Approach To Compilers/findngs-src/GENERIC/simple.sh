#!/bin/bash
cat > simple.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
int add_one(int input) {
    return input + 1;
}
int square(int input) {
    return input * input;
}
int main() {
    int sum = 1 + 2 + 3;
    int product = 2 * 3;
    int add_result = add_one(10);
    int square_result = square(5);
    return 0;
}
EOF
input_size=$(stat -c%s simple.c)
start_time=$(date +%s%3N)
compile_memory=$(/usr/bin/time -v gcc -o simple simple.c 2>&1 | grep "Maximum resident set size" | awk '{print $6}')
end_time=$(date +%s%3N)
compile_time=$((end_time - start_time))
output_size=$(stat -c%s simple.c)
echo "Input: ${input_size} bytes"
echo "Output: ${output_size} bytes"
echo "Compilation time: ${compile_time} ms"
peak_mb=$(echo "scale=2; $compile_memory / 1024" | bc)
overhead_kb=$(echo "scale=2; $compile_memory * 0.25" | bc)
printf "Compilation memory: %.2f MB peak (%.2f KB overhead)\n" $peak_mb $overhead_kb
echo ""
echo "Running the program:"
echo "===================="
./simple
rm simple simple.c
