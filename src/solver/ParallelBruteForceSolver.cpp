//src/solver/ParallelBruteForceSolver.cpp
#include "solver/ParallelBruteForceSolver.hpp"
#include <mutex>

ParallelBruteForceSolver::ParallelBruteForceSolver(int numThreads)
    : numThreads_(numThreads) {
        std::cout << "ParallelBruteForceSolver using " << numThreads << " threads .\n";
    }

bool ParallelBruteForceSolver::solve(Sudoku& sudoku) {
    auto emptyCells = sudoku.getEmptyCells();
    if (emptyCells.empty()) return true;

    tf::Taskflow taskflow;
    tf::Executor executor(numThreads_);

    std::atomic<bool> solved(false);
    std::mutex mtx;  // Protect sudoku

    auto size = sudoku.getSize();

    std::function<bool(Sudoku&, size_t)> bruteForce = [&](Sudoku& board, size_t index) {
        if (solved.load()) return true;
        if (index == emptyCells.size()) return true;

        auto [row, col] = emptyCells[index];
        for (int val = 1; val <= size; ++val) {
            if (solved.load()) return true;
            if (board.isValid(row, col, val)) {
                board.setValue(row, col, val);
                if (bruteForce(board, index + 1)) return true;
                board.setValue(row, col, 0);
            }
        }
        return false;
    };

    for (int val = 1; val <= size; ++val) {
        if (sudoku.isValid(emptyCells[0].first, emptyCells[0].second, val)) {
            Sudoku boardCopy = sudoku;
            boardCopy.setValue(emptyCells[0].first, emptyCells[0].second, val);

            taskflow.emplace([&sudoku, &bruteForce, &solved, &mtx, boardCopy]() mutable {
                if (bruteForce(boardCopy, 1)) {
                    if (!solved.exchange(true)) { // The first task to find a solution is executed
                        std::lock_guard<std::mutex> lock(mtx);
                        sudoku = boardCopy; //Copy the solution back to the passed parameter
                    }
                }
            });
        }
    }

    executor.run(taskflow).wait();

    return solved.load();
}
