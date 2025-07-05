#include "sudoku.hpp"
#include <iostream>
#include <algorithm>
#include <tuple>

SudokuSolver::SudokuSolver(const std::vector<std::vector<int>>& puzzle) : grid(puzzle), candidates(9, std::vector<std::set<int>>(9)) {
    initialize_candidates();
}

void SudokuSolver::initialize_candidates() {
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (grid[r][c] == 0) {
                for (int v = 1; v <= 9; ++v) {
                    if (is_valid(r, c, v)) {
                        candidates[r][c].insert(v);
                    }
                }
            }
        }
    }
}

bool SudokuSolver::is_valid(int row, int col, int val) const {
    for (int i = 0; i < 9; ++i) {
        if (grid[row][i] == val || grid[i][col] == val) return false;
    }
    int box_r = (row / 3) * 3, box_c = (col / 3) * 3;
    for (int r = box_r; r < box_r + 3; ++r)
        for (int c = box_c; c < box_c + 3; ++c)
            if (grid[r][c] == val) return false;
    return true;
}

std::optional<std::pair<int, int>> SudokuSolver::find_mrv_cell() const {
    size_t min_size = 10; 
    std::optional<std::pair<int, int>> result;
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            if (grid[r][c] == 0 && candidates[r][c].size() < min_size) {
                min_size = candidates[r][c].size();
                result = {r, c};
                if (min_size == 1) return result;  // early exit
            }
        }
    }
    return result;
}

bool SudokuSolver::forward_check(int row, int col, int val, std::vector<std::tuple<int, int, int>>& changes) {
    auto update = [&](int r, int c) {
        if (grid[r][c] != 0) return true;
        auto& cand = candidates[r][c];
        if (cand.find(val) != cand.end()) {
            cand.erase(val);
            changes.emplace_back(r, c, val);
            if (cand.empty()) return false;
        }
        return true;
    };

    for (int i = 0; i < 9; ++i) {
        if (!update(row, i) || !update(i, col)) return false;
    }

    int box_r = (row / 3) * 3, box_c = (col / 3) * 3;
    for (int r = box_r; r < box_r + 3; ++r)
        for (int c = box_c; c < box_c + 3; ++c)
            if (!update(r, c)) return false;

    return true;
}

void SudokuSolver::restore_candidates(const std::vector<std::tuple<int, int, int>>& changes) {
    for (auto [r, c, v] : changes) {
        candidates[r][c].insert(v);
    }
}

bool SudokuSolver::solve_sequential() {
    auto pos = find_mrv_cell();
    if (!pos) return true;  // done

    int row = pos->first, col = pos->second;

    for (int val : candidates[row][col]) {
        if (!is_valid(row, col, val)) continue;

        grid[row][col] = val;
        std::vector<std::tuple<int, int, int>> changes;
        if (forward_check(row, col, val, changes)) {
            if (solve_sequential()) return true;
        }
        restore_candidates(changes);
        grid[row][col] = 0;
    }

    return false;
}

void SudokuSolver::print() const {
    for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
            std::cout << grid[r][c] << " ";
        }
        std::cout << "\n";
    }
}
