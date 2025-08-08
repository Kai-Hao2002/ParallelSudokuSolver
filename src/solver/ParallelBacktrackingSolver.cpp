//src/solver/ParallelBacktrackingSolver.cpp
#include "solver/ParallelBacktrackingSolver.hpp"
#include <mutex>

ParallelBacktrackingSolver::ParallelBacktrackingSolver(int numThreads)
    : numThreads_(numThreads) {
        std::cout << "ParallelBacktrackingSolver using " << numThreads << " threads .\n";
    }

bool ParallelBacktrackingSolver::solve(Sudoku& sudoku) {
    auto emptyCells = sudoku.getEmptyCells();

    tf::Taskflow taskflow;
    tf::Executor executor(numThreads_);

    std::atomic<bool> solved(false);

    std::mutex mtx; // Protect the Sudoku instance
    Sudoku sudokuCopy = sudoku; // Avoid direct manipulation of the original chessboard

    std::function<void(size_t, Sudoku&)> backtrack = [&](size_t index, Sudoku& board) {
        if (solved.load()) return;
        if (index == emptyCells.size()) {
            std::lock_guard<std::mutex> lock(mtx);
            sudoku = board; // Copy the result back to the original disk
            solved.store(true);
            return;
        }

        auto [row, col] = emptyCells[index];
        for (int val = 1; val <= board.getSize(); ++val) {
            if (solved.load()) return;

            if (board.isValid(row, col, val)) {
                board.setValue(row, col, val);
                backtrack(index + 1, board);
                board.setValue(row, col, 0);
            }
        }
    };

    // Parallelize the first layer of branches and create multiple tasks
    for (int val = 1; val <= sudokuCopy.getSize(); ++val) {
        if (sudokuCopy.isValid(emptyCells[0].first, emptyCells[0].second, val)) {
            Sudoku boardCopy = sudokuCopy;
            boardCopy.setValue(emptyCells[0].first, emptyCells[0].second, val);

            taskflow.emplace([boardCopy, &backtrack, &emptyCells]() mutable {
                backtrack(1, boardCopy);
            });
        }
    }

    executor.run(taskflow).wait();

    return solved.load();
}

bool ParallelBacktrackingSolver::solveAll(Sudoku& sudoku, std::vector<Sudoku>& allSolutions) {
    auto emptyCells = sudoku.getEmptyCells();

    tf::Taskflow taskflow;
    tf::Executor executor(numThreads_);

    std::mutex mtx;

    for (int val = 1; val <= sudoku.getSize(); ++val) {
        if (sudoku.isValid(emptyCells[0].first, emptyCells[0].second, val)) {
            Sudoku boardCopy = sudoku;
            boardCopy.setValue(emptyCells[0].first, emptyCells[0].second, val);

            taskflow.emplace([boardCopy, &emptyCells, &allSolutions, &mtx]() mutable {
                std::function<void(size_t, Sudoku&)> backtrackAll;
                backtrackAll = [&](size_t index, Sudoku& board) {
                    if (index == emptyCells.size()) {
                        std::lock_guard<std::mutex> lock(mtx);
                        allSolutions.push_back(board);
                        return;
                    }
                    auto [row, col] = emptyCells[index];
                    for (int v = 1; v <= board.getSize(); ++v) {
                        if (board.isValid(row, col, v)) {
                            board.setValue(row, col, v);
                            backtrackAll(index + 1, board);
                            board.setValue(row, col, 0);
                        }
                    }
                };
                backtrackAll(1, boardCopy);
            });
        }
    }

    executor.run(taskflow).wait();

    return !allSolutions.empty();
}
