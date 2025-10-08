#!/bin/bash
cat > Program.cs << 'EOF'
using System;
class Program{
    static int Factorial(int n){
        if (n == 0) return 1;
        return n * Factorial(n - 1);
    }
    static bool IsEven(int n){
        return n % 2 == 0;
    }
    static int ComplexLogic(int x, int y, int z){
        if (x > 0 && y < 10){
            if (z == 5 || z >= 20){
                return (x * y) + z;
            } else {
                return (x * z) - y;
            }
        } else {
            if (x != 0){
                return (y + z) / x;
            } else {
                return 42;
            }
        }
    }
    static void Main(){
        int[] numbers = {0, 1, 2, 3, 4, 5};
        int[] factorials = new int[6];
        int[] evenFactorials = new int[6];
        int evenCount = 0;
        for (int i = 0; i < 6; i++){
            factorials[i] = Factorial(numbers[i]);
        }
        int sum = 0;
        for (int i = 0; i < 6; i++){
            if (IsEven(factorials[i])){
                evenFactorials[evenCount++] = factorials[i];
                sum += factorials[i];
            }
        }
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
