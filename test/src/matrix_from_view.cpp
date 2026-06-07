#include <matrix.hpp>
#include <matrix_view.hpp>

#include <array>
#include <gtest/gtest.h>
#include <type_traits>

// ─── compile-time assertions ──────────────────────────────────────────────────

static_assert(
    std::is_constructible_v<ysc::matrix<int, 4>, const ysc::matrix_view<int, ysc::contiguous, 4>&>);
static_assert(
    std::is_constructible_v<ysc::matrix<int, 3>, const ysc::matrix_view<int, ysc::strided, 3>&>);
// Constructors are explicit: no implicit conversion
static_assert(
    !std::is_convertible_v<ysc::matrix_view<int, ysc::contiguous, 4>, ysc::matrix<int, 4>>);
static_assert(!std::is_convertible_v<ysc::matrix_view<int, ysc::strided, 3>, ysc::matrix<int, 3>>);

// ─── detail::index_to_coordinates (compile-time) ─────────────────────────────

static_assert(ysc::detail::index_to_coordinates(std::array<std::size_t, 1>{3}, std::size_t{0}) ==
              std::array<std::size_t, 1>{0});
static_assert(ysc::detail::index_to_coordinates(std::array<std::size_t, 2>{3, 4}, std::size_t{5}) ==
              std::array<std::size_t, 2>{1, 1}); // 5 = 1×4 + 1
static_assert(ysc::detail::index_to_coordinates(std::array<std::size_t, 2>{3, 4},
                                                std::size_t{11}) ==
              std::array<std::size_t, 2>{2, 3});

// ─── detail::index_to_coordinates (runtime) ──────────────────────────────────

TEST(detail_index_to_coordinates, 1d) {
    EXPECT_EQ((ysc::detail::index_to_coordinates(std::array<std::size_t, 1>{3}, std::size_t{0})),
              (std::array<std::size_t, 1>{0}));
    EXPECT_EQ((ysc::detail::index_to_coordinates(std::array<std::size_t, 1>{3}, std::size_t{2})),
              (std::array<std::size_t, 1>{2}));
}

TEST(detail_index_to_coordinates, 2d_row_major) {
    EXPECT_EQ((ysc::detail::index_to_coordinates(std::array<std::size_t, 2>{3, 4}, std::size_t{0})),
              (std::array<std::size_t, 2>{0, 0}));
    EXPECT_EQ((ysc::detail::index_to_coordinates(std::array<std::size_t, 2>{3, 4}, std::size_t{5})),
              (std::array<std::size_t, 2>{1, 1}));
    EXPECT_EQ(
        (ysc::detail::index_to_coordinates(std::array<std::size_t, 2>{3, 4}, std::size_t{11})),
        (std::array<std::size_t, 2>{2, 3}));
}

// ─── contiguous view (from row) ───────────────────────────────────────────────

TEST(matrix_from_view, contiguous_from_row_copies_values) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto row1 = m.row(1);
    auto m2 = ysc::matrix<int, 4>(row1);
    EXPECT_EQ(m2(0), 5);
    EXPECT_EQ(m2(1), 6);
    EXPECT_EQ(m2(2), 7);
    EXPECT_EQ(m2(3), 8);
}

TEST(matrix_from_view, contiguous_mutation_of_copy_does_not_affect_source) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto m2 = ysc::matrix<int, 4>(m.row(1));
    m2(0) = 99;
    EXPECT_EQ(m(1, 0), 5);
}

TEST(matrix_from_view, contiguous_mutation_of_source_does_not_affect_copy) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto m2 = ysc::matrix<int, 4>(m.row(1));
    m(1, 0) = 99;
    EXPECT_EQ(m2(0), 5);
}

TEST(matrix_from_view, contiguous_from_slice) {
    ysc::matrix<int, 2, 3, 4> m{};
    for (std::size_t i = 0; i < 2; ++i) {
        for (std::size_t j = 0; j < 3; ++j) {
            for (std::size_t k = 0; k < 4; ++k) {
                m(i, j, k) = static_cast<int>((i * 12) + (j * 4) + k + 1);
            }
        }
    }
    auto v = m.slice(1); // contiguous view of m[1]: a 3×4 slice
    auto m2 = ysc::matrix<int, 3, 4>(v);
    EXPECT_EQ(m2(0, 0), m(1, 0, 0));
    EXPECT_EQ(m2(1, 2), m(1, 1, 2));
    EXPECT_EQ(m2(2, 3), m(1, 2, 3));
}

TEST(matrix_from_view, contiguous_2d_explicit) {
    ysc::matrix<int, 2, 3, 4> m{};
    for (std::size_t i = 0; i < 2; ++i) {
        for (std::size_t j = 0; j < 3; ++j) {
            for (std::size_t k = 0; k < 4; ++k) {
                m(i, j, k) = static_cast<int>((i * 12) + (j * 4) + k + 1);
            }
        }
    }
    auto v = m.slice(0); // contiguous view of m[0]: matrix_view<int, contiguous, 3, 4>
    ysc::matrix<int, 3, 4> m2(v);
    for (std::size_t j = 0; j < 3; ++j) {
        for (std::size_t k = 0; k < 4; ++k) {
            EXPECT_EQ(m2(j, k), m(std::size_t{0}, j, k));
        }
    }
}

