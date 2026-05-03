#include <matrix.hpp>

#include <gtest/gtest.h>

//
// --- ADDITION ---
//

TEST(arithmetic_add, compound_add_correct_values) {
    ysc::matrix<int, 3> a{1, 2, 3};
    const ysc::matrix<int, 3> b{10, 20, 30};
    a += b;
    ASSERT_EQ(a, (ysc::matrix<int, 3>{11, 22, 33}));
}

TEST(arithmetic_add, compound_add_returns_self) {
    ysc::matrix<int, 2> a{1, 2};
    const ysc::matrix<int, 2> b{3, 4};
    ysc::matrix<int, 2>& ref = (a += b);
    ASSERT_EQ(&ref, &a);
}

TEST(arithmetic_add, compound_add_does_not_modify_rhs) {
    ysc::matrix<int, 2> a{1, 2};
    const ysc::matrix<int, 2> b{3, 4};
    const ysc::matrix<int, 2> b_copy = b;
    a += b;
    ASSERT_EQ(b, b_copy);
}

TEST(arithmetic_add, binary_plus_correct_values) {
    const ysc::matrix<int, 3> a{1, 2, 3};
    const ysc::matrix<int, 3> b{10, 20, 30};
    const auto c = a + b;
    ASSERT_EQ(c, (ysc::matrix<int, 3>{11, 22, 33}));
}

TEST(arithmetic_add, binary_plus_does_not_modify_operands) {
    const ysc::matrix<int, 2> a{1, 2};
    const ysc::matrix<int, 2> b{3, 4};
    const auto c = a + b;
    ASSERT_EQ(c, (ysc::matrix<int, 2>{4, 6}));
    ASSERT_EQ(a, (ysc::matrix<int, 2>{1, 2}));
    ASSERT_EQ(b, (ysc::matrix<int, 2>{3, 4}));
}

TEST(arithmetic_add, add_float) {
    ysc::matrix<double, 2> a{1.5, 2.5};
    a += ysc::matrix<double, 2>{0.5, 0.5};
    ASSERT_DOUBLE_EQ(a(0), 2.0);
    ASSERT_DOUBLE_EQ(a(1), 3.0);
}

TEST(arithmetic_add, add_2d_matrix) {
    ysc::matrix<int, 2, 2> a{1, 2, 3, 4};
    const ysc::matrix<int, 2, 2> b{5, 6, 7, 8};
    const auto c = a + b;
    ASSERT_EQ(c, (ysc::matrix<int, 2, 2>{6, 8, 10, 12}));
}

//
// --- SUBTRACTION ---
//

TEST(arithmetic_sub, compound_sub_correct_values) {
    ysc::matrix<int, 3> a{10, 20, 30};
    const ysc::matrix<int, 3> b{1, 2, 3};
    a -= b;
    ASSERT_EQ(a, (ysc::matrix<int, 3>{9, 18, 27}));
}

TEST(arithmetic_sub, compound_sub_returns_self) {
    ysc::matrix<int, 2> a{5, 6};
    const ysc::matrix<int, 2> b{1, 2};
    ysc::matrix<int, 2>& ref = (a -= b);
    ASSERT_EQ(&ref, &a);
}

TEST(arithmetic_sub, compound_sub_does_not_modify_rhs) {
    ysc::matrix<int, 2> a{5, 6};
    const ysc::matrix<int, 2> b{1, 2};
    const ysc::matrix<int, 2> b_copy = b;
    a -= b;
    ASSERT_EQ(b, b_copy);
}

TEST(arithmetic_sub, binary_minus_correct_values) {
    const ysc::matrix<int, 3> a{10, 20, 30};
    const ysc::matrix<int, 3> b{1, 2, 3};
    const auto c = a - b;
    ASSERT_EQ(c, (ysc::matrix<int, 3>{9, 18, 27}));
}

TEST(arithmetic_sub, binary_minus_does_not_modify_operands) {
    const ysc::matrix<int, 2> a{5, 6};
    const ysc::matrix<int, 2> b{1, 2};
    const auto c = a - b;
    ASSERT_EQ(c, (ysc::matrix<int, 2>{4, 4}));
    ASSERT_EQ(a, (ysc::matrix<int, 2>{5, 6}));
    ASSERT_EQ(b, (ysc::matrix<int, 2>{1, 2}));
}

//
// --- COMPILE-TIME CONSTRAINTS ---
//

struct no_arithmetic {
    int val;
    bool operator==(const no_arithmetic&) const = default;
};

// Helper concepts — GCC does not support bare requires-expressions in static_assert
// when all candidates are constrained out (same workaround as in concepts.cpp).
template <class M>
concept matrix_add_assignable = requires(M& a, const M& b) { a += b; };
template <class M>
concept matrix_addable = requires(const M& a, const M& b) { a + b; };
template <class M>
concept matrix_sub_assignable = requires(M& a, const M& b) { a -= b; };
template <class M>
concept matrix_subtractable = requires(const M& a, const M& b) { a - b; };

static_assert(!matrix_add_assignable<ysc::matrix<no_arithmetic, 2>>);
static_assert(!matrix_addable<ysc::matrix<no_arithmetic, 2>>);
static_assert(!matrix_sub_assignable<ysc::matrix<no_arithmetic, 2>>);
static_assert(!matrix_subtractable<ysc::matrix<no_arithmetic, 2>>);

static_assert(matrix_add_assignable<ysc::matrix<int, 2>>);
static_assert(matrix_addable<ysc::matrix<int, 2>>);
static_assert(matrix_sub_assignable<ysc::matrix<int, 2>>);
static_assert(matrix_subtractable<ysc::matrix<int, 2>>);

TEST(arithmetic_elementwise, concept_constraints_checked_at_compile_time) {
    // Runtime witness that the static_asserts above were checked.
    SUCCEED();
}
