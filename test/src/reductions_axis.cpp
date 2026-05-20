#include "matrix.hpp"

#include <gtest/gtest.h>

// clang-format off
// constexpr static checks
static_assert(ysc::matrix<int, 2, 3>{1, 2, 3, 4, 5, 6}.sum<0>() == (ysc::matrix<int, 3>{5, 7, 9}));
static_assert(ysc::matrix<int, 2, 3>{1, 2, 3, 4, 5, 6}.sum<1>() == (ysc::matrix<int, 2>{6, 15}));
// Axis out of bounds must not compile (verified by requires constraint — not testable at runtime)
// clang-format on

TEST(reductions_axis, sum_axis0_2d) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    ysc::matrix<int, 3> expected{5, 7, 9};
    EXPECT_EQ(m.sum<0>(), expected);
}

TEST(reductions_axis, sum_axis1_2d) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    ysc::matrix<int, 2> expected{6, 15};
    EXPECT_EQ(m.sum<1>(), expected);
}

TEST(reductions_axis, sum_axis0_3d) {
    ysc::matrix<int, 2, 3, 4> m{1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                                13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
    ysc::matrix<int, 3, 4> expected{14, 16, 18, 20, 22, 24, 26, 28, 30, 32, 34, 36};
    EXPECT_EQ(m.sum<0>(), expected);
}

TEST(reductions_axis, sum_axis1_3d) {
    ysc::matrix<int, 2, 3, 4> m{1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                                13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
    ysc::matrix<int, 2, 4> expected{15, 18, 21, 24, 51, 54, 57, 60};
    EXPECT_EQ(m.sum<1>(), expected);
}

TEST(reductions_axis, sum_axis2_3d) {
    ysc::matrix<int, 2, 3, 4> m{1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                                13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
    ysc::matrix<int, 2, 3> expected{10, 26, 42, 58, 74, 90};
    EXPECT_EQ(m.sum<2>(), expected);
}

TEST(reductions_axis, sum_single_row) {
    ysc::matrix<int, 1, 4> m{2, 3, 5, 7};
    ysc::matrix<int, 4> expected{2, 3, 5, 7};
    EXPECT_EQ(m.sum<0>(), expected);
}

TEST(reductions_axis, sum_single_col) {
    ysc::matrix<int, 3, 1> m{10, 20, 30};
    ysc::matrix<int, 3> expected{10, 20, 30};
    EXPECT_EQ(m.sum<1>(), expected);
}

TEST(reductions_axis, min_axis0_2d) {
    ysc::matrix<int, 2, 3> m{3, 1, 4, 1, 5, 9};
    ysc::matrix<int, 3> expected{1, 1, 4};
    EXPECT_EQ(m.min<0>(), expected);
}

TEST(reductions_axis, min_axis1_2d) {
    ysc::matrix<int, 2, 3> m{3, 1, 4, 1, 5, 9};
    ysc::matrix<int, 2> expected{1, 1};
    EXPECT_EQ(m.min<1>(), expected);
}

TEST(reductions_axis, max_axis0_2d) {
    ysc::matrix<int, 2, 3> m{3, 1, 4, 1, 5, 9};
    ysc::matrix<int, 3> expected{3, 5, 9};
    EXPECT_EQ(m.max<0>(), expected);
}

TEST(reductions_axis, max_axis1_2d) {
    ysc::matrix<int, 2, 3> m{3, 1, 4, 1, 5, 9};
    ysc::matrix<int, 2> expected{4, 9};
    EXPECT_EQ(m.max<1>(), expected);
}

TEST(reductions_axis, min_axis0_3d) {
    ysc::matrix<int, 2, 2, 2> m{5, 1, 3, 7, 2, 8, 6, 4};
    ysc::matrix<int, 2, 2> expected{2, 1, 3, 4};
    EXPECT_EQ(m.min<0>(), expected);
}

TEST(reductions_axis, max_axis2_3d) {
    ysc::matrix<int, 2, 2, 2> m{5, 1, 3, 7, 2, 8, 6, 4};
    ysc::matrix<int, 2, 2> expected{5, 7, 8, 6};
    EXPECT_EQ(m.max<2>(), expected);
}

TEST(reductions_axis, sum_zero_dim_not_on_axis) {
    ysc::matrix<int, 2, 0, 3> m{};
    ysc::matrix<int, 0, 3> expected{};
    EXPECT_EQ(m.sum<0>(), expected);
}

TEST(reductions_axis, sum_float) {
    ysc::matrix<double, 2, 2> m{1.5, 2.5, 3.5, 4.5};
    ysc::matrix<double, 2> expected{5.0, 7.0};
    EXPECT_EQ(m.sum<0>(), expected);
}
