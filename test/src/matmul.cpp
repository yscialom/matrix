#include "matrix.hpp"
#include <gtest/gtest.h>

TEST(MatrixMatmul, ReturnTypeIsCorrect) {
    ysc::matrix<int, 2, 3> a{1, 2, 3, 4, 5, 6};
    ysc::matrix<int, 3, 4> b{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0};
    auto c = ysc::matmul(a, b);
    static_assert(std::is_same_v<decltype(c), ysc::matrix<int, 2, 4>>);
    (void)c;
}

TEST(MatrixMatmul, SquareMatrices) {
    // [[1,2],[3,4]] * [[5,6],[7,8]] = [[19,22],[43,50]]
    ysc::matrix<int, 2, 2> a{1, 2, 3, 4};
    ysc::matrix<int, 2, 2> b{5, 6, 7, 8};
    auto c = ysc::matmul(a, b);
    EXPECT_EQ(c(0, 0), 19);
    EXPECT_EQ(c(0, 1), 22);
    EXPECT_EQ(c(1, 0), 43);
    EXPECT_EQ(c(1, 1), 50);
}

TEST(MatrixMatmul, NonSquareMatrices) {
    // [[1,2,3],[4,5,6]] * [[7,8],[9,10],[11,12]] = [[58,64],[139,154]]
    ysc::matrix<int, 2, 3> a{1, 2, 3, 4, 5, 6};
    ysc::matrix<int, 3, 2> b{7, 8, 9, 10, 11, 12};
    auto c = ysc::matmul(a, b);
    EXPECT_EQ(c(0, 0), 58);
    EXPECT_EQ(c(0, 1), 64);
    EXPECT_EQ(c(1, 0), 139);
    EXPECT_EQ(c(1, 1), 154);
}

TEST(MatrixMatmul, IdentityLeftNeutral) {
    ysc::matrix<int, 3, 2> m{1, 2, 3, 4, 5, 6};
    auto result = ysc::matmul(ysc::identity<int, 3>(), m);
    EXPECT_EQ(result, m);
}

TEST(MatrixMatmul, IdentityRightNeutral) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto result = ysc::matmul(m, ysc::identity<int, 3>());
    EXPECT_EQ(result, m);
}

TEST(MatrixMatmul, SquareIdentity) {
    ysc::matrix<int, 3, 3> m{1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto id = ysc::identity<int, 3>();
    EXPECT_EQ(ysc::matmul(id, m), m);
}

TEST(MatrixMatmul, ResultIsZeroWhenInputIsZero) {
    ysc::matrix<int, 2, 3> a(ysc::zero);
    ysc::matrix<int, 3, 4> b(ysc::zero);
    auto c = ysc::matmul(a, b);
    ysc::matrix<int, 2, 4> expected(ysc::zero);
    EXPECT_EQ(c, expected);
}

TEST(MatrixMatmul, WorksWithDouble) {
    ysc::matrix<double, 2, 2> a{1.0, 2.0, 3.0, 4.0};
    ysc::matrix<double, 2, 2> b{0.5, 0.0, 0.0, 0.5};
    auto c = ysc::matmul(a, b);
    EXPECT_DOUBLE_EQ(c(0, 0), 0.5);
    EXPECT_DOUBLE_EQ(c(0, 1), 1.0);
    EXPECT_DOUBLE_EQ(c(1, 0), 1.5);
    EXPECT_DOUBLE_EQ(c(1, 1), 2.0);
}

// constexpr verification
static_assert([] {
    ysc::matrix<int, 2, 2> a{1, 2, 3, 4};
    ysc::matrix<int, 2, 2> b{5, 6, 7, 8};
    auto c = ysc::matmul(a, b);
    return c(0, 0) == 19 && c(0, 1) == 22 && c(1, 0) == 43 && c(1, 1) == 50;
}());
