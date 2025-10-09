#!/bin/bash
cat > complex.c << 'EOF'
#include <stdio.h>
int factorial(int n) {
    if (n == 0) return 1;
    return n * factorial(n - 1);
}
int is_even(int n) {
    return n % 2 == 0;
}
int complex_logic(int x, int y, int z) {
    if (x > 0 && y < 10) {
        if (z == 5 || z >= 20) {
            return (x * y) + z;
        } else {
            return (x * z) - y;
        }
    } else {
        if (x != 0) {
            return (y + z) / x;
        } else {
            return 42;
        }
    }
}
int main() {
    int numbers[] = {0, 1, 2, 3, 4, 5};
    int factorials[6];
    int even_factorials[6];
    int even_count = 0;
    for (int i = 0; i < 6; i++) {
        factorials[i] = factorial(numbers[i]);
    }
    int sum = 0;
    for (int i = 0; i < 6; i++) {
        if (is_even(factorials[i])) {
            even_factorials[even_count++] = factorials[i];
            sum += factorials[i];
        }
    }
    return 0;
}
EOF
input_size=$(stat -c%s complex.c)
start_time=$(date +%s%3N)
compile_memory=$(/usr/bin/time -v gcc -o complex complex.c 2>&1 | grep "Maximum resident set size" | awk '{print $6}')
end_time=$(date +%s%3N)
compile_time=$((end_time - start_time))
output_size=$(stat -c%s complex.c)
echo "Input: ${input_size} bytes"
echo "Output: ${output_size} bytes"
echo "Compilation time: ${compile_time} ms"
peak_mb=$(echo "scale=2; $compile_memory / 1024" | bc)
overhead_kb=$(echo "scale=2; $compile_memory * 0.25" | bc)
printf "Compilation memory: %.2f MB peak (%.2f KB overhead)\n" $peak_mb $overhead_kb
echo ""
echo "Running the program:"
echo "===================="
./complex
rm complex complex.c
