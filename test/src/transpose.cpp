#include "matrix.hpp"
#include <gtest/gtest.h>

TEST(MatrixTranspose, ReturnTypeIsTransposed) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto t = ysc::transpose(m);
    static_assert(std::is_same_v<decltype(t), ysc::matrix<int, 3, 2>>);
    (void)t;
}

TEST(MatrixTranspose, ValuesAreCorrectlySwapped) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto t = ysc::transpose(m);
    // m(0,0)=1 m(0,1)=2 m(0,2)=3
    // m(1,0)=4 m(1,1)=5 m(1,2)=6
    // t(j,i) == m(i,j)
    EXPECT_EQ(t(0, 0), 1);
    EXPECT_EQ(t(0, 1), 4);
    EXPECT_EQ(t(1, 0), 2);
    EXPECT_EQ(t(1, 1), 5);
    EXPECT_EQ(t(2, 0), 3);
    EXPECT_EQ(t(2, 1), 6);
}

TEST(MatrixTranspose, TransposeOfTransposeEqualsOriginal) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    EXPECT_EQ(ysc::transpose(ysc::transpose(m)), m);
}

TEST(MatrixTranspose, SquareMatrixTranspose) {
    ysc::matrix<int, 2, 2> m{1, 2, 3, 4};
    auto t = ysc::transpose(m);
    static_assert(std::is_same_v<decltype(t), ysc::matrix<int, 2, 2>>);
    EXPECT_EQ(t(0, 0), 1);
    EXPECT_EQ(t(0, 1), 3);
    EXPECT_EQ(t(1, 0), 2);
    EXPECT_EQ(t(1, 1), 4);
}

TEST(MatrixTranspose, IdentityTransposeIsIdentity) {
    auto id = ysc::identity<int, 3>();
    EXPECT_EQ(ysc::transpose(id), id);
}

TEST(MatrixTranspose, SingleRowMatrix) {
    ysc::matrix<int, 1, 4> m{1, 2, 3, 4};
    auto t = ysc::transpose(m);
    static_assert(std::is_same_v<decltype(t), ysc::matrix<int, 4, 1>>);
    EXPECT_EQ(t(0, 0), 1);
    EXPECT_EQ(t(1, 0), 2);
    EXPECT_EQ(t(2, 0), 3);
    EXPECT_EQ(t(3, 0), 4);
}

TEST(MatrixTranspose, WorksWithDoubleElements) {
    ysc::matrix<double, 2, 2> m{1.5, 2.5, 3.5, 4.5};
    auto t = ysc::transpose(m);
    EXPECT_DOUBLE_EQ(t(0, 1), 3.5);
    EXPECT_DOUBLE_EQ(t(1, 0), 2.5);
}

// constexpr verification
static_assert([] {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto t = ysc::transpose(m);
    return t(0, 0) == 1 && t(0, 1) == 4 && t(2, 1) == 6;
}());
