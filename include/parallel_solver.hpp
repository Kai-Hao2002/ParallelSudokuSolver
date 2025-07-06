#pragma once
#include "sudoku.hpp"
#include <taskflow/taskflow.hpp>
#include <atomic>
#include <optional>
#include <vector>

class ParallelSudokuSolver {
public:
    ParallelSudokuSolver(const std::vector<std::vector<int>>& puzzle);

    // single solution
    bool solve_single();

    // all solutions
    void solve_all(std::vector<std::vector<std::vector<int>>>& all_solutions, size_t max_solutions = SIZE_MAX);
    std::optional<std::vector<std::vector<int>>> get_solution() const;  // 放 public

    void print() const;

private:
    SudokuSolver base_solver;
    std::optional<std::vector<std::vector<int>>> solution;
    std::atomic<bool> found_solution;

    void expand_task_tree_single(tf::Taskflow& tf, SudokuSolver solver, int depth_limit);
    void expand_task_tree_all(
        tf::Taskflow& tf,
        SudokuSolver solver,
        int depth_limit,
        std::vector<std::vector<std::vector<int>>>& results,
        std::mutex& result_mutex,
        std::atomic<size_t>& solution_count,
        size_t max_solutions
    );

};
