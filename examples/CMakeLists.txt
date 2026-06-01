##
# examples/CMakeLists.txt
#
# Each example is a standalone .cpp file that produces a single executable
# and is registered as a ctest. The CI job 'examples' (push to develop only)
# compiles them with -DYSC_MATRIX_BUILD_EXAMPLES=ON on Ubuntu/GCC-13/Release
# and asserts each executable exits with code 0.
#

function(ysc_add_example file)
    # file: "01_basics" -> exe "example-01-basics"
    string(REPLACE "_" "-" target_suffix ${file})
    set(target example-${target_suffix})

    add_executable(${target} ${file}.cpp)
    target_link_libraries(${target} PRIVATE ysc::matrix)

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()

    add_test(NAME ${target} COMMAND ${target})
endfunction()

# Add new examples below, sorted by leading number.
ysc_add_example(01_basics)
ysc_add_example(02_arithmetic)
ysc_add_example(03_views)
ysc_add_example(04_algorithms)
ysc_add_example(05_interop_stl)
ysc_add_example(06_linear_algebra)
ysc_add_example(07_game_of_life)
ysc_add_example(08_kalman_filter_1d)
