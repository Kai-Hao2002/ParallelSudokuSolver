#pragma once

#include <vector>
#include <set>
#include <optional>
#include <tuple>
#include <climits> 

class SudokuSolver {
public:
    SudokuSolver(const std::vector<std::vector<int>>& puzzle);

    // single solution
    bool solve_sequential();

    // all_solutions 
    void solve_all_sequential(std::vector<std::vector<std::vector<int>>>& all_solutions, int max_solutions = INT_MAX);

    void print() const;
    const std::vector<std::vector<int>>& get_grid() const { return grid; }

    const std::set<int>& get_candidates(int row, int col) const { return candidates[row][col]; }
    void set_cell(int row, int col, int value);

    std::optional<std::pair<int, int>> find_mrv_cell() const;
    bool is_valid(int row, int col, int value) const;

private:
    std::vector<std::vector<int>> grid;
    std::vector<std::vector<std::set<int>>> candidates;

    void initialize_candidates();

    bool forward_check(int row, int col, int val, std::vector<std::tuple<int, int, int>>& changes);
    void restore_candidates(const std::vector<std::tuple<int, int, int>>& changes);

    void solve_all_helper(std::vector<std::vector<std::vector<int>>>& all_solutions, int max_solutions);
};
