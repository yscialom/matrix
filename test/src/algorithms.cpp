#include "matrix.hpp"
#include <gtest/gtest.h>
#include <string>

// --- apply ---

TEST(MatrixApply, MutatesElementsInPlace) {
    ysc::matrix<int, 3> m{1, 2, 3};
    m.apply([](int& v) { v *= 2; });
    EXPECT_EQ(m, (ysc::matrix<int, 3>{2, 4, 6}));
}

TEST(MatrixApply, DoesNotModifyOtherElements) {
    ysc::matrix<int, 2, 2> m{1, 2, 3, 4};
    ysc::matrix<int, 2, 2> original = m;
    m.apply([](int& v) { v += 10; });
    for (std::size_t i = 0; i < 2; ++i) {
        for (std::size_t j = 0; j < 2; ++j) {
            EXPECT_EQ(m(i, j), original(i, j) + 10);
        }
    }
}

TEST(MatrixApply, WorksOnFloatMatrix) {
    ysc::matrix<double, 3> m{1.0, 2.0, 3.0};
    m.apply([](double& v) { v = v * v; });
    EXPECT_DOUBLE_EQ(m(0), 1.0);
    EXPECT_DOUBLE_EQ(m(1), 4.0);
    EXPECT_DOUBLE_EQ(m(2), 9.0);
}

TEST(MatrixApply, RowMajorOrderIsRespected) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    int counter = 0;
    m.apply([&counter](int& v) { v = counter++; });
    EXPECT_EQ(m(0, 0), 0);
    EXPECT_EQ(m(0, 1), 1);
    EXPECT_EQ(m(0, 2), 2);
    EXPECT_EQ(m(1, 0), 3);
    EXPECT_EQ(m(1, 1), 4);
    EXPECT_EQ(m(1, 2), 5);
}

// --- map ---

TEST(MatrixMap, ReturnsSameTypeMatrix) {
    ysc::matrix<int, 3> m{1, 2, 3};
    auto result = m.map([](int v) { return v * 2; });
    EXPECT_EQ(result, (ysc::matrix<int, 3>{2, 4, 6}));
}

TEST(MatrixMap, DoesNotMutateOriginal) {
    ysc::matrix<int, 3> m{1, 2, 3};
    ysc::matrix<int, 3> original = m;
    auto result = m.map([](int v) { return v * 10; });
    EXPECT_EQ(m, original);
    (void)result;
}

TEST(MatrixMap, ReturnsDifferentTypeMatrix) {
    ysc::matrix<int, 3> m{1, 2, 3};
    auto result = m.map([](int v) { return std::to_string(v); });
    static_assert(std::is_same_v<decltype(result), ysc::matrix<std::string, 3>>);
    EXPECT_EQ(result(0), "1");
    EXPECT_EQ(result(1), "2");
    EXPECT_EQ(result(2), "3");
}

TEST(MatrixMap, WorksOn2DMatrix) {
    ysc::matrix<int, 2, 2> m{1, 2, 3, 4};
    auto result = m.map([](int v) { return v + 10; });
    EXPECT_EQ(result, (ysc::matrix<int, 2, 2>{11, 12, 13, 14}));
}

TEST(MatrixMap, WorksWithFloatToDouble) {
    ysc::matrix<float, 3> m{1.0f, 2.0f, 3.0f};
    auto result = m.map([](float v) -> double { return static_cast<double>(v) * 0.5; });
    static_assert(std::is_same_v<decltype(result), ysc::matrix<double, 3>>);
    EXPECT_DOUBLE_EQ(result(0), 0.5);
    EXPECT_DOUBLE_EQ(result(1), 1.0);
    EXPECT_DOUBLE_EQ(result(2), 1.5);
}
