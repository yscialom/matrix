#include "matrix.hpp"
#include <gtest/gtest.h>

TEST(MatrixDot, BasicExample) {
    ysc::matrix<int, 3> a{1, 2, 3};
    ysc::matrix<int, 3> b{4, 5, 6};
    EXPECT_EQ(ysc::dot(a, b), 32);
}

TEST(MatrixDot, ReturnTypeMatchesElementType) {
    ysc::matrix<int, 2> a{1, 2};
    ysc::matrix<int, 2> b{3, 4};
    static_assert(std::is_same_v<decltype(ysc::dot(a, b)), int>);
    (void)a;
    (void)b;
}

TEST(MatrixDot, Commutativity) {
    ysc::matrix<int, 4> a{1, 2, 3, 4};
    ysc::matrix<int, 4> b{5, 6, 7, 8};
    EXPECT_EQ(ysc::dot(a, b), ysc::dot(b, a));
}

TEST(MatrixDot, ZeroVector) {
    ysc::matrix<int, 3> a{1, 2, 3};
    ysc::matrix<int, 3> z(ysc::zero);
    EXPECT_EQ(ysc::dot(a, z), 0);
}

TEST(MatrixDot, SingleElement) {
    ysc::matrix<int, 1> a{7};
    ysc::matrix<int, 1> b{6};
    EXPECT_EQ(ysc::dot(a, b), 42);
}

TEST(MatrixDot, WorksWithDouble) {
    ysc::matrix<double, 3> a{1.0, 2.0, 3.0};
    ysc::matrix<double, 3> b{0.1, 0.2, 0.3};
    EXPECT_DOUBLE_EQ(ysc::dot(a, b), 1.4);
}

TEST(MatrixDot, Orthogonal) {
    ysc::matrix<int, 2> a{1, 0};
    ysc::matrix<int, 2> b{0, 1};
    EXPECT_EQ(ysc::dot(a, b), 0);
}

// constexpr verification
static_assert([] {
    ysc::matrix<int, 3> a{1, 2, 3};
    ysc::matrix<int, 3> b{4, 5, 6};
    return ysc::dot(a, b) == 32;
}());
