//include/slover/BruteForceSolver.hpp
#pragma once

#include "SolverBase.hpp"

class BruteForceSolver : public SolverBase {
public:
    bool solve(Sudoku& sudoku) override;
    explicit BruteForceSolver(int numThreads);
    std::unique_ptr<SolverBase> clone() const override {
        return std::make_unique<BruteForceSolver>(*this);
    }

};
