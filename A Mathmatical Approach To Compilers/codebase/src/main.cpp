/***************************************************************************************
*    Title: A Mathematical Approach To Compilers
*    Author: Cameron Haynes
*    Date: 03/09/2025
*    Description: Compiler that translates S-expressions from alisp like srclang to pure
*                 lambda calculus using Church encodings with CSE via hash-consing for
*                 Church numerals.
***************************************************************************************/

#include "parser.hpp"
#include "translator.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <sstream>
#include <vector>
#include <chrono>
#include <iomanip>

#include <sys/resource.h>
#include <unistd.h>

struct MemoryStats {
    size_t peak_rss = 0;
    size_t peak_virtual = 0;
    size_t initial_rss = 0;
    size_t compilation_peak = 0;
};

size_t get_memory_usage() {
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line)) {
        if (line.substr(0, 6) == "VmRSS:") {
            size_t kb;
            if (std::sscanf(line.c_str(), "VmRSS: %zu kB", &kb) == 1) {
                return kb * 1024;
            }
        }
    }
    return 0;
}

size_t get_virtual_memory_usage() {
    std::ifstream status("/proc/self/status");
    std::string line;
    while (std::getline(status, line)) {
        if (line.substr(0, 6) == "VmSize:") {
            size_t kb;
            if (std::sscanf(line.c_str(), "VmSize: %zu kB", &kb) == 1) {
                return kb * 1024;
            }
        }
    }
    return 0;
}

std::string format_bytes(size_t bytes) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unit = 0;
    double size = static_cast<double>(bytes);

    while (size >= 1024.0 && unit < 3) {
        size /= 1024.0;
        unit++;
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << size << " " << units[unit];
    return oss.str();
}

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " <file.lctest> [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --stats    Show compilation statistics\n";
    std::cout << "  --help     Show this help message\n\n";
    std::cout << "Input:  .lctest file containing S-expressions\n";
    std::cout << "Output: .lc file containing lambda calculus IR\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << program_name << " factorial.lctest\n";
    std::cout << "  " << program_name << " arithmetic.lctest --stats\n";
}

void print_compilation_stats(const std::chrono::duration<double>& parse_time,
                           const std::chrono::duration<double>& translate_time,
                           size_t input_size, size_t output_size,
                           const MemoryStats& memory_stats) {
    std::cout << "\n=== Compilation Statistics ===\n";
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Parse time:      " << parse_time.count() * 1000 << " ms\n";
    std::cout << "Translate time:  " << translate_time.count() * 1000 << " ms\n";
    std::cout << "Total time:      " << (parse_time + translate_time).count() * 1000 << " ms\n";
    std::cout << "Input size:      " << format_bytes(input_size) << "\n";
    std::cout << "Output size:     " << format_bytes(output_size) << "\n";
    std::cout << "Size ratio:      " << std::setprecision(2)
              << (double)output_size / input_size << "x\n";

    std::cout << "\n--- Memory Usage ---\n";
    std::cout << "Initial memory:  " << format_bytes(memory_stats.initial_rss) << "\n";
    std::cout << "Peak memory:     " << format_bytes(memory_stats.peak_rss) << "\n";
    std::cout << "Peak virtual:    " << format_bytes(memory_stats.peak_virtual) << "\n";
    size_t compilation_overhead = memory_stats.compilation_peak > memory_stats.initial_rss
                                  ? memory_stats.compilation_peak - memory_stats.initial_rss
                                  : 0;
    std::cout << "Compilation overhead: " << format_bytes(compilation_overhead) << "\n";

    print_cache_stats();
    std::cout << "==============================\n";
}

class MemoryMonitor {
private:
    MemoryStats& stats;
    bool monitoring;

public:
    MemoryMonitor(MemoryStats& mem_stats) : stats(mem_stats), monitoring(false) {
        stats.initial_rss = get_memory_usage();
        stats.peak_rss = stats.initial_rss;
        stats.peak_virtual = get_virtual_memory_usage();
        stats.compilation_peak = stats.initial_rss;
    }

    void start_monitoring() {
        monitoring = true;
    }

