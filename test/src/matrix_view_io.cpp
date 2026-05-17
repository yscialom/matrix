#include <matrix.hpp>
#include <matrix_view.hpp>

#include <gtest/gtest.h>

#include <sstream>
#include <type_traits>

// ─── static assertions: const_matrix_view alias ──────────────────────────────

static_assert(
    std::same_as<ysc::const_matrix_view<int, 3>, ysc::matrix_view<const int, ysc::contiguous, 3>>);
static_assert(std::same_as<ysc::const_matrix_view<int, 2, 3>,
                           ysc::matrix_view<const int, ysc::contiguous, 2, 3>>);

// ─── operator<< for matrix_view<T, contiguous, Dims...> ──────────────────────

TEST(matrix_view_io, ostream_1d) {
    ysc::matrix<int, 3> m{1, 2, 3};
    auto v = m.flatten();
    std::ostringstream oss;
    oss << v;
    ASSERT_EQ(oss.str(), "[1, 2, 3]");
}

TEST(matrix_view_io, ostream_2d) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto v = m.reshape<2, 3>();
    std::ostringstream oss;
    oss << v;
    ASSERT_EQ(oss.str(), "[[1, 2, 3], [4, 5, 6]]");
}

TEST(matrix_view_io, ostream_row) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    std::ostringstream oss;
    oss << m.row(0);
    ASSERT_EQ(oss.str(), "[1, 2, 3]");
}

TEST(matrix_view_io, ostream_matches_matrix) {
    ysc::matrix<int, 2, 3> m{10, 20, 30, 40, 50, 60};
    std::ostringstream oss_m;
    std::ostringstream oss_v;
    oss_m << m;
    oss_v << m.reshape<2, 3>();
    ASSERT_EQ(oss_m.str(), oss_v.str());
}

TEST(matrix_view_io, ostream_chaining) {
    ysc::matrix<int, 2> m{7, 8};
    auto v = m.flatten();
    std::ostringstream oss;
    oss << v << "!";
    ASSERT_EQ(oss.str(), "[7, 8]!");
}

TEST(matrix_view_io, ostream_const_view) {
    const ysc::matrix<int, 3> m{1, 2, 3};
    ysc::const_matrix_view<int, 3> v{m};
    std::ostringstream oss;
    oss << v;
    ASSERT_EQ(oss.str(), "[1, 2, 3]");
}

// ─── const ctor (matrix_view<const T, contiguous, ...>) ──────────────────────

TEST(matrix_view_io, const_view_from_const_matrix) {
    const ysc::matrix<int, 3, 3> m{1, 2, 3, 4, 5, 6, 7, 8, 9};
    ysc::const_matrix_view<int, 3, 3> v{m};
    ASSERT_EQ(v(0, 0), 1);
    ASSERT_EQ(v(1, 1), 5);
    ASSERT_EQ(v(2, 2), 9);
}

TEST(matrix_view_io, const_view_alias) {
    const ysc::matrix<int, 3> m{10, 20, 30};
    ysc::const_matrix_view<int, 3> v{m};
    ASSERT_EQ(v(0), 10);
    ASSERT_EQ(v(2), 30);
}

TEST(matrix_view_io, const_view_reflects_mutations) {
    ysc::matrix<int, 3> m{1, 2, 3};
    const ysc::matrix<int, 3>& cm = m;
    ysc::const_matrix_view<int, 3> v{cm};
    ASSERT_EQ(v(0), 1);
    m(0) = 99;
    ASSERT_EQ(v(0), 99); // view sees the updated value
}

// ─── slice() on matrix_view<T, contiguous, ...> ──────────────────────────────

TEST(matrix_view_io, contiguous_view_slice_prefix) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto v2d = m.reshape<3, 4>();
    // slice(0) on a 2D view → fix first dim → contiguous view of 4 elements
    auto row0 = v2d.slice(0);
    static_assert(std::same_as<decltype(row0), ysc::matrix_view<int, ysc::contiguous, 4>>);
    ASSERT_EQ(row0(0), 1);
    ASSERT_EQ(row0(3), 4);
}

