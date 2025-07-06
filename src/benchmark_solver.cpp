#include "util.hpp"
#include "sudoku.hpp"
#include "parallel_solver.hpp"
#include <chrono>
#include <iostream>
#include <numeric>
#include <cmath>
#include <string>

double benchmark(const std::vector<std::vector<int>>& puzzle, bool parallel, int runs = 5) {
    std::vector<double> times;
    int solved_count = 0;

    for (int i = 0; i < runs; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        bool solved = false;

        if (parallel) {
            ParallelSudokuSolver solver(puzzle);
            solved = solver.solve();
        } else {
            SudokuSolver solver(puzzle);
            solved = solver.solve_sequential();
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        times.push_back(elapsed.count());

        if (solved) ++solved_count;
    }

    double avg = std::accumulate(times.begin(), times.end(), 0.0) / runs;
    double sq_sum = std::inner_product(times.begin(), times.end(), times.begin(), 0.0);
    double stddev = std::sqrt(sq_sum / runs - avg * avg);

    std::cout << (parallel ? "[Parallel]" : "[Sequential]") 
              << " Avg: " << avg << "s, StdDev: " << stddev 
              << ", Solved: " << solved_count << "/" << runs << "\n";

    return avg;
}

int main(int argc, char* argv[]) {
    std::string filename = "data/puzzles.txt";
    int runs = 5;

    // CLI argument parsing
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--runs" && i + 1 < argc) {
            runs = std::stoi(argv[++i]);
        } else {
            filename = arg;
        }
    }

    try {
        auto puzzles = load_puzzles(filename);
        for (size_t i = 0; i < puzzles.size(); ++i) {
            std::cout << "== Puzzle " << (i + 1) << " ==\n";
            benchmark(puzzles[i], false, runs); // sequential
            benchmark(puzzles[i], true, runs);  // parallel
            std::cout << "---------------------\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error loading puzzles: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
