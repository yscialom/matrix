cmake_minimum_required(VERSION 3.20)

include(FetchContent)
FetchContent_Declare(
    googlebenchmark
    GIT_REPOSITORY https://github.com/google/benchmark.git
    GIT_TAG        v1.8.3
    GIT_SHALLOW    TRUE
)
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googlebenchmark)

add_executable(matrix-bench
    src/main.cpp
    src/bench_construct.cpp
    src/bench_access.cpp
    src/bench_arithmetic.cpp
    src/bench_linalg.cpp
)
target_include_directories(matrix-bench PRIVATE
    ${CMAKE_SOURCE_DIR}/src/include
)
target_link_libraries(matrix-bench PRIVATE benchmark::benchmark)
target_compile_features(matrix-bench PRIVATE cxx_std_20)
