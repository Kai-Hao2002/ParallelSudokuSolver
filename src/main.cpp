// src/main.cpp
#include <memory>
#include <iostream>
#include "core/Sudoku.hpp"
#include "io/FileIO.hpp"
#include "io/ArgumentParser.hpp"
#include "solver/BacktrackingSolver.hpp"
#include "solver/BruteForceSolver.hpp"
#include "solver/DLXSolver.hpp"
#include "solver/ParallelBacktrackingSolver.hpp"
#include "solver/ParallelBruteForceSolver.hpp"
#include "solver/ParallelDLXSolver.hpp"

int main(int argc, char* argv[]) {
    // parse instruction
    ParsedArgs args = ArgumentParser::parse(argc, argv);
    if (!args.valid) {
        std::cerr << "Invalid arguments.\n";
        return 1;
    }

    Sudoku sudoku;  // 9x9 default

    // read board
    if (!FileIO::loadSudokuFromFile(sudoku, args.inputFile)) {
        std::cerr << "Failed to load sudoku from file: " << args.inputFile << "\n";
        return 1;
    }

    std::cout << "Input Sudoku:\n";
    sudoku.print();

    // choose sudoku solver
    std::unique_ptr<SolverBase> solver;
    switch (args.mode) {
        case 0:
            solver = std::make_unique<BacktrackingSolver>(args.numThreads);
            break;
        case 1:
            solver = std::make_unique<BruteForceSolver>(args.numThreads);
            break;
        case 2:
            solver = std::make_unique<DLXSolver>(args.numThreads);
            break;
        case 3:
            solver = std::make_unique<ParallelBacktrackingSolver>(args.numThreads);
            break;
        case 4:
            solver = std::make_unique<ParallelBruteForceSolver>(args.numThreads);
            break;
        case 5:
            solver = std::make_unique<ParallelDLXSolver>(args.numThreads);
            break;
        default:
            std::cerr << "Unknown solver mode.\n";
            return 1;
    }

    if (!solver->solve(sudoku)) {
        std::cerr << "Failed to solve sudoku.\n";
        return 1;
    }

    std::cout << "Solved Sudoku:\n";
    sudoku.print();

    if (args.writeToFile) {
        if (!FileIO::writeSolutionToFile(sudoku, "solution.txt")) {
            std::cerr << "Failed to write solution to file.\n";
            return 1;
        }
    }

    return 0;
}
