#include <matrix.hpp>
#include <matrix_view.hpp>

#include <gtest/gtest.h>
#include <stdexcept>
#include <type_traits>

// ─── compile-time assertions ──────────────────────────────────────────────────

static_assert(
    std::is_same_v<decltype(std::declval<ysc::matrix<int, 4, 4>&>().submatrix<2, 2>({0, 0})),
                   ysc::matrix_view<int, ysc::strided, 2, 2>>);
static_assert(
    std::is_same_v<decltype(std::declval<const ysc::matrix<int, 4, 4>&>().submatrix<2, 2>({0, 0})),
                   ysc::matrix_view<const int, ysc::strided, 2, 2>>);
static_assert(std::is_same_v<
              decltype(std::declval<ysc::matrix<int, 3, 4, 5>&>().submatrix<2, 3, 4>({0, 0, 0})),
              ysc::matrix_view<int, ysc::strided, 2, 3, 4>>);

// ─── submatrix 2D ─────────────────────────────────────────────────────────────

TEST(submatrix, element_values_2d) {
    // clang-format off
    ysc::matrix<int, 4, 4> m{ 1,  2,  3,  4,
                               5,  6,  7,  8,
                               9, 10, 11, 12,
                              13, 14, 15, 16};
    // clang-format on
    auto sub = m.submatrix<2, 2>({1, 1});
    EXPECT_EQ(sub(0, 0), m(1, 1)); // 6
    EXPECT_EQ(sub(0, 1), m(1, 2)); // 7
    EXPECT_EQ(sub(1, 0), m(2, 1)); // 10
    EXPECT_EQ(sub(1, 1), m(2, 2)); // 11
}

TEST(submatrix, mutation_reflected_in_original) {
    // clang-format off
    ysc::matrix<int, 4, 4> m{ 1,  2,  3,  4,
                               5,  6,  7,  8,
                               9, 10, 11, 12,
                              13, 14, 15, 16};
    // clang-format on
    auto sub = m.submatrix<2, 2>({1, 1});
    sub(0, 0) = 99;
    EXPECT_EQ(m(1, 1), 99);
    sub(1, 1) = 42;
    EXPECT_EQ(m(2, 2), 42);
}

TEST(submatrix, original_mutation_reflected_in_view) {
    // clang-format off
    ysc::matrix<int, 4, 4> m{ 1,  2,  3,  4,
                               5,  6,  7,  8,
                               9, 10, 11, 12,
                              13, 14, 15, 16};
    // clang-format on
    auto sub = m.submatrix<2, 2>({1, 1});
    m(2, 2) = 77;
    EXPECT_EQ(sub(1, 1), 77);
}

TEST(submatrix, origin_at_zero) {
    ysc::matrix<int, 3, 3> m{1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto sub = m.submatrix<2, 2>({0, 0});
    EXPECT_EQ(sub(0, 0), 1);
    EXPECT_EQ(sub(0, 1), 2);
    EXPECT_EQ(sub(1, 0), 4);
    EXPECT_EQ(sub(1, 1), 5);
}

TEST(submatrix, full_extent_equals_whole_matrix) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto sub = m.submatrix<2, 3>({0, 0});
    for (std::size_t i = 0; i < 2; ++i) {
        for (std::size_t j = 0; j < 3; ++j) {
            EXPECT_EQ(sub(i, j), m(i, j));
        }
    }
}

TEST(submatrix, const_matrix_gives_const_view) {
    // clang-format off
    const ysc::matrix<int, 4, 4> m{ 1,  2,  3,  4,
                                     5,  6,  7,  8,
                                     9, 10, 11, 12,
                                    13, 14, 15, 16};
    // clang-format on
    auto sub = m.submatrix<2, 2>({1, 1});
    static_assert(std::is_const_v<std::remove_reference_t<decltype(sub(0, 0))>>);
    EXPECT_EQ(sub(0, 0), 6);
    EXPECT_EQ(sub(1, 1), 11);
}

TEST(submatrix, single_element_submatrix) {
    ysc::matrix<int, 3, 3> m{1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto sub = m.submatrix<1, 1>({1, 2});
    EXPECT_EQ(sub(0, 0), m(1, 2)); // 6
}

// ─── submatrix out-of-range ───────────────────────────────────────────────────

TEST(submatrix, throws_when_block_exceeds_dimension) {
    // clang-format off
    ysc::matrix<int, 4, 4> m{ 1,  2,  3,  4,
                               5,  6,  7,  8,
                               9, 10, 11, 12,
                              13, 14, 15, 16};
    // clang-format on
    // Capturing lambda is not a coroutine; wrapping the throwing call for EXPECT_THROW.
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-capturing-lambda-coroutines)
    auto bad = [&] { (void)m.submatrix<3, 3>({2, 2}); };
    EXPECT_THROW(bad(), std::out_of_range);
}

TEST(submatrix, throws_on_row_overflow) {
    ysc::matrix<int, 3, 5> m{};
    // Capturing lambda is not a coroutine; wrapping the throwing call for EXPECT_THROW.
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-capturing-lambda-coroutines)
    auto bad = [&] { (void)m.submatrix<2, 3>({2, 0}); };
    EXPECT_THROW(bad(), std::out_of_range);
}

TEST(submatrix, throws_on_col_overflow) {
    ysc::matrix<int, 5, 3> m{};
    // Capturing lambda is not a coroutine; wrapping the throwing call for EXPECT_THROW.
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-capturing-lambda-coroutines)
    auto bad = [&] { (void)m.submatrix<3, 2>({0, 2}); };
    EXPECT_THROW(bad(), std::out_of_range);
}

TEST(submatrix, throws_on_const_matrix_overflow) {
    const ysc::matrix<int, 4, 4> m{};
    // Capturing lambda is not a coroutine; wrapping the throwing call for EXPECT_THROW.
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-capturing-lambda-coroutines)
    auto bad = [&] { (void)m.submatrix<3, 3>({2, 2}); };
    EXPECT_THROW(bad(), std::out_of_range);
}

TEST(submatrix, does_not_throw_on_exact_boundary) {
    ysc::matrix<int, 4, 4> m{};
    // Capturing lambda is not a coroutine; wrapping the call for EXPECT_NO_THROW.
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-capturing-lambda-coroutines)
    auto ok = [&] { (void)m.submatrix<2, 2>({2, 2}); };
    EXPECT_NO_THROW(ok());
}

// ─── submatrix 3D ─────────────────────────────────────────────────────────────

TEST(submatrix, element_values_3d) {
    // Fill a 3x4x5 matrix with linear values
    ysc::matrix<int, 3, 4, 5> m{};
    for (std::size_t i = 0; i < 3; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            for (std::size_t k = 0; k < 5; ++k) {
                m(i, j, k) = static_cast<int>((i * 20) + (j * 5) + k);
            }
        }
    }
    auto sub = m.submatrix<2, 3, 4>({1, 1, 1});
    EXPECT_EQ(sub(0, 0, 0), m(1, 1, 1));
    EXPECT_EQ(sub(0, 0, 1), m(1, 1, 2));
    EXPECT_EQ(sub(0, 1, 0), m(1, 2, 1));
    EXPECT_EQ(sub(1, 0, 0), m(2, 1, 1));
    EXPECT_EQ(sub(1, 2, 3), m(2, 3, 4));
}

TEST(submatrix, mutation_reflected_3d) {
    ysc::matrix<int, 3, 4, 5> m{};
    auto sub = m.submatrix<2, 3, 4>({1, 1, 1});
    sub(0, 0, 0) = 42;
    EXPECT_EQ(m(1, 1, 1), 42);
}
