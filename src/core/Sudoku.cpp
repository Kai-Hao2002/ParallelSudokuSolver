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
    std::ifstream in(filename);
    if (!in) return false;

    int n;
    in >> n;           // Read the first line size
    if (n != size) {   // If the Sudoku object size and file size are different, reconfigure
        size = n;
        boxSize = static_cast<int>(std::sqrt(n));
        board.assign(size, std::vector<int>(size, 0));
    }

    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            int val;
            in >> val;
            if (!in) return false;
            setValue(r, c, val);
        }
    }
    return true;
}

bool Sudoku::isValid(int row, int col, int val) const {
    for (int c = 0; c < size; ++c)
        if (c != col && board[row][c] == val)
            return false;

    for (int r = 0; r < size; ++r)
        if (r != row && board[r][col] == val)
            return false;

    int startRow = row - row % boxSize;
    int startCol = col - col % boxSize;
    for (int r = 0; r < boxSize; ++r)
        for (int c = 0; c < boxSize; ++c) {
            int curRow = startRow + r;
            int curCol = startCol + c;
            if ((curRow != row || curCol != col) && board[curRow][curCol] == val)
                return false;
        }

    return true;
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
    for (int i = 0; i < size; ++i) {
        if (i % boxSize == 0 && i != 0) {
            for (int k = 0; k < size + boxSize - 1; ++k)
                std::cout << "- ";
            std::cout << "\n";
        }
        for (int j = 0; j < size; ++j) {
            if (j % boxSize == 0 && j != 0)
                std::cout << "| ";
            std::cout << board[i][j] << ' ';
        }
        std::cout << "\n";
    }
}
