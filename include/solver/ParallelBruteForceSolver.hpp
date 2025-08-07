//include/solver/ParallelBruteForceSolver.hpp
#pragma once

#include "SolverBase.hpp"
#include "core/Sudoku.hpp"
#include <taskflow/taskflow.hpp>

class ParallelBruteForceSolver : public SolverBase {
public:
    ParallelBruteForceSolver(int numThreads = 2);

    bool solve(Sudoku& sudoku) override;
    std::unique_ptr<SolverBase> clone() const override {
        return std::make_unique<ParallelBruteForceSolver>(*this);
    }  
private:
    int numThreads_;
};
