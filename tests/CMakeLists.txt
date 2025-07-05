add_executable(test_solver test_solver.cpp ../src/sudoku.cpp)

target_include_directories(test_solver PRIVATE ../include)

target_link_libraries(test_solver
    PRIVATE
    GTest::gtest
    GTest::gtest_main
    pthread
)

enable_testing()
add_test(NAME SudokuSolverTest COMMAND test_solver)
