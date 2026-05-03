#include <matrix.hpp>

#include <gtest/gtest.h>

//
// --- UNARY PLUS ---
//

TEST(arithmetic_unary, unary_plus_equal_to_original) {
    const ysc::matrix<int, 3> m{1, 2, 3};
    ASSERT_EQ(+m, m);
}

TEST(arithmetic_unary, unary_plus_does_not_modify_operand) {
    const ysc::matrix<int, 2> m{5, 6};
    const auto result = +m;
    ASSERT_EQ(m, (ysc::matrix<int, 2>{5, 6}));
    (void)result;
}

TEST(arithmetic_unary, unary_plus_returns_copy) {
    ysc::matrix<int, 2> m{1, 2};
    const auto& result = +m;
    ASSERT_NE(&result, &m);
}

//
// --- UNARY MINUS ---
//

TEST(arithmetic_unary, unary_minus_negates_elements) {
    const ysc::matrix<int, 3> m{1, -2, 3};
    const auto result = -m;
    ASSERT_EQ(result, (ysc::matrix<int, 3>{-1, 2, -3}));
}

TEST(arithmetic_unary, unary_minus_all_positive) {
    const ysc::matrix<int, 3> m{1, 2, 3};
    ASSERT_EQ(-m, (ysc::matrix<int, 3>{-1, -2, -3}));
}

TEST(arithmetic_unary, unary_minus_does_not_modify_operand) {
    const ysc::matrix<int, 2> m{3, 4};
    const auto result = -m;
    ASSERT_EQ(result, (ysc::matrix<int, 2>{-3, -4}));
    ASSERT_EQ(m, (ysc::matrix<int, 2>{3, 4}));
}

TEST(arithmetic_unary, unary_minus_double) {
    const ysc::matrix<double, 2> m{1.5, -2.5};
    const auto result = -m;
    ASSERT_DOUBLE_EQ(result(0), -1.5);
    ASSERT_DOUBLE_EQ(result(1), 2.5);
}

TEST(arithmetic_unary, double_negation_equals_original) {
    const ysc::matrix<int, 3> m{1, 2, 3};
    ASSERT_EQ(-(-m), m);
}

TEST(arithmetic_unary, unary_minus_2d_matrix) {
    const ysc::matrix<int, 2, 2> m{1, -2, 3, -4};
    ASSERT_EQ(-m, (ysc::matrix<int, 2, 2>{-1, 2, -3, 4}));
}
