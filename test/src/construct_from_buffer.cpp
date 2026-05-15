#include <array>
#include <span>

#include <gtest/gtest.h>

#include "matrix.hpp"

TEST(construct_from_array, basic) {
    ysc::matrix<int, 3> m(std::array<int, 3>{1, 2, 3});
    EXPECT_EQ(m(0), 1);
    EXPECT_EQ(m(2), 3);
}

TEST(construct_from_array, values_are_moved) {
    std::array<int, 3> arr{10, 20, 30};
    // NOLINTNEXTLINE(performance-move-const-arg) -- demonstrates move ctor; trivial copy is fine
    ysc::matrix<int, 3> m(std::move(arr));
    EXPECT_EQ(m(0), 10);
    EXPECT_EQ(m(1), 20);
    EXPECT_EQ(m(2), 30);
}

TEST(construct_from_array, 2d) {
    ysc::matrix<int, 2, 3> m(std::array<int, 6>{1, 2, 3, 4, 5, 6});
    EXPECT_EQ(m(0, 0), 1);
    EXPECT_EQ(m(1, 2), 6);
}

TEST(construct_from_span, basic) {
    std::array<int, 3> buf{4, 5, 6};
    ysc::matrix<int, 3> m(std::span<const int, 3>{buf.data(), 3});
    EXPECT_EQ(m(0), 4);
}

TEST(construct_from_span, copies_not_aliases) {
    std::array<int, 3> buf{7, 8, 9};
    ysc::matrix<int, 3> m(std::span<const int, 3>{buf.data(), 3});
    buf[0] = 99;
    EXPECT_EQ(m(0), 7); // m is independent of buf
}

TEST(construct_from_span, 2d) {
    std::array<int, 6> buf{1, 2, 3, 4, 5, 6};
    ysc::matrix<int, 2, 3> m(std::span<const int, 6>{buf.data(), 6});
    EXPECT_EQ(m(0, 0), 1);
    EXPECT_EQ(m(1, 2), 6);
}

TEST(generate_linear, basic) {
    auto m = ysc::generate<int, 3>([](std::size_t i) { return static_cast<int>(i * 2); });
    EXPECT_EQ(m(0), 0);
    EXPECT_EQ(m(1), 2);
    EXPECT_EQ(m(2), 4);
}

TEST(generate_linear, identity_function) {
    auto m = ysc::generate<int, 5>([](std::size_t i) { return static_cast<int>(i); });
    for (std::size_t i = 0; i < 5; ++i) {
        EXPECT_EQ(m(static_cast<int>(i)), static_cast<int>(i));
    }
}

TEST(generate_linear, 2d) {
    auto m = ysc::generate<int, 2, 3>([](std::size_t i) { return static_cast<int>(i); });
    // linear indices: (0,0)=0, (0,1)=1, (0,2)=2, (1,0)=3, (1,1)=4, (1,2)=5
    EXPECT_EQ(m(0, 0), 0);
    EXPECT_EQ(m(0, 2), 2);
    EXPECT_EQ(m(1, 0), 3);
    EXPECT_EQ(m(1, 2), 5);
}
