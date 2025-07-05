#include "util.hpp"
#include <fstream>
#include <stdexcept>

std::vector<std::vector<std::vector<int>>> load_puzzles(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open puzzle file");
    }

    std::vector<std::vector<std::vector<int>>> puzzles;
    std::vector<std::vector<int>> puzzle;
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) {
            if (!puzzle.empty()) {
                puzzles.push_back(puzzle);
                puzzle.clear();
            }
            continue;
        }
        std::vector<int> row;
        for (char ch : line) {
            if (ch >= '0' && ch <= '9') {
                row.push_back(ch - '0');
            }
        }
        if (row.size() == 9) {
            puzzle.push_back(row);
        }
    }
    if (!puzzle.empty()) {
        puzzles.push_back(puzzle);
    }
    return puzzles;
}