    void update() {
        if (!monitoring) return;
        size_t current_rss = get_memory_usage();
        size_t current_virtual = get_virtual_memory_usage();
        if (current_rss > stats.peak_rss) {
            stats.peak_rss = current_rss;
        }
        if (current_virtual > stats.peak_virtual) {
            stats.peak_virtual = current_virtual;
        }
        if (current_rss > stats.compilation_peak) {
            stats.compilation_peak = current_rss;
        }
    }

    void stop_monitoring() {
        monitoring = false;
        update();
    }
};

int main(int argc, char** argv) {
    std::string inputFile;
    bool show_stats = false;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h") {
            print_usage(argv[0]);
            return 0;
        } else if (arg == "--stats" || arg == "-s") {
            show_stats = true;
        } else if (arg.substr(0, 2) == "--") {
            std::cerr << "Unknown option: " << arg << "\n";
            std::cerr << "Use --help for usage information\n";
            return 1;
        } else {
            if (inputFile.empty()) {
                inputFile = arg;
            } else {
                std::cerr << "Multiple input files specified\n";
                return 1;
            }
        }
    }
    if (inputFile.empty()) {
        std::cerr << "Error: no input file specified\n";
        print_usage(argv[0]);
        return 1;
    }
    if (inputFile.size() < 7 || inputFile.substr(inputFile.size() - 7) != ".lctest") {
        std::cerr << "Error: input file must have .lctest extension\n";
        return 1;
    }

    std::ifstream in(inputFile);
    if (!in) {
        std::cerr << "Error: could not open input file '" << inputFile << "'\n";
        std::cerr << "Please check that the file exists and is readable\n";
        return 1;
    }
    std::string source((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();
    size_t input_size = source.size();
    if (source.find_first_not_of(" \t\r\n") == std::string::npos) {
        std::cerr << "Error: input file is empty or contains only whitespace\n";
        return 1;
    }

    MemoryStats memory_stats;
    MemoryMonitor monitor(memory_stats);

    std::string outputData;
    auto total_start = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> parse_time, translate_time;

    try {
        monitor.start_monitoring();
        monitor.update();

        auto parse_start = std::chrono::high_resolution_clock::now();
        Parser parser(source);
        auto root = parser.parse();
        monitor.update();
        auto parse_end = std::chrono::high_resolution_clock::now();
        parse_time = parse_end - parse_start;

        auto translate_start = std::chrono::high_resolution_clock::now();
        if (!root->is_atom && !root->list.empty()) {
            for (auto& form : root->list) {
                outputData += translate(form) + "\n";
                monitor.update();
            }
        } else {
            outputData += translate(root) + "\n";
            monitor.update();
        }
        auto translate_end = std::chrono::high_resolution_clock::now();
        translate_time = translate_end - translate_start;

        monitor.stop_monitoring();

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation Error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected Error: " << e.what() << "\n";
        return 1;
    }

    if (outputData.empty()) {
        std::cerr << "Error: translation produced no output\n";
        return 1;
    }

    std::filesystem::path outPath = inputFile;
    outPath.replace_extension(".lc");

    std::ofstream out(outPath);
    if (!out) {
        std::cerr << "Error: could not write to output file '" << outPath << "'\n";
        std::cerr << "Please check directory permissions\n";
        return 1;
    }

    out << outputData;
    out.close();

    auto total_end = std::chrono::high_resolution_clock::now();
    auto total_time = total_end - total_start;

    std::cout << "LC IR successfully generated\n";
    std::cout << "  Input:  " << inputFile << " (" << format_bytes(input_size) << ")\n";
    std::cout << "  Output: " << outPath << " (" << format_bytes(outputData.size()) << ")\n";

    if (show_stats) {
        print_compilation_stats(parse_time, translate_time,
                                input_size, outputData.size(), memory_stats);
        clear_cache();
    } else {
        auto total_us = std::chrono::duration_cast<std::chrono::microseconds>(total_time);
        std::cout << "  Time:   " << std::fixed << std::setprecision(2)
                  << total_us.count() / 1000.0 << " ms\n";
        std::cout << "  Memory: " << format_bytes(memory_stats.compilation_peak)
                  << " peak (" << format_bytes(memory_stats.compilation_peak - memory_stats.initial_rss)
                  << " overhead)\n";
    }

    return 0;
}

