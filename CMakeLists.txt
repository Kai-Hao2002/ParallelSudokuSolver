#CMakeLists
cmake_minimum_required(VERSION 3.14)
project(ParallelSudokuSolver)

# C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#  include/ sub_directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/external/taskflow/)


# add Taskflow (external/taskflow）
add_subdirectory(external/taskflow)

# collect files
file(GLOB_RECURSE CORE_SRC src/core/*.cpp)
file(GLOB_RECURSE SOLVER_SRC src/solver/*.cpp)
file(GLOB_RECURSE IO_SRC src/io/*.cpp)
set(SOURCES ${CORE_SRC} ${SOLVER_SRC} ${IO_SRC} src/main.cpp)

# set up main
add_executable(ParallelSudokuSolver ${SOURCES})
target_include_directories(ParallelSudokuSolver PRIVATE external/taskflow/include)

# ===============================
# Catch2 Unit Tests
# ===============================
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.5.4 
)
FetchContent_MakeAvailable(Catch2)

# test files
file(GLOB_RECURSE TEST_SRC tests/*.cpp)
add_executable(tests ${TEST_SRC} ${CORE_SRC} ${SOLVER_SRC} ${IO_SRC})
target_include_directories(tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/external/taskflow/include
)
target_link_libraries(tests Catch2::Catch2WithMain)
add_custom_command(
    TARGET tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/Test_Cases
            ${CMAKE_BINARY_DIR}/Test_Cases
)

enable_testing()
include(CTest)
include(Catch)
catch_discover_tests(tests)

# ===============================
# Benchmark Executable
# ===============================

# collect benchmark files
file(GLOB_RECURSE BENCHMARK_SRC src/benchmark/*.cpp)
set(BENCHMARK_MAIN src/main_benchmark.cpp)
add_executable(benchmark ${BENCHMARK_MAIN} ${BENCHMARK_SRC} ${CORE_SRC} ${SOLVER_SRC} ${IO_SRC})

# include 
target_include_directories(benchmark PRIVATE 
    ${CMAKE_SOURCE_DIR}/include 
    ${CMAKE_SOURCE_DIR}/external/taskflow/include
)
