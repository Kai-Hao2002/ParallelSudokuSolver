#pragma once

#include <vector>
#include <string>

class SudokuSolver {
public:
    SudokuSolver(const std::vector<std::vector<int>>& board);
    bool solve_sequential(); // baseline
    bool solve_parallel();   // parallel version
    void print() const;
    const std::vector<std::vector<int>>& get_board() const;

private:
    std::vector<std::vector<int>> board;

    bool is_valid(int row, int col, int num) const;
    bool solve_recursive(int row, int col);
};
