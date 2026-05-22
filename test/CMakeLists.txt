##
# Tests framework: gtest
#
find_package(GTest REQUIRED)
include_directories(include)


##
# Test definition
#
set(TARGET_NAME matrix-test)
add_executable(${TARGET_NAME}
    src/empty_matrix.cpp
    src/access.cpp
    src/accessors.cpp
    src/algorithms.cpp
    src/arithmetic_bitwise.cpp
    src/arithmetic_elementwise.cpp
    src/arithmetic_hadamard.cpp
    src/arithmetic_scalar.cpp
    src/arithmetic_unary.cpp
    src/assignment_returns_self.cpp
    src/comparison.cpp
    src/concepts.cpp
    src/construct.cpp
    src/construct_from_buffer.cpp
    src/factories.cpp
    src/generate_multi_index.cpp
    src/fill.cpp
    src/formatter.cpp
    src/hash.cpp
    src/instantiation.cpp
    src/nested_init.cpp
    src/iterators.cpp
    src/dot.cpp
    src/matmul.cpp
    src/matrix_from_view.cpp
    src/matrix_view.cpp
    src/matrix_view_io.cpp
    src/reshape.cpp
    src/slice.cpp
    src/ostream.cpp
    src/reductions.cpp
    src/reductions_axis.cpp
    src/rows_cols.cpp
    src/transpose.cpp
    src/size_data.cpp
    src/typedefs.cpp
    src/main.cpp
)

target_link_libraries(${TARGET_NAME} ysc::matrix GTest::gtest)


##
# Test run
#
add_test(NAME ${TARGET_NAME} COMMAND ${TARGET_NAME})