TEST(matrix_view_io, contiguous_view_slice_non_prefix) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto v2d = m.reshape<3, 4>();
    // slice(all, 1) on a 2D view → fix second dim → strided view of 3 elements
    auto col1 = v2d.slice(ysc::all, 1);
    static_assert(std::same_as<decltype(col1), ysc::matrix_view<int, ysc::strided, 3>>);
    ASSERT_EQ(col1(0), 2);
    ASSERT_EQ(col1(1), 6);
    ASSERT_EQ(col1(2), 10);
}

TEST(matrix_view_io, contiguous_view_row) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto v2d = m.reshape<3, 4>();
    auto r1 = v2d.row(1);
    static_assert(std::same_as<decltype(r1), ysc::matrix_view<int, ysc::contiguous, 4>>);
    ASSERT_EQ(r1(0), 5);
    ASSERT_EQ(r1(3), 8);
}

TEST(matrix_view_io, contiguous_view_col) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto v2d = m.reshape<3, 4>();
    auto c2 = v2d.col(2);
    static_assert(std::same_as<decltype(c2), ysc::matrix_view<int, ysc::strided, 3>>);
    ASSERT_EQ(c2(0), 3);
    ASSERT_EQ(c2(1), 7);
    ASSERT_EQ(c2(2), 11);
}

TEST(matrix_view_io, contiguous_view_row_out_of_range) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto v = m.reshape<2, 3>();
    // NOLINTNEXTLINE(bugprone-unused-return-value,clang-analyzer-cplusplus.Move)
    ASSERT_THROW({ auto r = v.row(2); }, std::out_of_range);
}

TEST(matrix_view_io, contiguous_view_col_out_of_range) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto v = m.reshape<2, 3>();
    // NOLINTNEXTLINE(bugprone-unused-return-value,clang-analyzer-cplusplus.Move)
    ASSERT_THROW({ auto c = v.col(3); }, std::out_of_range);
}

TEST(matrix_view_io, contiguous_view_slice_out_of_range) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto v = m.reshape<2, 3>();
    // Fix first dim with an out-of-bounds index → throws
    // NOLINTNEXTLINE(bugprone-unused-return-value,clang-analyzer-cplusplus.Move)
    ASSERT_THROW({ auto s = v.slice(5); }, std::out_of_range);
}

TEST(matrix_view_io, contiguous_view_slice_non_prefix_out_of_range) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto v = m.reshape<2, 3>();
    // Fix second dim with an out-of-bounds index → throws
    // NOLINTNEXTLINE(bugprone-unused-return-value,clang-analyzer-cplusplus.Move)
    ASSERT_THROW({ auto s = v.slice(ysc::all, 10); }, std::out_of_range);
}

TEST(matrix_view_io, contiguous_view_slice_all_dims) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto v = m.reshape<2, 3>();
    // slice(all, all) — all specs are all_t, no bounds check, prefix pattern → contiguous view
    auto s = v.slice(ysc::all, ysc::all);
    static_assert(std::same_as<decltype(s), ysc::matrix_view<int, ysc::contiguous, 2, 3>>);
    ASSERT_EQ(s(0, 0), 1);
    ASSERT_EQ(s(1, 2), 6);
}

// ─── std::formatter for matrix_view<T, contiguous, Dims...> ──────────────────

#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
#include <format>

TEST(matrix_view_io, formatter_1d) {
    ysc::matrix<int, 3> m{1, 2, 3};
    ASSERT_EQ(std::format("{}", m.flatten()), "[1, 2, 3]");
}

TEST(matrix_view_io, formatter_2d_row) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    ASSERT_EQ(std::format("{}", m.row(0)), "[1, 2, 3]");
    ASSERT_EQ(std::format("{}", m.row(1)), "[4, 5, 6]");
}

TEST(matrix_view_io, formatter_matches_ostream) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto v = m.reshape<2, 3>();
    std::ostringstream oss;
    oss << v;
    ASSERT_EQ(std::format("{}", v), oss.str());
}

#else

TEST(matrix_view_io, formatter_not_available) {
    GTEST_SKIP() << "std::format not available on this compiler";
}

#endif // defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
