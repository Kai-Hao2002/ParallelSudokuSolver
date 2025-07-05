#include <gtest/gtest.h>
#include "sudoku.hpp"

// easy
TEST(SudokuSolverTest, EasyPuzzle) {
    std::vector<std::vector<int>> puzzle = {
        {5,3,0,0,7,0,0,0,0},
        {6,0,0,1,9,5,0,0,0},
        {0,9,8,0,0,0,0,6,0},
        {8,0,0,0,6,0,0,0,3},
        {4,0,0,8,0,3,0,0,1},
        {7,0,0,0,2,0,0,0,6},
        {0,6,0,0,0,0,2,8,0},
        {0,0,0,4,1,9,0,0,5},
        {0,0,0,0,8,0,0,7,9}
    };
    SudokuSolver solver(puzzle);
    EXPECT_TRUE(solver.solve_sequential());
}

// medium
TEST(SudokuSolverTest, MediumPuzzle) {
    std::vector<std::vector<int>> puzzle = {
        {0,0,0,0,0,0,0,1,2},
        {0,0,0,0,0,0,0,0,0},
        {0,0,1,0,0,5,0,0,0},
        {0,7,0,0,0,0,2,0,0},
        {0,0,0,0,6,0,0,0,0},
        {0,0,3,0,0,0,0,5,0},
        {0,0,0,2,0,0,3,0,0},
        {0,0,0,0,0,0,0,0,0},
        {8,9,0,0,0,0,0,0,0}
    };
    SudokuSolver solver(puzzle);
    EXPECT_TRUE(solver.solve_sequential());
}

// hard
TEST(SudokuSolverTest, HardPuzzle) {
    std::vector<std::vector<int>> puzzle = {
        {8,0,0,0,0,0,0,0,0},
        {0,0,3,6,0,0,0,0,0},
        {0,7,0,0,9,0,2,0,0},
        {0,5,0,0,0,7,0,0,0},
        {0,0,0,0,4,5,7,0,0},
        {0,0,0,1,0,0,0,3,0},
        {0,0,1,0,0,0,0,6,8},
        {0,0,8,5,0,0,0,1,0},
        {0,9,0,0,0,0,4,0,0}
    };
    SudokuSolver solver(puzzle);
    EXPECT_TRUE(solver.solve_sequential());
}

// no answer
TEST(SudokuSolverTest, NoSolutionPuzzle) {
    std::vector<std::vector<int>> puzzle = {
        {5,5,0,0,7,0,0,0,0}, // the first row has two 5
        {6,0,0,1,9,5,0,0,0},
        {0,9,8,0,0,0,0,6,0},
        {8,0,0,0,6,0,0,0,3},
        {4,0,0,8,0,3,0,0,1},
        {7,0,0,0,2,0,0,0,6},
        {0,6,0,0,0,0,2,8,0},
        {0,0,0,4,1,9,0,0,5},
        {0,0,0,0,8,0,0,7,9}
    };
    SudokuSolver solver(puzzle);
    EXPECT_FALSE(solver.solve_sequential());
}
