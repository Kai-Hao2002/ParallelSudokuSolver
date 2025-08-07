//include/io/FileIO.hpp
#pragma once

#include "core/Sudoku.hpp"
#include <string>

namespace FileIO {

bool loadSudokuFromFile(Sudoku& sudoku, const std::string& filename);

bool writeSolutionToFile(const Sudoku& sudoku, const std::string& filename);

}
