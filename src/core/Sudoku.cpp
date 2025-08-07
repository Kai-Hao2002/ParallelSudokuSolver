//src/core/Sudoku.cpp
#include "core/Sudoku.hpp"
#include <fstream>
#include <sstream>
#include <cmath>
#include <iostream>

Sudoku::Sudoku(int size) : size(size), board(size, std::vector<int>(size, 0)) {
    boxSize = static_cast<int>(std::sqrt(size));
}

bool Sudoku::loadFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) return false;

    std::string line;
    int row = 0;
    while (std::getline(inFile, line) && row < size) {
        std::istringstream ss(line);
        for (int col = 0; col < size; ++col) {
            int val;
            ss >> val;
            board[row][col] = val;
        }
        ++row;
    }
    return true;
}

bool Sudoku::isValid(int row, int col, int val) const {
    return isRowValid(row, val) &&
           isColValid(col, val) &&
           isBoxValid(row - row % boxSize, col - col % boxSize, val) &&
           board[row][col] == 0;
}

bool Sudoku::isRowValid(int row, int val) const {
    for (int col = 0; col < size; ++col)
        if (board[row][col] == val) return false;
    return true;
}

bool Sudoku::isColValid(int col, int val) const {
    for (int row = 0; row < size; ++row)
        if (board[row][col] == val) return false;
    return true;
}

bool Sudoku::isBoxValid(int startRow, int startCol, int val) const {
    for (int r = 0; r < boxSize; ++r)
        for (int c = 0; c < boxSize; ++c)
            if (board[startRow + r][startCol + c] == val)
                return false;
    return true;
}

bool Sudoku::isComplete() const {
    for (const auto& row : board)
        for (int val : row)
            if (val == 0) return false;
    return true;
}

void Sudoku::setValue(int row, int col, int val) {
    board[row][col] = val;
}

int Sudoku::getValue(int row, int col) const {
    return board[row][col];
}

int Sudoku::getSize() const {
    return size;
}

const std::vector<std::vector<int>>& Sudoku::getBoard() const {
    return board;
}

std::vector<std::pair<int, int>> Sudoku::getEmptyCells() const {
    std::vector<std::pair<int, int>> empty;
    for (int r = 0; r < size; ++r)
        for (int c = 0; c < size; ++c)
            if (board[r][c] == 0)
                empty.emplace_back(r, c);
    return empty;
}
void Sudoku::print() const {
    for (int i = 0; i < 9; ++i) {
        for (int j = 0; j < 9; ++j) {
            std::cout << board[i][j] << " ";
        }
        std::cout << "\n";
    }
}