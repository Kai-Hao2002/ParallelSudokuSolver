// tests/test_dlx.cpp
#define CATCH_CONFIG_MAIN
#include <catch2/catch_all.hpp>
#include "core/Sudoku.hpp"
#include "solver/DLXSolver.hpp"
#include "solver/ParallelDLXSolver.hpp"
#include <filesystem>
using namespace std;

TEST_CASE("DLX Solver solves all puzzles", "[dlx]") {
    vector<string> files = {
        "../Test_Cases/9x9_easy.txt",
        "../Test_Cases/16x16_easy.txt",
        "../Test_Cases/25x25_easy.txt"
    };

    for (const auto& file : files) {
        Sudoku puzzle;
        REQUIRE(puzzle.loadFromFile(file));

        SECTION("Solving: " + file) {
            DLXSolver solver(1);
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

TEST_CASE("DLX Solver fails on invalid puzzles", "[dlx][invalid]") {
    vector<string> invalidFiles = {
        "../Test_Cases/invalidRows.txt",
        "../Test_Cases/invalidColumns.txt",
        "../Test_Cases/invalidBoxes.txt"
    };

    for (const auto& file : invalidFiles) {
        Sudoku puzzle;
        REQUIRE(puzzle.loadFromFile(file));

        SECTION("Invalid Puzzle: " + file) {
            DLXSolver solver(1);
            REQUIRE_FALSE(solver.solve(puzzle));
        }
    }
}


TEST_CASE("ParallelDLX Solver solves all puzzles", "[dlx]") {
    vector<string> files = {
        "../Test_Cases/9x9_easy.txt",
        "../Test_Cases/16x16_easy.txt",
        "../Test_Cases/25x25_easy.txt"
    };

    for (const auto& file : files) {
        Sudoku puzzle;
        REQUIRE(puzzle.loadFromFile(file));

        SECTION("Solving: " + file) {
            ParallelDLXSolver solver(16);
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

TEST_CASE("ParallelDLX Solver fails on invalid puzzles", "[dlx][invalid]") {
    vector<string> invalidFiles = {
        "../Test_Cases/invalidRows.txt",
        "../Test_Cases/invalidColumns.txt",
        "../Test_Cases/invalidBoxes.txt"
    };

    for (const auto& file : invalidFiles) {
        Sudoku puzzle;
        REQUIRE(puzzle.loadFromFile(file));

        SECTION("Invalid Puzzle: " + file) {
            ParallelDLXSolver solver(16);
            REQUIRE_FALSE(solver.solve(puzzle));
        }
    }
}
