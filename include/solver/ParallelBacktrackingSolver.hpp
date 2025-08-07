//include/solver/ParallelBacktrackingSolver.hpp
#pragma once
#include "SolverBase.hpp"
#include "core/Sudoku.hpp"
#include <taskflow/taskflow.hpp>
#include <vector>

class ParallelBacktrackingSolver : public SolverBase {
public:
    ParallelBacktrackingSolver(int numThreads = 2);

    bool solve(Sudoku& sudoku) override;
    bool solveAll(Sudoku& sudoku, std::vector<Sudoku>& allSolutions) override;
    std::unique_ptr<SolverBase> clone() const override {
        return std::make_unique<ParallelBacktrackingSolver>(*this);
    }     
private:
    int numThreads_;

    bool parallelBacktrack(Sudoku& board, 
                           const std::vector<std::pair<int, int>>& emptyCells,
                           size_t index);

    void parallelBacktrackAll(Sudoku& board,
                              const std::vector<std::pair<int, int>>& emptyCells,
                              size_t index,
                              std::vector<Sudoku>& solutions,
                              tf::Taskflow& taskflow,
                              tf::Executor& executor);

};
