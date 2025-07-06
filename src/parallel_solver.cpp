#include "parallel_solver.hpp"

ParallelSudokuSolver::ParallelSudokuSolver(const std::vector<std::vector<int>>& puzzle)
    : base_solver(puzzle), found_solution(false) {}

bool ParallelSudokuSolver::solve() {
    tf::Taskflow tf;
    tf::Executor executor;

    expand_task_tree(tf, base_solver, 2); // depth limit can change
    executor.run(tf).wait();

    return found_solution.load();
}

void ParallelSudokuSolver::expand_task_tree(tf::Taskflow& tf, SudokuSolver& solver, int depth_limit) {
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
        tf.emplace([=, &tf]() {
            if (found_solution.load()) return;

            SudokuSolver branch_solver = solver;
            branch_solver.set_cell(row, col, val);

            if (depth_limit > 0) {
                tf::Taskflow sub_tf;
                expand_task_tree(sub_tf, branch_solver, depth_limit - 1);
                tf::Executor exec; 
                exec.run(sub_tf).wait();
            } else {
                if (branch_solver.solve_sequential() && !found_solution.exchange(true)) {
                    solution = branch_solver.get_grid();
                }
            }
        });
    }
}

void ParallelSudokuSolver::print() const {
    if (!solution.has_value()) {
        std::cout << "No solution found.\n";
        return;
    }
    std::cout << "Solution:\n";
    for (const auto& row : solution.value()) {
        for (int val : row) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }
}

std::optional<std::vector<std::vector<int>>> ParallelSudokuSolver::get_solution() const {
    return solution;
}
