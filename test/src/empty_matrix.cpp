#include "matrix.hpp"
#include <gtest/gtest.h>
#include <stdexcept>

// --- static checks ---

static_assert(ysc::matrix<int, 0>::size() == 0);
static_assert(ysc::matrix<int, 0>::empty());

static_assert(ysc::matrix<int, 2, 0, 3>::size() == 0);
static_assert(ysc::matrix<int, 2, 0, 3>::empty());

// --- order-1 empty matrix ---

TEST(EmptyMatrix, DefaultConstruct) {
    ysc::matrix<int, 0> m;
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0U);
}

TEST(EmptyMatrix, ZeroConstruct) {
    ysc::matrix<int, 0> m{ysc::zero};
    EXPECT_TRUE(m.empty());
}

TEST(EmptyMatrix, CopyConstruct) {
    ysc::matrix<int, 0> a;
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization) -- tests empty matrix copy ctor
    ysc::matrix<int, 0> b = a;
    EXPECT_EQ(a, b);
}

TEST(EmptyMatrix, FillIsNoop) {
    ysc::matrix<int, 0> m;
    m.fill(42);
    EXPECT_TRUE(m.empty());
}

TEST(EmptyMatrix, BeginEqualsEnd) {
    ysc::matrix<int, 0> m;
    EXPECT_EQ(m.begin(), m.end());
    EXPECT_EQ(m.cbegin(), m.cend());
}

TEST(EmptyMatrix, AtThrows) {
    ysc::matrix<int, 0> m;
    EXPECT_THROW(m.at(0), std::out_of_range);
}

TEST(EmptyMatrix, SumIsZero) {
    ysc::matrix<int, 0> m;
    EXPECT_EQ(m.sum(), 0);
}

TEST(EmptyMatrix, AllVacuouslyTrue) {
    ysc::matrix<int, 0> m;
    EXPECT_TRUE(m.all());
}

TEST(EmptyMatrix, AnyVacuouslyFalse) {
    ysc::matrix<int, 0> m;
    EXPECT_FALSE(m.any());
}

TEST(EmptyMatrix, ApplyIsNoop) {
    ysc::matrix<int, 0> m;
    m.apply([](int& v) { v = 99; });
    EXPECT_TRUE(m.empty());
}

TEST(EmptyMatrix, MapReturnsEmpty) {
    ysc::matrix<int, 0> m;
    auto result = m.map([](int v) { return v * 2; });
    EXPECT_TRUE(result.empty());
}

TEST(EmptyMatrix, EqualityHolds) {
    ysc::matrix<int, 0> a;
    ysc::matrix<int, 0> b;
    EXPECT_EQ(a, b);
}

// --- order-3 empty matrix (one zero dimension) ---

TEST(EmptyMatrix, MultiDimDefaultConstruct) {
    ysc::matrix<int, 2, 0, 3> m;
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0U);
}

TEST(EmptyMatrix, MultiDimBeginEqualsEnd) {
    ysc::matrix<int, 2, 0, 3> m;
    EXPECT_EQ(m.begin(), m.end());
    EXPECT_EQ(m.cbegin(), m.cend());
}

TEST(EmptyMatrix, MultiDimAtThrows) {
    ysc::matrix<int, 2, 0, 3> m;
    EXPECT_THROW(m.at(0, 0, 0), std::out_of_range);
    EXPECT_THROW(m.at(1, 0, 2), std::out_of_range);
}
