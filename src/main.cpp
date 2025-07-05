#include "util.hpp"
#include "sudoku.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    std::string filename = "data/puzzles.txt";
    if (argc > 1) {
        filename = argv[1];
    }

    try {
        auto puzzles = load_puzzles(filename);
        for (size_t i = 0; i < puzzles.size(); ++i) {
            SudokuSolver solver(puzzles[i]);
            bool solved = solver.solve_sequential();
            std::cout << "Puzzle " << (i + 1) << (solved ? " solved." : " not solved.") << "\n";
            solver.print();
            std::cout << "-----------------------\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
