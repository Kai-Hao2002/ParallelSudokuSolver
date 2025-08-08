//src/benchmark/BenchmarkManager.cpp
#include "benchmark/BenchmarkManager.hpp"
#include "io/FileIO.hpp"
#include <chrono>
#include <iostream>
#include <fstream>

BenchmarkManager::BenchmarkManager(const std::vector<std::string>& puzzlePaths)
    : puzzlePaths(puzzlePaths) {}

void BenchmarkManager::addSolver(const std::string& name, std::unique_ptr<SolverBase> solver) {
    solvers.emplace_back(name, std::move(solver));
}

void BenchmarkManager::runBenchmarks() {
    for (const auto& puzzlePath : puzzlePaths) {
        benchmarkSinglePuzzle(puzzlePath);
    }
}

void BenchmarkManager::benchmarkSinglePuzzle(const std::string& puzzlePath) {
    Sudoku puzzle;
    if (!FileIO::loadSudokuFromFile(puzzle, puzzlePath)) {
        std::cerr << "Failed to load puzzle from file: " << puzzlePath << std::endl;
        return;
    }

    for (auto& [solverName, solver] : solvers) {
        Sudoku sudoku = puzzle; // copy
        auto start = std::chrono::high_resolution_clock::now();
        bool success = solver->solve(sudoku);
        auto end = std::chrono::high_resolution_clock::now();
        double timeMs = std::chrono::duration<double, std::milli>(end - start).count();

        results.push_back({solverName, puzzlePath, timeMs, success});
        std::cout << solverName << " solved " << puzzlePath << " in " << timeMs << " ms"
                  << (success ? " ✔" : " ✘") << std::endl;
    }
}

void BenchmarkManager::exportResultsCSV(const std::string& path) const {
    std::ofstream out(path);
    out << "Solver,Puzzle,TimeMs,Success\n";
    for (const auto& r : results) {
        out << r.solverName << "," << r.puzzleName << "," << r.timeMs << "," << (r.success ? "true" : "false") << "\n";
    }
}
