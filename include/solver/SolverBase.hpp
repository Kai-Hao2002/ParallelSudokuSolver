//include/slover/SolverBase.hpp
#pragma once

#include "core/Sudoku.hpp"
#include <vector>
#include <memory> 

class SolverBase {
public:
    virtual bool solve(Sudoku& sudoku) = 0;

    // TODO solveAll
    virtual bool solveAll(Sudoku& sudoku, std::vector<Sudoku>& allSolutions) {
        return false;
    }
    virtual std::unique_ptr<SolverBase> clone() const = 0;

    virtual ~SolverBase() = default;
};
