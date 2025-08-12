/***************************************************************************************
*    Title:
*    Author: Cameron Haynes
*    Date: DD/MM/2025
***************************************************************************************/
#include "parser.hpp"
#include "translator.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <sstream>
#include <vector>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file.lctest>\n";
        return 1;
    }

    std::string inputFile = argv[1];
    if (inputFile.size() < 7 || inputFile.substr(inputFile.size() - 7) != ".lctest") {
        std::cerr << "Error: input file must have .lctest extension\n";
        return 1;
    }

    // read src file
    std::ifstream in(inputFile);
    if (!in) {
        std::cerr << "Error: could not open input file " << inputFile << "\n";
        return 1;
    }
    std::string source((std::istreambuf_iterator<char>(in)),
                       std::istreambuf_iterator<char>());
    if (source.find_first_not_of(" \t\r\n") == std::string::npos) {
        std::cerr << "Error: input file is empty or whitespace only\n";
        return 1;
    }

    // parse and translate
    std::string outputData;
    try {
        Parser p(source);
        auto root = p.parse();
        if (!root->is_atom && !root->list.empty()) {
            for (auto &form : root->list) {
                outputData += translate(form) + "\n";
            }
        } else {
            outputData += translate(root) + "\n";
        }
    } catch (std::exception &e) {
        std::cerr << "Error during translation: " << e.what() << "\n";
        return 1;
    }

    // split for dfa
    std::istringstream ss(outputData);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(ss, line)) {
        if (!line.empty())
            lines.push_back(line);
    }


    // output file
    std::filesystem::path outPath = inputFile;
    outPath.replace_extension(".lc");
    std::ofstream out(outPath);
    if (!out) {
        std::cerr << "Error: could not write to " << outPath << "\n";
        return 1;
    }
    out << outputData;
    out.close();

    std::cout << "Lambda calculus IR written to " << outPath << "\n";

    return 0;
}
