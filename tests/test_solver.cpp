#include <gtest/gtest.h>
#include "sudoku.hpp"
#include "parallel_solver.hpp"

bool is_valid_solution(const std::vector<std::vector<int>>& grid) {
    for (int i = 0; i < 9; ++i) {
        std::set<int> row, col;
        for (int j = 0; j < 9; ++j) {
            if (!row.insert(grid[i][j]).second || !col.insert(grid[j][i]).second)
                return false;
        }
    }
    for (int r = 0; r < 9; r += 3) {
        for (int c = 0; c < 9; c += 3) {
            std::set<int> block;
            for (int i = 0; i < 3; ++i)
                for (int j = 0; j < 3; ++j)
                    if (!block.insert(grid[r + i][c + j]).second)
                        return false;
        }
    }
    return true;
}

void test_puzzle(const std::vector<std::vector<int>>& puzzle, bool should_have_solution) {
    {
        SudokuSolver solver(puzzle);
        bool solved = solver.solve_sequential();
        EXPECT_EQ(solved, should_have_solution);
        if (should_have_solution) {
            EXPECT_TRUE(is_valid_solution(solver.get_grid()));
        }
    }
    {
        ParallelSudokuSolver solver(puzzle);
        bool solved = solver.solve_single();
        EXPECT_EQ(solved, should_have_solution);
        auto result = solver.get_solution();
        if (should_have_solution) {
            ASSERT_TRUE(result.has_value());
            EXPECT_TRUE(is_valid_solution(result.value()));
        } else {
            EXPECT_FALSE(result.has_value());
        }
    }
}

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
    test_puzzle(puzzle, true);
}

TEST(SudokuSolverTest, MediumPuzzle) {
    std::vector<std::vector<int>> puzzle = {
        {0,0,0,0,6,0,0,0,3},
        {0,0,0,1,0,0,0,7,0},
        {0,0,0,0,0,5,0,0,9},
        {0,0,0,7,0,0,0,0,0},
        {4,0,9,0,0,0,2,0,5},
        {0,0,0,0,0,6,0,0,0},
        {2,0,0,3,0,0,0,0,0},
        {0,9,0,0,0,7,0,0,0},
        {1,0,0,0,8,0,0,0,0}
    };
    test_puzzle(puzzle, true);
}



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
    test_puzzle(puzzle, true);
}

TEST(SudokuSolverTest, NoSolutionPuzzle) {
    std::vector<std::vector<int>> puzzle = {
        {5,5,0,0,7,0,0,0,0}, // invalid: two 5s
        {6,0,0,1,9,5,0,0,0},
        {0,9,8,0,0,0,0,6,0},
        {8,0,0,0,6,0,0,0,3},
        {4,0,0,8,0,3,0,0,1},
        {7,0,0,0,2,0,0,0,6},
        {0,6,0,0,0,0,2,8,0},
        {0,0,0,4,1,9,0,0,5},
        {0,0,0,0,8,0,0,7,9}
    };
    test_puzzle(puzzle, false);
}
