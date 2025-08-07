//src/io/FileIO.cpp
#include "io/FileIO.hpp"
#include <fstream>

namespace FileIO {

bool loadSudokuFromFile(Sudoku& sudoku, const std::string& filename) {
    return sudoku.loadFromFile(filename);
}

bool writeSolutionToFile(const Sudoku& sudoku, const std::string& filename) {
    std::ofstream out(filename);
    if (!out) return false;

    const auto& board = sudoku.getBoard();
    int size = sudoku.getSize();

    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            out << board[r][c];
            if (c != size - 1) out << ' ';
        }
        out << '\n';
    }

    return true;
}

}
