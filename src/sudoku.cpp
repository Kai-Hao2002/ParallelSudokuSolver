#include "sudoku.hpp"
#include <iostream>

SudokuSolver::SudokuSolver(const std::vector<std::vector<int>>& b) : board(b) {}

bool SudokuSolver::is_valid(int row, int col, int num) const {
    for (int i = 0; i < 9; ++i) {
        if (board[row][i] == num || board[i][col] == num)
            return false;
        int r = 3 * (row / 3) + i / 3;
        int c = 3 * (col / 3) + i % 3;
        if (board[r][c] == num)
            return false;
    }
    return true;
}

bool SudokuSolver::solve_recursive(int row, int col) {
    if (row == 9) return true;
    if (col == 9) return solve_recursive(row + 1, 0);
    if (board[row][col] != 0) return solve_recursive(row, col + 1);

    for (int num = 1; num <= 9; ++num) {
        if (is_valid(row, col, num)) {
            board[row][col] = num;
            if (solve_recursive(row, col + 1))
                return true;
            board[row][col] = 0;
        }
    }
    return false;
}

bool SudokuSolver::solve_sequential() {
    return solve_recursive(0, 0);
}

bool SudokuSolver::solve_parallel() {
    // Placeholder: implement this with Taskflow later
    return solve_sequential();
}

void SudokuSolver::print() const {
    for (const auto& row : board) {
        for (int val : row)
            std::cout << val << " ";
        std::cout << "\n";
    }
}

const std::vector<std::vector<int>>& SudokuSolver::get_board() const {
    return board;
}
