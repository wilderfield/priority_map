# Define your executable that contains the benchmarks
add_executable(run_benchmarking benchmarking.cpp)

# Link your executable against Google Benchmark
target_link_libraries(run_benchmarking benchmark::benchmark)
