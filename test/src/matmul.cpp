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

TEST(MatrixMatmul, MixedTypes) {
    // [[1,2,3],[4,5,6]] * [[1.5,2.5],[3.5,4.5],[5.5,6.5]]
    // row 0: [1*1.5+2*3.5+3*5.5, 1*2.5+2*4.5+3*6.5] = [25.0, 31.0]
    // row 1: [4*1.5+5*3.5+6*5.5, 4*2.5+5*4.5+6*6.5] = [56.5, 71.5]
    ysc::matrix<int, 2, 3> a{1, 2, 3, 4, 5, 6};
    ysc::matrix<double, 3, 2> b{1.5, 2.5, 3.5, 4.5, 5.5, 6.5};
    auto c = ysc::matmul(a, b);
    static_assert(std::is_same_v<decltype(c), ysc::matrix<double, 2, 2>>);
    EXPECT_DOUBLE_EQ(c(0, 0), 25.0);
    EXPECT_DOUBLE_EQ(c(0, 1), 31.0);
    EXPECT_DOUBLE_EQ(c(1, 0), 56.5);
    EXPECT_DOUBLE_EQ(c(1, 1), 71.5);
}

// constexpr verification
static_assert([] {
    ysc::matrix<int, 2, 2> a{1, 2, 3, 4};
    ysc::matrix<int, 2, 2> b{5, 6, 7, 8};
    auto c = ysc::matmul(a, b);
    return c(0, 0) == 19 && c(0, 1) == 22 && c(1, 0) == 43 && c(1, 1) == 50;
}());

// ─── matmul(matrix, vector) ───────────────────────────────────────────────────

TEST(MatrixMatmulVec, ReturnTypeIsCorrect) {
    ysc::matrix<int, 2, 3> mat{1, 0, 0, 0, 1, 0};
    ysc::matrix<int, 3> vec{1, 2, 3};
    auto r = ysc::matmul(mat, vec);
    static_assert(std::is_same_v<decltype(r), ysc::matrix<int, 2>>);
    (void)r;
}

TEST(MatrixMatmulVec, MixedTypes) {
    // int matrix × double vector → double vector
    ysc::matrix<int, 2, 3> mat{1, 2, 3, 4, 5, 6};
    ysc::matrix<double, 3> vec{0.5, 1.0, 1.5};
    auto r = ysc::matmul(mat, vec);
    static_assert(std::is_same_v<decltype(r), ysc::matrix<double, 2>>);
    EXPECT_DOUBLE_EQ(r(0), 1 * 0.5 + 2 * 1.0 + 3 * 1.5); // 7.0
    EXPECT_DOUBLE_EQ(r(1), 4 * 0.5 + 5 * 1.0 + 6 * 1.5); // 16.0
}

TEST(MatrixMatmulVec, BasicValues) {
    // [[1,0,0],[0,1,0]] * [1,2,3] = [1,2]
    ysc::matrix<int, 2, 3> mat{1, 0, 0, 0, 1, 0};
    ysc::matrix<int, 3> vec{1, 2, 3};
    auto r = ysc::matmul(mat, vec);
    EXPECT_EQ(r(0), 1);
    EXPECT_EQ(r(1), 2);
}

TEST(MatrixMatmulVec, NonTrivialValues) {
    // [[1,2,3],[4,5,6]] * [1,0,1] = [4,10]
    ysc::matrix<int, 2, 3> mat{1, 2, 3, 4, 5, 6};
    ysc::matrix<int, 3> vec{1, 0, 1};
    auto r = ysc::matmul(mat, vec);
    EXPECT_EQ(r(0), 4);
    EXPECT_EQ(r(1), 10);
}

TEST(MatrixMatmulVec, IdentityLeftNeutral) {
    ysc::matrix<int, 3> vec{7, 8, 9};
    auto r = ysc::matmul(ysc::identity<int, 3>(), vec);
    EXPECT_EQ(r, vec);
}

TEST(MatrixMatmulVec, ZeroMatrix) {
    ysc::matrix<int, 2, 3> mat(ysc::zero);
    ysc::matrix<int, 3> vec{1, 2, 3};
    auto r = ysc::matmul(mat, vec);
    ysc::matrix<int, 2> expected(ysc::zero);
    EXPECT_EQ(r, expected);
}

TEST(MatrixMatmulVec, SingleRowMatrix) {
    // [[1,2,3]] * [4,5,6] = [32]
    ysc::matrix<int, 1, 3> mat{1, 2, 3};
    ysc::matrix<int, 3> vec{4, 5, 6};
    auto r = ysc::matmul(mat, vec);
    EXPECT_EQ(r(0), 32);
}

// constexpr verification
static_assert([] {
    ysc::matrix<int, 2, 3> mat{1, 0, 0, 0, 1, 0};
    ysc::matrix<int, 3> vec{5, 6, 7};
    auto r = ysc::matmul(mat, vec);
    return r(0) == 5 && r(1) == 6;
}());
