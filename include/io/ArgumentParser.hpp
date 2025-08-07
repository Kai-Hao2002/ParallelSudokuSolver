//include/io/ArgumentParser.hpp
#pragma once

#include <string>

struct ParsedArgs {
    std::string inputFile;
    int mode = 0;
    int numThreads = 2;
    int writeToFile = 0;
    bool valid = false;
};

class ArgumentParser {
public:
    static ParsedArgs parse(int argc, char* argv[]);
};
