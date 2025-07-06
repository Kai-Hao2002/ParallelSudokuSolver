#include "util.hpp"
#include "sudoku.hpp"
#include "parallel_solver.hpp"
#include <iostream>
#include <chrono>

int main(int argc, char* argv[]) {
    std::string filename = "data/puzzles.txt";
    std::string mode = "sequential"; // default

    // Parse CLI arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--sequential" || arg == "-s") {
            mode = "sequential";
        } else if (arg == "--parallel" || arg == "-p") {
            mode = "parallel";
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [--sequential | --parallel] [puzzle_file]\n";
            std::cout << "Default: sequential mode with puzzles from data/puzzles.txt\n";
            return 0;
        } else {
            filename = arg;
        }
    }

    try {
        auto puzzles = load_puzzles(filename);
        for (size_t i = 0; i < puzzles.size(); ++i) {
            std::cout << "Puzzle " << (i + 1) << "...\n";

            auto start = std::chrono::high_resolution_clock::now();
            bool solved = false;

            if (mode == "sequential") {
                SudokuSolver solver(puzzles[i]);
                solved = solver.solve_sequential();
                auto end = std::chrono::high_resolution_clock::now();
                std::cout << (solved ? "Solved (Sequential)." : "Not solved.") << "\n";
                solver.print();
                std::chrono::duration<double> elapsed = end - start;
                std::cout << "Time: " << elapsed.count() << "s\n";

            } else if (mode == "parallel") {
                ParallelSudokuSolver solver(puzzles[i]);
                solved = solver.solve_single();
                auto end = std::chrono::high_resolution_clock::now();
                std::cout << (solved ? "Solved (Parallel)." : "Not solved.") << "\n";
                solver.print();
                std::chrono::duration<double> elapsed = end - start;
                std::cout << "Time: " << elapsed.count() << "s\n";

            } else {
                std::cerr << "Unknown mode: " << mode << "\n";
                return 1;
            }

            std::cout << "-----------------------\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
