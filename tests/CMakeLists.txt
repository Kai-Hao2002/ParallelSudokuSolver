# test_solver 
add_executable(
    test_solver
    test_solver.cpp
    ../src/sudoku.cpp
    ../src/parallel_solver.cpp
    ../src/util.cpp
)

# benchmark_solver 
add_executable(
    benchmark_solver
    ../src/benchmark_solver.cpp
    ../src/sudoku.cpp
    ../src/parallel_solver.cpp
    ../src/util.cpp
)

# Include path 
target_include_directories(test_solver PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party/taskflow
)

target_include_directories(benchmark_solver PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/third_party/taskflow
)

# gtest 
target_link_libraries(test_solver
    PRIVATE
    GTest::gtest
    GTest::gtest_main
)


enable_testing()
add_test(NAME SudokuSolverTest COMMAND test_solver)
