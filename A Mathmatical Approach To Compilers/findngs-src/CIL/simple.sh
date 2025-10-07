#!/bin/bash
cat > Program.cs << 'EOF'
using System;

class Program{
    static int AddOne(int input){
        return input + 1;
    }
    static int Square(int input){
        return input * input;
    }
    static void Main(){
        int sum = 1 + 2 + 3;
        int product = 2 * 3;
        int addResult = AddOne(10);
        int squareResult = Square(5);
    }
}
EOF
cat > simple.csproj << 'EOF'
<Project Sdk="Microsoft.NET.Sdk">
  <PropertyGroup>
    <OutputType>Exe</OutputType>
    <TargetFramework>net8.0</TargetFramework>
  </PropertyGroup>
</Project>
EOF

input_size=$(stat -c%s Program.cs)
start_time=$(date +%s%3N)
compile_memory=$(/usr/bin/time -v dotnet build -c Release --nologo -v quiet 2>&1 | grep "Maximum resident set size" | awk '{print $6}')
end_time=$(date +%s%3N)
compile_time=$((end_time - start_time))
#where output
output_binary=$(find bin/Release -name "simple.dll" 2>/dev/null | head -1)
if [ -z "$output_binary" ]; then
    output_binary=$(find bin/Release -name "simple" 2>/dev/null | head -1)
fi
if [ -n "$output_binary" ]; then
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

dotnet run -c Release --no-build --nologo
rm -rf bin obj Program.cs simple.csproj
