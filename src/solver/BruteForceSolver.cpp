//src/solver/BruteForceSolver.cpp
#include "solver/BruteForceSolver.hpp"
#include <functional> 
#include <iostream>

bool BruteForceSolver::solve(Sudoku& sudoku) {
    // 與 Backtracking 實作類似（也可複用）
    auto emptyCells = sudoku.getEmptyCells();

    std::function<bool(size_t)> dfs = [&](size_t index) {
        if (index == emptyCells.size()) return true;

        auto [row, col] = emptyCells[index];
        for (int val = 1; val <= sudoku.getSize(); ++val) {
            if (sudoku.isValid(row, col, val)) {
                sudoku.setValue(row, col, val);
                if (dfs(index + 1)) return true;
                sudoku.setValue(row, col, 0);
            }
        }
        return false;
    };

    return dfs(0);
}

BruteForceSolver::BruteForceSolver(int numThreads) {
    std::cout << "BruteForceSolver using " << numThreads << " threads .\n";
    // 你可以在這裡儲存 numThreads 或設置執行緒池等等
}
