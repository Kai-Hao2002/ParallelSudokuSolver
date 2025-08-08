//include/core/SudokuUtils.hpp
#pragma once

#include "Sudoku.hpp"
#include <iostream>
#include <cmath>
namespace SudokuUtils {

inline void printSudoku(const Sudoku& sudoku) {
    const auto& board = sudoku.getBoard();
    int size = sudoku.getSize();
    int boxSize = static_cast<int>(std::sqrt(size));

    for (int r = 0; r < size; ++r) {
        if (r % boxSize == 0 && r != 0)
            std::cout << std::string(size * 3 + boxSize - 1, '-') << '\n';

        for (int c = 0; c < size; ++c) {
            if (c % boxSize == 0 && c != 0)
                std::cout << "| ";

            int val = board[r][c];
            if (val == 0) std::cout << ". ";
            else if (val <= 9) std::cout << val << " ";
            else std::cout << static_cast<char>('A' + val - 10) << " ";
        }
        std::cout << '\n';
    }
}


}
