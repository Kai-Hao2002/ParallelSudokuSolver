#pragma once

#include "sudoku.hpp"
#include <taskflow/taskflow.hpp>
#include <atomic>
#include <optional>
#include <vector>
#include <iostream>

class ParallelSudokuSolver {
public:
    ParallelSudokuSolver(const std::vector<std::vector<int>>& puzzle);
    bool solve();
    void print() const;
    std::optional<std::vector<std::vector<int>>> get_solution() const;

private:
    SudokuSolver base_solver;
    std::optional<std::vector<std::vector<int>>> solution;
    std::atomic<bool> found_solution;

    void expand_task_tree(tf::Taskflow& tf, SudokuSolver& solver, int depth_limit);
};
