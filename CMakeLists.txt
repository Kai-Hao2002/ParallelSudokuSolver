cmake_minimum_required(VERSION 3.10)
project(SudokuSolver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -O2 -fopenmp")

# Add Taskflow
include_directories(third_party/taskflow)

# Include headers
include_directories(include)

# Main executable
add_executable(sudoku src/main.cpp src/sudoku.cpp src/util.cpp)

include(FetchContent)

# Download and make GoogleTest available
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(googletest)

# Add tests
enable_testing()
add_subdirectory(tests)
