#include <matrix.hpp>

#include <gtest/gtest.h>

//
// --- SCALAR MULTIPLICATION ---
//

TEST(arithmetic_scalar_mul, compound_mul_correct_values) {
    ysc::matrix<int, 3> m{1, 2, 3};
    m *= 2;
    ASSERT_EQ(m, (ysc::matrix<int, 3>{2, 4, 6}));
}

TEST(arithmetic_scalar_mul, compound_mul_returns_self) {
    ysc::matrix<int, 2> m{1, 2};
    ysc::matrix<int, 2>& ref = (m *= 3);
    ASSERT_EQ(&ref, &m);
}

TEST(arithmetic_scalar_mul, binary_matrix_times_scalar) {
    const ysc::matrix<int, 3> m{1, 2, 3};
    const auto result = m * 2;
    ASSERT_EQ(result, (ysc::matrix<int, 3>{2, 4, 6}));
}

TEST(arithmetic_scalar_mul, binary_scalar_times_matrix) {
    const ysc::matrix<int, 3> m{1, 2, 3};
    const auto result = 2 * m;
    ASSERT_EQ(result, (ysc::matrix<int, 3>{2, 4, 6}));
}

TEST(arithmetic_scalar_mul, binary_commutativity) {
    const ysc::matrix<int, 2> m{3, 4};
    ASSERT_EQ(m * 5, 5 * m);
}

TEST(arithmetic_scalar_mul, binary_does_not_modify_operands) {
    const ysc::matrix<int, 2> m{3, 4};
    const auto result = m * 5;
    ASSERT_EQ(result, (ysc::matrix<int, 2>{15, 20}));
    ASSERT_EQ(m, (ysc::matrix<int, 2>{3, 4}));
}

TEST(arithmetic_scalar_mul, float_scalar) {
    ysc::matrix<double, 2> m{1.0, 2.0};
    m *= 1.5;
    ASSERT_DOUBLE_EQ(m(0), 1.5);
    ASSERT_DOUBLE_EQ(m(1), 3.0);
}

TEST(arithmetic_scalar_mul, mul_2d_matrix) {
    const ysc::matrix<int, 2, 2> m{1, 2, 3, 4};
    const auto result = m * 3;
    ASSERT_EQ(result, (ysc::matrix<int, 2, 2>{3, 6, 9, 12}));
}

//
// --- SCALAR DIVISION ---
//

TEST(arithmetic_scalar_div, compound_div_correct_values) {
    ysc::matrix<int, 3> m{6, 8, 10};
    m /= 2;
    ASSERT_EQ(m, (ysc::matrix<int, 3>{3, 4, 5}));
}

TEST(arithmetic_scalar_div, compound_div_returns_self) {
    ysc::matrix<int, 2> m{6, 8};
    ysc::matrix<int, 2>& ref = (m /= 2);
    ASSERT_EQ(&ref, &m);
}

TEST(arithmetic_scalar_div, binary_div_correct_values) {
    const ysc::matrix<int, 3> m{6, 8, 10};
    const auto result = m / 2;
    ASSERT_EQ(result, (ysc::matrix<int, 3>{3, 4, 5}));
}

TEST(arithmetic_scalar_div, binary_does_not_modify_operands) {
    const ysc::matrix<int, 2> m{6, 8};
    const auto result = m / 2;
    ASSERT_EQ(result, (ysc::matrix<int, 2>{3, 4}));
    ASSERT_EQ(m, (ysc::matrix<int, 2>{6, 8}));
}

//
// --- SCALAR ADDITION ---
//

TEST(arithmetic_scalar_add, compound_add_correct_values) {
    ysc::matrix<int, 3> m{1, 2, 3};
    m += 10;
    ASSERT_EQ(m, (ysc::matrix<int, 3>{11, 12, 13}));
}

TEST(arithmetic_scalar_add, compound_add_returns_self) {
    ysc::matrix<int, 2> m{1, 2};
    ysc::matrix<int, 2>& ref = (m += 5);
    ASSERT_EQ(&ref, &m);
}

TEST(arithmetic_scalar_add, binary_add_correct_values) {
    const ysc::matrix<int, 3> m{1, 2, 3};
    const auto result = m + 10;
    ASSERT_EQ(result, (ysc::matrix<int, 3>{11, 12, 13}));
}

TEST(arithmetic_scalar_add, binary_does_not_modify_operands) {
    const ysc::matrix<int, 2> m{1, 2};
    const auto result = m + 5;
    ASSERT_EQ(result, (ysc::matrix<int, 2>{6, 7}));
    ASSERT_EQ(m, (ysc::matrix<int, 2>{1, 2}));
}

//
// --- SCALAR SUBTRACTION ---
//

TEST(arithmetic_scalar_sub, compound_sub_correct_values) {
    ysc::matrix<int, 3> m{11, 12, 13};
    m -= 10;
    ASSERT_EQ(m, (ysc::matrix<int, 3>{1, 2, 3}));
}

TEST(arithmetic_scalar_sub, compound_sub_returns_self) {
    ysc::matrix<int, 2> m{5, 6};
    ysc::matrix<int, 2>& ref = (m -= 3);
    ASSERT_EQ(&ref, &m);
}

TEST(arithmetic_scalar_sub, binary_sub_correct_values) {
    const ysc::matrix<int, 3> m{11, 12, 13};
    const auto result = m - 10;
    ASSERT_EQ(result, (ysc::matrix<int, 3>{1, 2, 3}));
}

TEST(arithmetic_scalar_sub, binary_does_not_modify_operands) {
    const ysc::matrix<int, 2> m{5, 6};
    const auto result = m - 3;
    ASSERT_EQ(result, (ysc::matrix<int, 2>{2, 3}));
    ASSERT_EQ(m, (ysc::matrix<int, 2>{5, 6}));
}

//
// --- COMPILE-TIME CONSTRAINTS ---
//

struct no_scalar_arithmetic {
    int val;
    bool operator==(const no_scalar_arithmetic&) const = default;
};

// Helper concepts — GCC does not support bare requires-expressions in static_assert
// when all candidates are constrained out (same workaround as in concepts.cpp).
template <class M, class S>
concept matrix_scalar_mul_assignable = requires(M& m, const S& s) { m *= s; };
template <class M, class S>
concept matrix_scalar_div_assignable = requires(M& m, const S& s) { m /= s; };
template <class M, class S>
concept matrix_scalar_add_assignable = requires(M& m, const S& s) { m += s; };
template <class M, class S>
concept matrix_scalar_sub_assignable = requires(M& m, const S& s) { m -= s; };

static_assert(!matrix_scalar_mul_assignable<ysc::matrix<no_scalar_arithmetic, 2>, int>);
static_assert(!matrix_scalar_div_assignable<ysc::matrix<no_scalar_arithmetic, 2>, int>);
static_assert(!matrix_scalar_add_assignable<ysc::matrix<no_scalar_arithmetic, 2>, int>);
static_assert(!matrix_scalar_sub_assignable<ysc::matrix<no_scalar_arithmetic, 2>, int>);

static_assert(matrix_scalar_mul_assignable<ysc::matrix<int, 2>, int>);
static_assert(matrix_scalar_div_assignable<ysc::matrix<int, 2>, int>);
static_assert(matrix_scalar_add_assignable<ysc::matrix<int, 2>, int>);
static_assert(matrix_scalar_sub_assignable<ysc::matrix<int, 2>, int>);

TEST(arithmetic_scalar, concept_constraints_checked_at_compile_time) {
    // Runtime witness that the static_asserts above were checked.
    SUCCEED();
}
