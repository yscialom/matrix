#include <matrix.hpp>

#include <gtest/gtest.h>

//
// --- HADAMARD PRODUCT (element-wise *) ---
//

TEST(arithmetic_hadamard, compound_mul_correct_values) {
    ysc::matrix<int, 3> a{2, 3, 4};
    const ysc::matrix<int, 3> b{5, 6, 7};
    a *= b;
    ASSERT_EQ(a, (ysc::matrix<int, 3>{10, 18, 28}));
}

TEST(arithmetic_hadamard, compound_mul_returns_self) {
    ysc::matrix<int, 2> a{2, 3};
    const ysc::matrix<int, 2> b{4, 5};
    ysc::matrix<int, 2>& ref = (a *= b);
    ASSERT_EQ(&ref, &a);
}

TEST(arithmetic_hadamard, compound_mul_does_not_modify_rhs) {
    ysc::matrix<int, 2> a{2, 3};
    const ysc::matrix<int, 2> b{4, 5};
    const ysc::matrix<int, 2> b_copy = b;
    a *= b;
    ASSERT_EQ(b, b_copy);
}

TEST(arithmetic_hadamard, binary_mul_correct_values) {
    const ysc::matrix<int, 3> a{2, 3, 4};
    const ysc::matrix<int, 3> b{5, 6, 7};
    const auto c = a * b;
    ASSERT_EQ(c, (ysc::matrix<int, 3>{10, 18, 28}));
}

TEST(arithmetic_hadamard, binary_mul_does_not_modify_operands) {
    const ysc::matrix<int, 2> a{2, 3};
    const ysc::matrix<int, 2> b{4, 5};
    const auto c = a * b;
    ASSERT_EQ(c, (ysc::matrix<int, 2>{8, 15}));
    ASSERT_EQ(a, (ysc::matrix<int, 2>{2, 3}));
    ASSERT_EQ(b, (ysc::matrix<int, 2>{4, 5}));
}

TEST(arithmetic_hadamard, mul_float) {
    const ysc::matrix<double, 2> a{1.5, 2.0};
    const ysc::matrix<double, 2> b{2.0, 3.0};
    const auto c = a * b;
    ASSERT_DOUBLE_EQ(c(0), 3.0);
    ASSERT_DOUBLE_EQ(c(1), 6.0);
}

TEST(arithmetic_hadamard, mul_2d_matrix) {
    const ysc::matrix<int, 2, 2> a{1, 2, 3, 4};
    const ysc::matrix<int, 2, 2> b{5, 6, 7, 8};
    const auto c = a * b;
    ASSERT_EQ(c, (ysc::matrix<int, 2, 2>{5, 12, 21, 32}));
}

//
// --- ELEMENT-WISE DIVISION ---
//

TEST(arithmetic_hadamard, compound_div_correct_values) {
    ysc::matrix<int, 3> a{10, 18, 28};
    const ysc::matrix<int, 3> b{2, 3, 4};
    a /= b;
    ASSERT_EQ(a, (ysc::matrix<int, 3>{5, 6, 7}));
}

TEST(arithmetic_hadamard, compound_div_returns_self) {
    ysc::matrix<int, 2> a{6, 8};
    const ysc::matrix<int, 2> b{2, 4};
    ysc::matrix<int, 2>& ref = (a /= b);
    ASSERT_EQ(&ref, &a);
}

TEST(arithmetic_hadamard, compound_div_does_not_modify_rhs) {
    ysc::matrix<int, 2> a{6, 8};
    const ysc::matrix<int, 2> b{2, 4};
    const ysc::matrix<int, 2> b_copy = b;
    a /= b;
    ASSERT_EQ(b, b_copy);
}

TEST(arithmetic_hadamard, binary_div_correct_values) {
    const ysc::matrix<int, 3> a{10, 18, 28};
    const ysc::matrix<int, 3> b{2, 3, 4};
    const auto c = a / b;
    ASSERT_EQ(c, (ysc::matrix<int, 3>{5, 6, 7}));
}

TEST(arithmetic_hadamard, binary_div_does_not_modify_operands) {
    const ysc::matrix<int, 2> a{6, 8};
    const ysc::matrix<int, 2> b{2, 4};
    const auto c = a / b;
    ASSERT_EQ(c, (ysc::matrix<int, 2>{3, 2}));
    ASSERT_EQ(a, (ysc::matrix<int, 2>{6, 8}));
    ASSERT_EQ(b, (ysc::matrix<int, 2>{2, 4}));
}

//
// --- COMPILE-TIME CONSTRAINTS ---
//

struct no_mul {
    int val;
    bool operator==(const no_mul&) const = default;
};

// Helper concepts — GCC does not support bare requires-expressions in static_assert
// when all candidates are constrained out (same workaround as in concepts.cpp).
// Note: operator* and operator/ delegate to *=/=/=, so testing *= and /= is sufficient.
template <class M>
concept matrix_mul_assignable = requires(M& a, const M& b) { a *= b; };
template <class M>
concept matrix_div_assignable = requires(M& a, const M& b) { a /= b; };

static_assert(!matrix_mul_assignable<ysc::matrix<no_mul, 2>>);
static_assert(!matrix_div_assignable<ysc::matrix<no_mul, 2>>);

static_assert(matrix_mul_assignable<ysc::matrix<int, 2>>);
static_assert(matrix_div_assignable<ysc::matrix<int, 2>>);

TEST(arithmetic_hadamard, concept_constraints_checked_at_compile_time) {
    // Runtime witness that the static_asserts above were checked.
    SUCCEED();
}
