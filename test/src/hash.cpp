#include "matrix.hpp"

#include <gtest/gtest.h>

#include <unordered_set>

TEST(hash, unordered_set_compiles_and_works) {
    std::unordered_set<ysc::matrix<int, 3>> s;
    s.insert({1, 2, 3});
    s.insert({4, 5, 6});
    s.insert({1, 2, 3});

    EXPECT_EQ(s.size(), 2U);
    EXPECT_EQ(s.count({1, 2, 3}), 1U);
    EXPECT_EQ(s.count({4, 5, 6}), 1U);
    EXPECT_EQ(s.count({7, 8, 9}), 0U);
}

TEST(hash, equal_matrices_have_equal_hashes) {
    std::hash<ysc::matrix<int, 2, 2>> h;
    ysc::matrix<int, 2, 2> m1{1, 2, 3, 4};
    ysc::matrix<int, 2, 2> m2{1, 2, 3, 4};

    EXPECT_EQ(h(m1), h(m2));
}

TEST(hash, different_matrices_have_different_hashes) {
    std::hash<ysc::matrix<int, 3>> h;
    ysc::matrix<int, 3> a{1, 2, 3};
    ysc::matrix<int, 3> b{1, 2, 4};
    ysc::matrix<int, 3> c{0, 2, 3};

    EXPECT_NE(h(a), h(b));
    EXPECT_NE(h(a), h(c));
    EXPECT_NE(h(b), h(c));
}
