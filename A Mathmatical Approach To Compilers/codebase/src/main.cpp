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
                           size_t input_size, size_t output_size) {
    std::cout << "\n=== Compilation Statistics ===\n";
    std::cout << std::fixed << std::setprecision(3);
    std::cout << "Parse time:      " << parse_time.count() * 1000 << " ms\n";
    std::cout << "Translate time:  " << translate_time.count() * 1000 << " ms\n";
    std::cout << "Total time:      " << (parse_time + translate_time).count() * 1000 << " ms\n";
    std::cout << "Input size:      " << input_size << " bytes\n";
    std::cout << "Output size:     " << output_size << " bytes\n";
    std::cout << "Size ratio:      " << std::setprecision(2)
              << (double)output_size / input_size << "x\n";
    print_cache_stats();
    std::cout << "==============================\n";
}

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
    std::string source((std::istreambuf_iterator<char>(in)),std::istreambuf_iterator<char>());
    in.close();
    size_t input_size = source.size();
    if (source.find_first_not_of(" \t\r\n") == std::string::npos) {
        std::cerr << "Error: input file is empty or contains only whitespace\n";
        return 1;
    }
    std::string outputData;
    auto total_start = std::chrono::high_resolution_clock::now();

    try {
        auto parse_start = std::chrono::high_resolution_clock::now();
        Parser parser(source);
        auto root = parser.parse();
        auto parse_end = std::chrono::high_resolution_clock::now();
        auto parse_time = parse_end - parse_start;

        auto translate_start = std::chrono::high_resolution_clock::now();
        if (!root->is_atom && !root->list.empty()) {
            for (auto &form : root->list) {
                outputData += translate(form) + "\n";
            }
        } else {
            outputData += translate(root) + "\n";
        }
        auto translate_end = std::chrono::high_resolution_clock::now();
        auto translate_time = translate_end - translate_start;
        if (show_stats) {
            print_compilation_stats(parse_time, translate_time, input_size, outputData.size());
        }

    } catch (const std::runtime_error &e) {
        std::cerr << "Compilation Error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception &e) {
        std::cerr << "Unexpected Error: " << e.what() << "\n";
        return 1;
    }

    auto total_end = std::chrono::high_resolution_clock::now();
    if (outputData.empty()) {
        std::cerr << "Error: translation produced no output\n";
        return 1;
    }
    std::istringstream ss(outputData);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(ss, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
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

    std::cout << "LC IR successfully generated\n";
    std::cout << "  Input:  " << inputFile << " (" << input_size << " bytes)\n";
    std::cout << "  Output: " << outPath << " (" << outputData.size() << " bytes)\n";

    if (!show_stats) {
        auto total_time = total_end - total_start;
        std::cout << "  Time:   " << std::fixed << std::setprecision(2)
                  << total_time.count() << " ms\n";
    }
    if (show_stats) {
        clear_cache();
    }

    return 0;
}
