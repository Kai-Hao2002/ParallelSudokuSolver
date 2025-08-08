// tests/test_bruteforce.cpp
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "core/Sudoku.hpp"
#include "solver/BruteForceSolver.hpp"
#include "solver/ParallelBruteForceSolver.hpp"
#include <filesystem>

using namespace std;

TEST_CASE("BruteForceSolver solves small puzzles", "[bruteforce]") {
    vector<string> files = {
        "../Test_Cases/9x9_easy.txt"  
    };

    for (const auto& file : files) {
        Sudoku puzzle;
        REQUIRE(puzzle.loadFromFile(file));  

        SECTION("Solving: " + file) {
            BruteForceSolver solver(1);  
            REQUIRE(solver.solve(puzzle));
            REQUIRE(puzzle.isComplete());

            int size = puzzle.getSize();
            for (int row = 0; row < size; ++row) {
                for (int col = 0; col < size; ++col) {
                    int val = puzzle.getValue(row, col);
                    REQUIRE(puzzle.isValid(row, col, val));  
                }
            }
        }
    }
}

TEST_CASE("ParallelBruteForceSolver solves small puzzles", "[bruteforce]") {
    vector<string> files = {
        "../Test_Cases/9x9_easy.txt"  
    };

    for (const auto& file : files) {
        Sudoku puzzle;
        REQUIRE(puzzle.loadFromFile(file));  

        SECTION("Solving: " + file) {
            ParallelBruteForceSolver solver(16);  
            REQUIRE(solver.solve(puzzle));
            REQUIRE(puzzle.isComplete());

            int size = puzzle.getSize();
            for (int row = 0; row < size; ++row) {
                for (int col = 0; col < size; ++col) {
                    int val = puzzle.getValue(row, col);
                    REQUIRE(puzzle.isValid(row, col, val));  
                }
            }
        }
    }
}
