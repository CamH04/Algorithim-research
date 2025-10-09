#!/bin/bash
cat > Simple.java << 'EOF'
class Simple{
    static int AddOne(int input){
        return input + 1;
    }
    static int Square(int input){
        return input * input;
    }
    public static void main(String[] args){
        int sum = 1 + 2 + 3;
        int product = 2 * 3;
        int addResult = AddOne(10);
        int squareResult = Square(5);
    }
}
EOF

input_size=$(stat -c%s Simple.java)
start_time=$(date +%s%3N)
compile_memory=$(/usr/bin/time -v javac Simple.java 2>&1 | grep "Maximum resident set size" | awk '{print $6}')
end_time=$(date +%s%3N)
compile_time=$((end_time - start_time))
output_binary="Simple.class"
if [ -f "$output_binary" ]; then
    output_size=$(stat -c%s "$output_binary")
else
    output_size=0
fi
echo "Input: ${input_size} bytes"
echo "Output: ${output_size} bytes"
echo "Compilation time: ${compile_time} ms"
peak_mb=$(echo "scale=2; $compile_memory / 1024" | bc)
overhead_kb=$(echo "scale=2; $compile_memory * 0.25" | bc)
printf "Compilation memory: %.2f MB peak (%.2f KB overhead)\n" $peak_mb $overhead_kb
echo ""
echo "Running the program:"
echo "===================="
java Simple

rm -f Simple.java Simple.class
