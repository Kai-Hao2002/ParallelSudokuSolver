//include/benchmark/BenchmarkManager.hpp
#pragma once

#include <vector>
#include <string>
#include <memory>               
#include "../core/Sudoku.hpp"
#include "../solver/SolverBase.hpp"

//Structure to store the results of each test
struct BenchmarkResult {
    std::string solverName;
    std::string puzzleName;
    double timeMs;
    bool success;
};

class BenchmarkManager {
public:
    // Constructor: pass in all test file paths
    explicit BenchmarkManager(const std::vector<std::string>& puzzlePaths);

    // Add a solver and its name
    void addSolver(const std::string& name, std::unique_ptr<SolverBase> solver);

    // Execute all test and problem solver benchmarks
    void runBenchmarks();

    // Output results to CSV file
    void exportResultsCSV(const std::string& path) const;

private:
    std::vector<std::string> puzzlePaths;
    std::vector<std::pair<std::string, std::unique_ptr<SolverBase>>> solvers;
    std::vector<BenchmarkResult> results;

    // Single test Benchmark execution
    void benchmarkSinglePuzzle(const std::string& puzzlePath);
};
