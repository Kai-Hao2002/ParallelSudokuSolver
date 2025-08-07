//src/io/ArgumentParser.cpp
#include "io/ArgumentParser.hpp"
#include <iostream>
#include <sstream>

ParsedArgs ArgumentParser::parse(int argc, char* argv[]) {
    ParsedArgs args;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            args.inputFile = argv[++i];
        } else if (arg == "-m" && i + 1 < argc) {
            args.mode = std::stoi(argv[++i]);
        } else if (arg == "-t" && i + 1 < argc) {
            args.numThreads = std::stoi(argv[++i]);
        } else if (arg == "-w") {
            args.writeToFile = true;
        }
    }

    if (args.inputFile.empty()) {
        std::cerr << "Missing -i <input_file>\n";
        return args;
    }

    args.valid = true;
    return args;
}

