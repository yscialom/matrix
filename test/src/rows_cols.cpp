#include "matrix.hpp"
#include <gtest/gtest.h>

// ─── rows() ──────────────────────────────────────────────────────────────────

TEST(MatrixRows, IterationCountMatchesRowCount) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    std::size_t count = 0;
    for (auto row_view : m.rows()) {
        (void)row_view;
        ++count;
    }
    EXPECT_EQ(count, 3U);
}

TEST(MatrixRows, RowValuesAreCorrect) {
    // row 0: {1,2,3}, row 1: {4,5,6}
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto it = m.rows().begin();
    auto row0 = *it++;
    auto row1 = *it;
    EXPECT_EQ(row0(0), 1);
    EXPECT_EQ(row0(1), 2);
    EXPECT_EQ(row0(2), 3);
    EXPECT_EQ(row1(0), 4);
    EXPECT_EQ(row1(1), 5);
    EXPECT_EQ(row1(2), 6);
}

TEST(MatrixRows, ConstOverloadCompiles) {
    const ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    std::size_t count = 0;
    for (auto row_view : m.rows()) {
        (void)row_view;
        ++count;
    }
    EXPECT_EQ(count, 2U);
}

TEST(MatrixRows, RowViewMutatesOriginalMatrix) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    for (auto row_view : m.rows()) {
        row_view(0) = 99;
    }
    EXPECT_EQ(m(0, 0), 99);
    EXPECT_EQ(m(1, 0), 99);
}

TEST(MatrixRows, ConstRowViewDoesNotAllowMutation) {
    // Compile-time check: const matrix yields const views
    const ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    for (auto row_view : m.rows()) {
        static_assert(std::is_const_v<std::remove_reference_t<decltype(row_view(0))>>);
        (void)row_view;
    }
}

// rows() unavailable for order != 2
// Use a void_t SFINAE helper to avoid GCC hard errors on constrained templates
namespace test_detail {
template <class M, class = void> struct has_rows : std::false_type {};
template <class M>
struct has_rows<M, std::void_t<decltype(std::declval<M&>().rows())>> : std::true_type {};
} // namespace test_detail
static_assert(!test_detail::has_rows<ysc::matrix<int, 3, 4, 5>>::value,
              "rows() must be unavailable for order != 2");

// ─── cols() ──────────────────────────────────────────────────────────────────

TEST(MatrixCols, IterationCountMatchesColCount) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    std::size_t count = 0;
    for (auto col_view : m.cols()) {
        (void)col_view;
        ++count;
    }
    EXPECT_EQ(count, 4U);
}

TEST(MatrixCols, ColValuesAreCorrect) {
    // col 0: {1,4}, col 1: {2,5}, col 2: {3,6}
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto it = m.cols().begin();
    auto col0 = *it++;
    auto col1 = *it++;
    auto col2 = *it;
    EXPECT_EQ(col0(0), 1);
    EXPECT_EQ(col0(1), 4);
    EXPECT_EQ(col1(0), 2);
    EXPECT_EQ(col1(1), 5);
    EXPECT_EQ(col2(0), 3);
    EXPECT_EQ(col2(1), 6);
}

TEST(MatrixCols, ConstOverloadCompiles) {
    const ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    std::size_t count = 0;
    for (auto col_view : m.cols()) {
        (void)col_view;
        ++count;
    }
    EXPECT_EQ(count, 3U);
}

TEST(MatrixCols, ColViewMutatesOriginalMatrix) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    for (auto col_view : m.cols()) {
        col_view(0) = 99;
    }
    EXPECT_EQ(m(0, 0), 99);
    EXPECT_EQ(m(0, 1), 99);
    EXPECT_EQ(m(0, 2), 99);
}

TEST(MatrixCols, ConstColViewDoesNotAllowMutation) {
    const ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    for (auto col_view : m.cols()) {
        static_assert(std::is_const_v<std::remove_reference_t<decltype(col_view(0))>>);
        (void)col_view;
    }
}

// cols() unavailable for order != 2
namespace test_detail {
template <class M, class = void> struct has_cols : std::false_type {};
template <class M>
struct has_cols<M, std::void_t<decltype(std::declval<M&>().cols())>> : std::true_type {};
} // namespace test_detail
static_assert(!test_detail::has_cols<ysc::matrix<int, 3, 4, 5>>::value,
              "cols() must be unavailable for order != 2");
