// SPDX-License-Identifier: MIT
// Tests for the multi-index overload of ysc::generate (US-069).

#include <matrix.hpp>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// Compile-time checks
// ---------------------------------------------------------------------------

// 2D: generate<int,2,2> with multi-index callable is constexpr
static_assert(ysc::generate<int, 2, 2>([](std::size_t i, std::size_t j) {
                  return int(i + j);
              })(1, 1) == 2);

// 2D: identity matrix
static_assert(ysc::generate<int, 3, 3>([](std::size_t i, std::size_t j) {
                  return i == j ? 1 : 0;
              })(1, 1) == 1);
static_assert(ysc::generate<int, 3, 3>([](std::size_t i, std::size_t j) {
                  return i == j ? 1 : 0;
              })(0, 1) == 0);

// 3D: tensor constexpr
static_assert(ysc::generate<int, 2, 3, 4>([](std::size_t i, std::size_t j, std::size_t k) {
                  return int((i * 100) + (j * 10) + k);
              })(1, 2, 3) == 123);

// Backward-compat: linear-index overload still compiles and is constexpr
static_assert(ysc::generate<int, 4>([](std::size_t k) { return int(k * k); })(2) == 4);

// ---------------------------------------------------------------------------
// Runtime tests
// ---------------------------------------------------------------------------

TEST(generate_multi_index, identity_2d) {
    auto m = ysc::generate<int, 3, 3>([](std::size_t i, std::size_t j) { return i == j ? 1 : 0; });

    for (std::size_t i = 0; i < 3; ++i) {
        for (std::size_t j = 0; j < 3; ++j) {
            EXPECT_EQ(m(i, j), i == j ? 1 : 0) << "at (" << i << ", " << j << ")";
        }
    }
}

TEST(generate_multi_index, sum_of_coords_2d) {
    auto m = ysc::generate<int, 2, 3>([](std::size_t i, std::size_t j) { return int(i + j); });

    EXPECT_EQ(m(0, 0), 0);
    EXPECT_EQ(m(0, 2), 2);
    EXPECT_EQ(m(1, 0), 1);
    EXPECT_EQ(m(1, 2), 3);
}

TEST(generate_multi_index, tensor_3d) {
    auto m = ysc::generate<int, 2, 3, 4>(
        [](auto i, auto j, auto k) { return int((i * 100) + (j * 10) + k); });

    EXPECT_EQ(m(0, 0, 0), 0);
    EXPECT_EQ(m(1, 2, 3), 123);
    EXPECT_EQ(m(0, 2, 3), 23);
    EXPECT_EQ(m(1, 0, 0), 100);
}

TEST(generate_multi_index, retro_compat_linear) {
    // The old single-size_t overload must remain usable.
    auto v = ysc::generate<int, 4>([](std::size_t k) { return int(k * k); });

    EXPECT_EQ(v(0), 0);
    EXPECT_EQ(v(1), 1);
    EXPECT_EQ(v(2), 4);
    EXPECT_EQ(v(3), 9);
}

TEST(generate_multi_index, non_trivial_type) {
    auto m = ysc::generate<double, 2, 2>(
        [](std::size_t i, std::size_t j) { return double(i) + (0.1 * double(j)); });

    EXPECT_DOUBLE_EQ(m(0, 0), 0.0);
    EXPECT_DOUBLE_EQ(m(0, 1), 0.1);
    EXPECT_DOUBLE_EQ(m(1, 0), 1.0);
    EXPECT_DOUBLE_EQ(m(1, 1), 1.1);
}

TEST(generate_multi_index, row_major_order) {
    // Verify that (i,j) coordinates follow row-major layout:
    // element at linear index k has i = k / cols, j = k % cols.
    constexpr std::size_t R = 3;
    constexpr std::size_t C = 4;

    auto m =
        ysc::generate<int, R, C>([](std::size_t i, std::size_t j) { return int((i * 10) + j); });

    for (std::size_t i = 0; i < R; ++i) {
        for (std::size_t j = 0; j < C; ++j) {
            EXPECT_EQ(m(i, j), int((i * 10) + j)) << "at (" << i << ", " << j << ")";
        }
    }
}