// ─── strided view (from col) ──────────────────────────────────────────────────

TEST(matrix_from_view, strided_from_col_copies_values) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto col2 = m.col(2);
    auto m2 = ysc::matrix<int, 3>(col2);
    EXPECT_EQ(m2(0), m(0, 2));
    EXPECT_EQ(m2(1), m(1, 2));
    EXPECT_EQ(m2(2), m(2, 2));
}

TEST(matrix_from_view, strided_mutation_of_copy_does_not_affect_source) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto m2 = ysc::matrix<int, 3>(m.col(2));
    m2(0) = 99;
    EXPECT_EQ(m(0, 2), 3);
}

TEST(matrix_from_view, strided_mutation_of_source_does_not_affect_copy) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto m2 = ysc::matrix<int, 3>(m.col(2));
    m(0, 2) = 99;
    EXPECT_EQ(m2(0), 3);
}

TEST(matrix_from_view, strided_from_slice_all_fixed) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto v = m.slice(ysc::all, 1); // strided view of column 1
    auto m2 = ysc::matrix<int, 3>(v);
    EXPECT_EQ(m2(0), m(0, 1));
    EXPECT_EQ(m2(1), m(1, 1));
    EXPECT_EQ(m2(2), m(2, 1));
}

TEST(matrix_from_view, strided_2d_from_3d_slice) {
    ysc::matrix<int, 2, 3, 4> m{};
    for (std::size_t i = 0; i < 2; ++i) {
        for (std::size_t j = 0; j < 3; ++j) {
            for (std::size_t k = 0; k < 4; ++k) {
                m(i, j, k) = static_cast<int>((i * 12) + (j * 4) + k + 1);
            }
        }
    }
    // slice(ysc::all, 1) → spec (all_t, 1, all_t): keeps dims 0 and 2, fixes dim 1 at 1
    // result: matrix_view<int, strided, 2, 4>
    auto v = m.slice(ysc::all, 1);
    ysc::matrix<int, 2, 4> m2(v);
    for (std::size_t i = 0; i < 2; ++i) {
        for (std::size_t k = 0; k < 4; ++k) {
            EXPECT_EQ(m2(i, k), m(i, std::size_t{1}, k));
        }
    }
}

// ─── CTAD ─────────────────────────────────────────────────────────────────────

TEST(matrix_from_view, ctad_contiguous) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto row0 = m.row(0);
    auto m2 = ysc::matrix(row0);
    static_assert(std::is_same_v<decltype(m2), ysc::matrix<int, 4>>);
    EXPECT_EQ(m2(0), 1);
    EXPECT_EQ(m2(3), 4);
}

TEST(matrix_from_view, ctad_strided) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto col0 = m.col(0);
    auto m2 = ysc::matrix(col0);
    static_assert(std::is_same_v<decltype(m2), ysc::matrix<int, 3>>);
    EXPECT_EQ(m2(0), 1);
    EXPECT_EQ(m2(2), 9);
}

// ─── US-058: strided ctor uses iterators ──────────────────────────────────────

TEST(matrix_from_view, strided_1d_iterator_copies_correct_values) {
    // Verify that the 1-D strided ctor (now using iterators) yields the same
    // result as constructing element by element.
    ysc::matrix<int, 4, 5> m{};
    for (std::size_t i = 0; i < 4; ++i) {
        for (std::size_t j = 0; j < 5; ++j) {
            m(i, j) = static_cast<int>((i * 5) + j + 1);
        }
    }
    // col(3): elements m(0,3), m(1,3), m(2,3), m(3,3) — values 4, 9, 14, 19
    auto col3 = m.col(3);
    auto m2 = ysc::matrix<int, 4>(col3);
    EXPECT_EQ(m2(0), m(0, 3));
    EXPECT_EQ(m2(1), m(1, 3));
    EXPECT_EQ(m2(2), m(2, 3));
    EXPECT_EQ(m2(3), m(3, 3));
}

TEST(matrix_from_view, strided_1d_copy_does_not_alias_source) {
    // After construction, mutating the copy must not affect the original.
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto col1 = m.col(1);
    auto m2 = ysc::matrix<int, 3>(col1);
    // Mutate the copy
    m2(0) = 42;
    m2(1) = 43;
    m2(2) = 44;
    // Source must be unchanged
    EXPECT_EQ(m(0, 1), 2);
    EXPECT_EQ(m(1, 1), 6);
    EXPECT_EQ(m(2, 1), 10);
    // And mutating the source must not affect the copy
    m(0, 1) = 99;
    EXPECT_EQ(m2(0), 42);
}
