#include "matrix.hpp"

#include <gtest/gtest.h>

// constexpr static checks
static_assert(ysc::matrix<int, 3>{1, 2, 3}.sum() == 6);
static_assert(ysc::matrix<int, 3>{3, 1, 2}.min() == 1);
static_assert(ysc::matrix<int, 3>{3, 1, 2}.max() == 3);
static_assert(ysc::matrix<int, 3>{1, 2, 3}.all());
static_assert(!ysc::matrix<int, 3>{1, 0, 3}.all());
static_assert(ysc::matrix<int, 3>{0, 0, 3}.any());
static_assert(!ysc::matrix<int, 3>{0, 0, 0}.any());

TEST(reductions, sum_1d) {
    ysc::matrix<int, 4> m{1, 2, 3, 4};
    EXPECT_EQ(m.sum(), 10);
}

TEST(reductions, sum_2d) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    EXPECT_EQ(m.sum(), 21);
}

TEST(reductions, sum_single) {
    ysc::matrix<int, 1> m{42};
    EXPECT_EQ(m.sum(), 42);
}

TEST(reductions, min_basic) {
    ysc::matrix<int, 5> m{3, 1, 4, 1, 5};
    EXPECT_EQ(m.min(), 1);
}

TEST(reductions, min_2d) {
    ysc::matrix<int, 2, 2> m{4, 2, 3, 1};
    EXPECT_EQ(m.min(), 1);
}

TEST(reductions, min_negative) {
    ysc::matrix<int, 3> m{-1, -5, -2};
    EXPECT_EQ(m.min(), -5);
}

TEST(reductions, max_basic) {
    ysc::matrix<int, 5> m{3, 1, 4, 1, 5};
    EXPECT_EQ(m.max(), 5);
}

TEST(reductions, max_2d) {
    ysc::matrix<int, 2, 2> m{4, 2, 3, 1};
    EXPECT_EQ(m.max(), 4);
}

TEST(reductions, max_negative) {
    ysc::matrix<int, 3> m{-1, -5, -2};
    EXPECT_EQ(m.max(), -1);
}

TEST(reductions, all_true) {
    ysc::matrix<int, 3> m{1, 2, 3};
    EXPECT_TRUE(m.all());
}

TEST(reductions, all_false_one_zero) {
    ysc::matrix<int, 3> m{1, 0, 3};
    EXPECT_FALSE(m.all());
}

TEST(reductions, all_false_all_zero) {
    ysc::matrix<int, 3> m{0, 0, 0};
    EXPECT_FALSE(m.all());
}

TEST(reductions, all_bool_type) {
    ysc::matrix<bool, 3> m{true, true, true};
    EXPECT_TRUE(m.all());
    ysc::matrix<bool, 3> n{true, false, true};
    EXPECT_FALSE(n.all());
}

TEST(reductions, any_one_true) {
    ysc::matrix<int, 3> m{0, 0, 3};
    EXPECT_TRUE(m.any());
}

TEST(reductions, any_all_false) {
    ysc::matrix<int, 3> m{0, 0, 0};
    EXPECT_FALSE(m.any());
}

TEST(reductions, any_all_true) {
    ysc::matrix<int, 3> m{1, 2, 3};
    EXPECT_TRUE(m.any());
}

TEST(reductions, any_bool_type) {
    ysc::matrix<bool, 3> m{false, false, true};
    EXPECT_TRUE(m.any());
    ysc::matrix<bool, 3> n{false, false, false};
    EXPECT_FALSE(n.any());
}
