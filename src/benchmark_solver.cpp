#include "util.hpp"
#include "sudoku.hpp"
#include "parallel_solver.hpp"
#include <chrono>
#include <iostream>
#include <numeric>
#include <cmath>
#include <cstring>

// Benchmarking function
double benchmark(const std::vector<std::vector<int>>& puzzle, bool parallel, bool find_all, int runs = 5, size_t max_solutions = SIZE_MAX) {
    std::vector<double> times;
    int solved_count = 0;
    size_t total_solutions = 0;

    for (int i = 0; i < runs; ++i) {
        auto start = std::chrono::high_resolution_clock::now();

        if (parallel) {
            if (find_all) {
                std::vector<std::vector<std::vector<int>>> solutions;
                ParallelSudokuSolver solver(puzzle);
                solver.solve_all(solutions, max_solutions);
                total_solutions = solutions.size();
                solved_count += solutions.size() > 0;
            } else {
                ParallelSudokuSolver solver(puzzle);
                bool solved = solver.solve_single();
                solved_count += solved;
            }
        } else {
            if (find_all) {
                std::vector<std::vector<std::vector<int>>> solutions;
                SudokuSolver solver(puzzle);
                solver.solve_all_sequential(solutions, max_solutions);
                total_solutions = solutions.size();
                solved_count += solutions.size() > 0;
            } else {
                SudokuSolver solver(puzzle);
                bool solved = solver.solve_sequential();
                solved_count += solved;
            }
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        times.push_back(elapsed.count());
    }

    double avg = std::accumulate(times.begin(), times.end(), 0.0) / runs;
    double sq_sum = std::inner_product(times.begin(), times.end(), times.begin(), 0.0);
    double stddev = std::sqrt(sq_sum / runs - avg * avg);

    std::cout << (parallel ? "[Parallel]" : "[Sequential]")
              << (find_all ? " Find All" : " Find One")
              << " Avg: " << avg << "s, StdDev: " << stddev
              << ", Solved: " << solved_count << "/" << runs;
    if (find_all) {
        std::cout << ", Total Solutions: " << total_solutions;
    }
    std::cout << "\n";

    return avg;
}


int main(int argc, char* argv[]) {
    std::string filename = "data/puzzles.txt";
    bool find_all = false;
    int runs = 5;
    size_t max_solutions = SIZE_MAX;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-a" || arg == "--all") {
            find_all = true;
        } else if ((arg == "-m" || arg == "--max") && i + 1 < argc) {
            max_solutions = std::stoul(argv[++i]);
        } else {
            filename = arg;
        }
    }

    auto puzzles = load_puzzles(filename);

    for (size_t i = 0; i < puzzles.size(); ++i) {
        std::cout << "Puzzle " << (i + 1) << ":\n";
        benchmark(puzzles[i], false, find_all, runs, max_solutions);
        benchmark(puzzles[i], true, find_all, runs, max_solutions);
        std::cout << "---------------------\n";
    }

    return 0;
}
