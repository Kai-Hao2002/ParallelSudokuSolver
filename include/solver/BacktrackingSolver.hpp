//include/slover/BacktrackingSolver.hpp
#pragma once

#include "SolverBase.hpp"

class BacktrackingSolver : public SolverBase {
public:
    bool solve(Sudoku& sudoku) override;
    bool solveAll(Sudoku& sudoku, std::vector<Sudoku>& allSolutions) override;
    explicit BacktrackingSolver(int numThreads);
    BacktrackingSolver() : BacktrackingSolver(1) {} 
    std::unique_ptr<SolverBase> clone() const override {
        return std::make_unique<BacktrackingSolver>(*this);
    }

private:
    bool backtrack(Sudoku& board, const std::vector<std::pair<int, int>>& emptyCells, size_t index);
    void backtrackAll(Sudoku& board, const std::vector<std::pair<int, int>>& emptyCells,
                      size_t index, std::vector<Sudoku>& solutions);
};
