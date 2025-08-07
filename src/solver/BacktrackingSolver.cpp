//src/solver/BacktrackingSolver.cpp
#include "solver/BacktrackingSolver.hpp"
#include <iostream>

bool BacktrackingSolver::solve(Sudoku& sudoku) {
    auto emptyCells = sudoku.getEmptyCells();
    return backtrack(sudoku, emptyCells, 0);
}

bool BacktrackingSolver::solveAll(Sudoku& sudoku, std::vector<Sudoku>& allSolutions) {
    auto emptyCells = sudoku.getEmptyCells();
    backtrackAll(sudoku, emptyCells, 0, allSolutions);
    return !allSolutions.empty();
}

bool BacktrackingSolver::backtrack(Sudoku& board,
                                   const std::vector<std::pair<int, int>>& emptyCells,
                                   size_t index) {
    if (index == emptyCells.size()) return true;

    auto [row, col] = emptyCells[index];
    for (int val = 1; val <= board.getSize(); ++val) {
        if (board.isValid(row, col, val)) {
            board.setValue(row, col, val);
            if (backtrack(board, emptyCells, index + 1)) return true;
            board.setValue(row, col, 0);
        }
    }
    return false;
}

void BacktrackingSolver::backtrackAll(Sudoku& board,
                                      const std::vector<std::pair<int, int>>& emptyCells,
                                      size_t index, std::vector<Sudoku>& solutions) {
    if (index == emptyCells.size()) {
        solutions.push_back(board);
        return;
    }

    auto [row, col] = emptyCells[index];
    for (int val = 1; val <= board.getSize(); ++val) {
        if (board.isValid(row, col, val)) {
            board.setValue(row, col, val);
            backtrackAll(board, emptyCells, index + 1, solutions);
            board.setValue(row, col, 0);
        }
    }
}
BacktrackingSolver::BacktrackingSolver(int numThreads) {
    std::cout << "BacktrackingSolver using " << numThreads << " threads .\n";
    // 你可以在這裡儲存 numThreads 或設置執行緒池等等
}