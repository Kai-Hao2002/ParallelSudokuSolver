//include/core/Sudoku.hpp
#pragma once

#include <vector>
#include <string>

class Sudoku {
public:
    Sudoku(int size = 9);

    bool loadFromFile(const std::string& filename);
    bool isValid(int row, int col, int val) const;
    bool isComplete() const;
    void setValue(int row, int col, int val);
    int getValue(int row, int col) const;

    int getSize() const;
    const std::vector<std::vector<int>>& getBoard() const;
    std::vector<std::pair<int, int>> getEmptyCells() const;
    void print() const;

private:
    int size;
    int boxSize; // e.g., 3 for 9x9
    std::vector<std::vector<int>> board;

    bool isRowValid(int row, int val) const;
    bool isColValid(int col, int val) const;
    bool isBoxValid(int startRow, int startCol, int val) const;
};
