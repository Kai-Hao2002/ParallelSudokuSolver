#include "parallel_solver.hpp"
#include <mutex>

ParallelSudokuSolver::ParallelSudokuSolver(const std::vector<std::vector<int>>& puzzle)
    : base_solver(puzzle), found_solution(false) {}

bool ParallelSudokuSolver::solve_single() {
    tf::Taskflow tf;
    expand_task_tree_single(tf, base_solver, 2);
    tf::Executor executor{std::min(4u, std::thread::hardware_concurrency())}; // limit threads
    executor.run(tf).wait();
    return found_solution.load();
}

void ParallelSudokuSolver::expand_task_tree_single(tf::Taskflow& tf, SudokuSolver solver, int depth_limit) {
    if (found_solution.load()) return;

    auto pos_opt = solver.find_mrv_cell();
    if (!pos_opt.has_value()) {
        if (solver.solve_sequential()) {
            if (!found_solution.exchange(true)) {
                solution = solver.get_grid();
            }
        }
        return;
    }

    auto [row, col] = pos_opt.value();
    const auto& candidates = solver.get_candidates(row, col);

    for (int val : candidates) {
        tf.emplace([=]() mutable {
            if (found_solution.load()) return;
            try {
                SudokuSolver branch_solver = solver;
                branch_solver.set_cell(row, col, val);
                if (depth_limit > 0) {
                    tf::Taskflow sub_tf;
                    expand_task_tree_single(sub_tf, branch_solver, depth_limit - 1);
                    tf::Executor executor{1};
                    executor.run(sub_tf).wait();
                } else {
                    if (branch_solver.solve_sequential() && !found_solution.exchange(true)) {
                        solution = branch_solver.get_grid();
                    }
                }
            } catch (...) {
                // optional: log error
            }
        });
    }
}

void ParallelSudokuSolver::solve_all(std::vector<std::vector<std::vector<int>>>& all_solutions, size_t max_solutions) {
    tf::Taskflow tf;
    std::mutex result_mutex;
    std::atomic<size_t> solution_count{0};
    expand_task_tree_all(tf, base_solver, 2, all_solutions, result_mutex, solution_count, max_solutions);
    tf::Executor executor{std::min(4u, std::thread::hardware_concurrency())};
    executor.run(tf).wait();
}

void ParallelSudokuSolver::expand_task_tree_all(
    tf::Taskflow& tf,
    SudokuSolver solver,
    int depth_limit,
    std::vector<std::vector<std::vector<int>>>& results,
    std::mutex& result_mutex,
    std::atomic<size_t>& solution_count,
    size_t max_solutions
) {
    if (solution_count.load() >= max_solutions) return;

    auto pos_opt = solver.find_mrv_cell();
    if (!pos_opt.has_value()) {
        std::lock_guard<std::mutex> lock(result_mutex);
        if (solution_count.load() < max_solutions) {
            results.push_back(solver.get_grid());
            ++solution_count;
        }
        return;
    }

    auto [row, col] = pos_opt.value();
    const auto& candidates = solver.get_candidates(row, col);

    for (int val : candidates) {
        tf.emplace([=, &results, &result_mutex, &solution_count]() mutable {
            if (solution_count.load() >= max_solutions) return;

            try {
                SudokuSolver branch_solver = solver;
                branch_solver.set_cell(row, col, val);

                if (depth_limit > 0) {
                    tf::Taskflow sub_tf;
                    expand_task_tree_all(sub_tf, branch_solver, depth_limit - 1, results, result_mutex, solution_count, max_solutions);
                    tf::Executor executor{1};
                    executor.run(sub_tf).wait();
                } else {
                    std::vector<std::vector<std::vector<int>>> local_results;
                    branch_solver.solve_all_sequential(local_results, max_solutions);

                    std::lock_guard<std::mutex> lock(result_mutex);
                    for (const auto& sol : local_results) {
                        if (solution_count.load() >= max_solutions) break;
                        results.push_back(sol);
                        ++solution_count;
                    }
                }
            } catch (...) {
                // optional: log error
            }
        });
    }
}


void ParallelSudokuSolver::print() const {
    if (solution.has_value()) {
        for (const auto& row : solution.value()) {
            for (int val : row) std::cout << val << " ";
            std::cout << "\n";
        }
    } else {
        std::cout << "No solution found.\n";
    }
}

std::optional<std::vector<std::vector<int>>> ParallelSudokuSolver::get_solution() const {
    return solution;
}
