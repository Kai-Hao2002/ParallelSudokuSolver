// tests/test_backtracking.cpp
#include <catch2/catch_all.hpp>
#include "solver/BacktrackingSolver.hpp"
#include "solver/ParallelBacktrackingSolver.hpp"
#include "core/Sudoku.hpp"

TEST_CASE("BacktrackingSolver solves valid puzzles", "[backtracking]") {
    std::vector<std::string> files = {
        "../Test_Cases/9x9_easy.txt",
        "../Test_Cases/16x16_easy.txt",
        "../Test_Cases/25x25_easy.txt"
    };

    for (const auto& file : files) {
        SECTION("Solving " + file) {
            Sudoku sudoku;
            REQUIRE(sudoku.loadFromFile(file));  // ensure file loads

            CAPTURE(file);
            BacktrackingSolver solver(1);  // Use 1 thread
            REQUIRE(solver.solve(sudoku));  // should solve
            REQUIRE(sudoku.isComplete());   // should be complete
        }
    }
}

TEST_CASE("BacktrackingSolver detects unsolvable puzzles", "[backtracking][invalid]") {
    std::vector<std::string> files = {
        "../Test_Cases/invalidRows.txt",
        "../Test_Cases/invalidColumns.txt",
        "../Test_Cases/invalidBoxes.txt"
    };

    for (const auto& file : files) {
        SECTION("Solving " + file) {
            Sudoku sudoku;
            REQUIRE(sudoku.loadFromFile(file));  // still loads fine

            CAPTURE(file);
            BacktrackingSolver solver(1);  // Use 1 thread
            REQUIRE_FALSE(solver.solve(sudoku));  // should not solve
        }
    }
}


TEST_CASE("ParallelBacktrackingSolver solves valid puzzles", "[backtracking]") {
    std::vector<std::string> files = {
        "../Test_Cases/9x9_easy.txt",
        "../Test_Cases/16x16_easy.txt",
        "../Test_Cases/25x25_easy.txt"
    };

    for (const auto& file : files) {
        SECTION("Solving " + file) {
            Sudoku sudoku;
            REQUIRE(sudoku.loadFromFile(file));  // ensure file loads

            CAPTURE(file);
            ParallelBacktrackingSolver solver(16);  // Use 1 thread
            REQUIRE(solver.solve(sudoku));  // should solve
            REQUIRE(sudoku.isComplete());   // should be complete
        }
    }
}

TEST_CASE("ParallelBacktrackingSolver detects unsolvable puzzles", "[backtracking][invalid]") {
    std::vector<std::string> files = {
        "../Test_Cases/invalidRows.txt",
        "../Test_Cases/invalidColumns.txt",
        "../Test_Cases/invalidBoxes.txt"
    };

    for (const auto& file : files) {
        SECTION("Solving " + file) {
            Sudoku sudoku;
            REQUIRE(sudoku.loadFromFile(file));  // still loads fine

            CAPTURE(file);
            ParallelBacktrackingSolver solver(16);  // Use 1 thread
            REQUIRE_FALSE(solver.solve(sudoku));  // should not solve
        }
    }
}
