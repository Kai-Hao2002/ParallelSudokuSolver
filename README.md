# Parallel_Computing_Project

This project builds upon prior research in parallel Sudoku solving, but introduces a task-based solver in C++ using Taskflow. It is designed for modular testing, profiling, and flexible solver behavior (first-solution vs all-solutions). Detailed comparisons with prior solvers are provided in the documentation and report.


1. What kind of scaling you can expect, and what the overhead of
scheduling would be.

I expect to observe near-linear speedup up to a moderate number of threads (e.g., 8–16), depending on puzzle complexity and task granularity. However, due to overhead from task creation, task queue management, and synchronization, diminishing returns are expected beyond a certain thread count. Taskflow's lightweight task scheduler should mitigate some of this overhead through its efficient work-stealing mechanism. I will benchmark performance across varying thread counts and puzzle difficulties to analyze both scalability and scheduling cost.

2. Do you plan to return all possible solutions ? Or only the first
correct solution?

I plan to support both modes: returning the first valid solution (for performance benchmarking) and returning all possible solutions (for completeness testing). The first mode allows early task termination via task cancellation or global flags, while the second mode enables parallel exploration without cutoff. The behavior will be configurable at runtime.


3. (Important) Please also ensure that you also implement and compare
against a good sequential baseline (not polynomial time). This also
enables you to get the correct absolute speedup.

To evaluate absolute speedup, I will first implement a high-quality sequential baseline solver using backtracking with MRV and forward checking heuristics. This will serve as the ground truth in terms of correctness and as a performance baseline. Both the sequential and parallel solvers will share the same core logic to ensure fair comparison.


terminal instructions

mkdir build
cd build
cmake ..
ninja

ctest
./tests/test_solver.exe

./sudoku -s ../data/puzzles.txt
./sudoku -p ../data/puzzles.txt

./tests/benchmark_solver.exe ../data/puzzles.txt
./tests/benchmark_solver.exe -a -m 100000 ../data/puzzles.txt