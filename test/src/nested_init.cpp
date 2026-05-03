#include <matrix.hpp>

#include <gtest/gtest.h>

//
// --- correct initialization ---
//

TEST(nested_init, int_2x3) {
    ysc::matrix<int, 2, 3> m{{1, 2, 3}, {4, 5, 6}};
    ASSERT_EQ(m(0, 0), 1);
    ASSERT_EQ(m(0, 1), 2);
    ASSERT_EQ(m(0, 2), 3);
    ASSERT_EQ(m(1, 0), 4);
    ASSERT_EQ(m(1, 1), 5);
    ASSERT_EQ(m(1, 2), 6);
}

TEST(nested_init, double_1x1) {
    ysc::matrix<double, 1, 1> m{{3.14}};
    ASSERT_DOUBLE_EQ(m(0, 0), 3.14);
}

TEST(nested_init, square_matrix) {
    ysc::matrix<int, 3, 3> m{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    ASSERT_EQ(m(2, 2), 9);
}

//
// --- runtime error cases ---
//

TEST(nested_init, wrong_row_count_throws) {
    ASSERT_THROW((ysc::matrix<int, 2, 3>{{1, 2, 3}}), std::length_error);
}

TEST(nested_init, wrong_column_count_throws) {
    ASSERT_THROW((ysc::matrix<int, 2, 3>{{1, 2}, {3, 4}}), std::length_error);
}
