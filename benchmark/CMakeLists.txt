# Include the FetchContent module to handle dependencies
include(FetchContent)

# Declare Google Benchmark dependency
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG main  # Consider specifying a stable release tag
)

# Prevent Google Benchmark from installing itself when you install your project
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "Disable installation of benchmark")

# Make the fetched content available for use
FetchContent_MakeAvailable(benchmark)

# Define your executable that contains the benchmarks
add_executable(run_benchmarking benchmarking.cpp)

# Link your executable against Google Benchmark
target_link_libraries(run_benchmarking benchmark::benchmark)
