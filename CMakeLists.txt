cmake_minimum_required(VERSION 3.14)
project(wilderfield)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include FetchContent for downloading dependencies
include(FetchContent)

# Fetch Catch2
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v2.13.7 # Adjust the version as needed
)

FetchContent_MakeAvailable(Catch2)

list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)

# Declare Google Benchmark
FetchContent_Declare(
  benchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG main  # Optionally specify a version tag or commit
)

# Configure Google Benchmark
# Set this to disable tests and other unnecessary parts of the benchmark library
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Disable benchmark testing as we don't need it.")

FetchContent_MakeAvailable(benchmark)

# Include directories
include_directories(include)

# Enable testing and add the subdirectory containing tests
enable_testing()
add_subdirectory(tests)

add_subdirectory(benchmark)

# Build documentation
add_subdirectory(docs)
