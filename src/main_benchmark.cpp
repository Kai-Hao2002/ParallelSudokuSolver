// src/main_benchmark.cpp

#include "benchmark/BenchmarkManager.hpp"
#include "solver/BacktrackingSolver.hpp"
#include "solver/ParallelBacktrackingSolver.hpp"
#include "solver/BruteForceSolver.hpp"
#include "solver/ParallelBruteForceSolver.hpp"
#include "solver/DLXSolver.hpp"
#include "solver/ParallelDLXSolver.hpp"
#include <iostream>

int main() {
    std::vector<std::string> testFiles = {
        "../Test_Cases/9x9_easy.txt",
        "../Test_Cases/9x9_medium.txt",
        "../Test_Cases/9x9_hard.txt",
        "../Test_Cases/16x16_easy.txt",
        "../Test_Cases/16x16_medium.txt",
        "../Test_Cases/16x16_hard.txt",
        "../Test_Cases/25x25_easy.txt",
        "../Test_Cases/25x25_medium.txt",
        "../Test_Cases/25x25_hard.txt"
    };


    BenchmarkManager manager(testFiles);

    int numThreads = std::thread::hardware_concurrency(); // Automatically detect the number of CPU cores, or set it to a fixed value

    manager.addSolver("Backtracking", std::make_unique<BacktrackingSolver>(1));
    manager.addSolver("ParallelBacktracking", std::make_unique<ParallelBacktrackingSolver>(numThreads));
    manager.addSolver("BruteForce", std::make_unique<BruteForceSolver>(1));
    manager.addSolver("ParallelBruteForce", std::make_unique<ParallelBruteForceSolver>(numThreads));
    manager.addSolver("DLX", std::make_unique<DLXSolver>(1));
    manager.addSolver("ParallelDLX", std::make_unique<ParallelDLXSolver>(numThreads));

    manager.runBenchmarks();
    manager.exportResultsCSV("../solution.txt");

    return 0;
}

