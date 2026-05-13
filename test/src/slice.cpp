#include <matrix.hpp>
#include <matrix_view.hpp>

#include <concepts>
#include <gtest/gtest.h>
#include <type_traits>

// ─── helpers ─────────────────────────────────────────────────────────────────

// Accept any strided view of size N (used to test implicit conversion)
template <class T, std::size_t N> T sum_strided(ysc::matrix_view<T, ysc::strided, N> v) {
    T result{};
    for (std::size_t i = 0; i < N; ++i) {
        result += v(i);
    }
    return result;
}

// ─── static assertions: return types ─────────────────────────────────────────

// 3D matrix: slice(i, all, all) → contiguous (prefix of fixed dims)
static_assert(
    std::same_as<decltype(std::declval<ysc::matrix<int, 3, 4, 5>&>().slice(0, ysc::all, ysc::all)),
                 ysc::matrix_view<int, ysc::contiguous, 4, 5>>);

// 3D matrix: slice(i) → padding → (i, all, all) → contiguous
static_assert(std::same_as<decltype(std::declval<ysc::matrix<int, 3, 4, 5>&>().slice(0)),
                           ysc::matrix_view<int, ysc::contiguous, 4, 5>>);

// 3D matrix: slice() → padding → (all, all, all) → contiguous (full view)
static_assert(std::same_as<decltype(std::declval<ysc::matrix<int, 3, 4, 5>&>().slice()),
                           ysc::matrix_view<int, ysc::contiguous, 3, 4, 5>>);

// 3D matrix: slice(all, j, all) → strided (non-prefix)
static_assert(
    std::same_as<decltype(std::declval<ysc::matrix<int, 3, 4, 5>&>().slice(ysc::all, 0, ysc::all)),
                 ysc::matrix_view<int, ysc::strided, 3, 5>>);

// 3D matrix: slice(all, all, k) → strided (non-prefix)
static_assert(
    std::same_as<decltype(std::declval<ysc::matrix<int, 3, 4, 5>&>().slice(ysc::all, ysc::all, 0)),
                 ysc::matrix_view<int, ysc::strided, 3, 4>>);

// 2D matrix: row() → contiguous, col() → strided
static_assert(std::same_as<decltype(std::declval<ysc::matrix<int, 3, 4>&>().row(0)),
                           ysc::matrix_view<int, ysc::contiguous, 4>>);
static_assert(std::same_as<decltype(std::declval<ysc::matrix<int, 3, 4>&>().col(0)),
                           ysc::matrix_view<int, ysc::strided, 3>>);

// const matrix → const T in view
static_assert(std::same_as<decltype(std::declval<const ysc::matrix<int, 3, 4>&>().slice(0)),
                           ysc::matrix_view<const int, ysc::contiguous, 4>>);
static_assert(std::same_as<decltype(std::declval<const ysc::matrix<int, 3, 4>&>().col(0)),
                           ysc::matrix_view<const int, ysc::strided, 3>>);

// sizeof(strided view) == sizeof(T*) + order * sizeof(size_t)
static_assert(sizeof(ysc::matrix_view<int, ysc::strided, 3>) ==
              sizeof(int*) + 1 * sizeof(std::size_t));
static_assert(sizeof(ysc::matrix_view<int, ysc::strided, 3, 4>) ==
              sizeof(int*) + 2 * sizeof(std::size_t));

// ─── slice — prefix (contiguous) ─────────────────────────────────────────────

TEST(slice_contiguous, row_via_slice_reads_correctly) {
    // matrix<int, 3, 4>: rows are [0..3],[4..7],[8..11]
    ysc::matrix<int, 3, 4> m{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    auto v = m.slice(1); // row 1: {4,5,6,7}
    EXPECT_EQ(v(0), 4);
    EXPECT_EQ(v(1), 5);
    EXPECT_EQ(v(2), 6);
    EXPECT_EQ(v(3), 7);
}

TEST(slice_contiguous, mutation_reflected_in_matrix) {
    ysc::matrix<int, 3, 4> m{};
    auto v = m.slice(1);
    v(2) = 99;
    EXPECT_EQ(m(1, 2), 99);
}

TEST(slice_contiguous, full_view_via_slice_no_args) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto v = m.slice(); // full view
    EXPECT_EQ(v(0, 0), 1);
    EXPECT_EQ(v(1, 2), 6);
    v(0, 1) = 77;
    EXPECT_EQ(m(0, 1), 77);
}

TEST(slice_contiguous, prefix_explicit_all) {
    ysc::matrix<int, 3, 4, 5> m{};
    m(2, 3, 4) = 42;
    auto v = m.slice(2, ysc::all, ysc::all); // contiguous subslice: layer 2
    EXPECT_EQ(v(3, 4), 42);
}

TEST(slice_contiguous, two_fixed_dims_prefix) {
    ysc::matrix<int, 3, 4, 5> m{};
    m(1, 2, 3) = 55;
    auto v = m.slice(1, 2); // padding: (1, 2, all) → contiguous row at [1][2]
    EXPECT_EQ(v(3), 55);
}

// ─── slice — non-prefix (strided) ────────────────────────────────────────────

TEST(slice_strided, middle_dim_fixed) {
    // matrix<int, 3, 4, 5>: slice(all, 2, all) → strided view of "plane" at dim1=2
    ysc::matrix<int, 3, 4, 5> m{};
    m(0, 2, 0) = 1;
    m(1, 2, 3) = 2;
    m(2, 2, 4) = 3;
    auto v = m.slice(ysc::all, 2, ysc::all); // matrix_view<int, strided, 3, 5>
    EXPECT_EQ(v(0, 0), 1);
    EXPECT_EQ(v(1, 3), 2);
    EXPECT_EQ(v(2, 4), 3);
}

TEST(slice_strided, last_dim_fixed) {
    ysc::matrix<int, 3, 4, 5> m{};
    m(1, 2, 3) = 77;
    auto v = m.slice(ysc::all, ysc::all, 3); // strided, 3×4
    EXPECT_EQ(v(1, 2), 77);
}

TEST(slice_strided, mutation_reflected_in_matrix) {
    ysc::matrix<int, 3, 4> m{};
    auto v = m.slice(ysc::all, 2); // strided column 2
    v(1) = 99;
    EXPECT_EQ(m(1, 2), 99);
}

TEST(slice_strided, at_out_of_range) {
    ysc::matrix<int, 3, 4, 5> m{};
    auto v = m.slice(ysc::all, 2, ysc::all);
    EXPECT_THROW((void)v.at(99, 0), std::out_of_range);
    EXPECT_THROW((void)v.at(0, 99), std::out_of_range);
    EXPECT_EQ(v.at(0, 0), 0); // in-bounds: no throw
}

// ─── padding (implicit all) ───────────────────────────────────────────────────

TEST(slice_padding, slice_one_arg_3d_equals_slice_i_all_all) {
    ysc::matrix<int, 3, 4, 5> m{};
    m(2, 0, 0) = 7;
    m(2, 3, 4) = 8;
    auto v1 = m.slice(2);
    auto v2 = m.slice(2, ysc::all, ysc::all);
    EXPECT_EQ(v1(0, 0), v2(0, 0));
    EXPECT_EQ(v1(3, 4), v2(3, 4));
}

// ─── bounds checking ─────────────────────────────────────────────────────────

TEST(slice_bounds, out_of_range_throws) {
    ysc::matrix<int, 3, 4> m{};
    EXPECT_THROW((void)m.slice(99), std::out_of_range);
    EXPECT_THROW((void)m.slice(ysc::all, 99), std::out_of_range);
    EXPECT_NO_THROW((void)m.slice(2));
}

TEST(slice_bounds, const_matrix_out_of_range) {
    const ysc::matrix<int, 3, 4> m{};
    EXPECT_THROW((void)m.slice(99), std::out_of_range);
    EXPECT_NO_THROW((void)m.slice(2));
}

// ─── const-correctness ────────────────────────────────────────────────────────

TEST(slice_const, const_matrix_returns_const_view) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    const auto& cm = m;
    auto v = cm.slice(1);
    // v is matrix_view<const int, contiguous, 4> — mutation must fail to compile:
    // v(0) = 99;   // would be a compile error
    EXPECT_EQ(v(0), 5); // read works
    static_assert(std::is_const_v<std::remove_reference_t<decltype(v(0))>>);
}

TEST(slice_const, const_strided_view_read) {
    ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    const auto& cm = m;
    auto v = cm.slice(ysc::all, 1); // strided column 1 — const view
    EXPECT_EQ(v(0), 2);             // m(0,1)
    EXPECT_EQ(v(1), 6);             // m(1,1)
    EXPECT_EQ(v(2), 10);            // m(2,1)
}

// ─── implicit conversion contiguous → strided ─────────────────────────────────

TEST(slice_conversion, contiguous_to_strided_implicit) {
    ysc::matrix<int, 5> m{10, 20, 30, 40, 50};
    auto cv = m.slice(); // contiguous view
    // Pass to function that accepts strided view — implicit conversion
    // Assign to local first: template args confuse the EXPECT_EQ macro's comma parsing
    int const total = sum_strided<int, 5>(cv);
    EXPECT_EQ(total, 150);
}

TEST(slice_conversion, strided_view_mutation_via_converted_view) {
    ysc::matrix<int, 3> m{1, 2, 3};
    ysc::matrix_view<int, ysc::strided, 3> sv = m.slice();
    sv(0) = 77;
    EXPECT_EQ(m(0), 77);
}

// ─── row() / col() ────────────────────────────────────────────────────────────

TEST(slice_row, row_reads_correctly) {
    ysc::matrix<int, 3, 4> m{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    auto r = m.row(2);
    EXPECT_EQ(r(0), 8);
    EXPECT_EQ(r(1), 9);
    EXPECT_EQ(r(2), 10);
    EXPECT_EQ(r(3), 11);
}

TEST(slice_row, row_mutation_reflected) {
    ysc::matrix<int, 3, 4> m{};
    m.row(1)(2) = 42;
    EXPECT_EQ(m(1, 2), 42);
}

TEST(slice_row, row_out_of_range) {
    ysc::matrix<int, 3, 4> m{};
    EXPECT_THROW((void)m.row(3), std::out_of_range);
    EXPECT_NO_THROW((void)m.row(2));
}

TEST(slice_row, const_row_read) {
    const ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto r = m.row(0);
    EXPECT_EQ(r(0), 1);
    EXPECT_EQ(r(3), 4);
}

TEST(slice_col, col_reads_correctly) {
    ysc::matrix<int, 3, 4> m{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
    auto c = m.col(1); // column 1: m(0,1)=1, m(1,1)=5, m(2,1)=9
    EXPECT_EQ(c(0), 1);
    EXPECT_EQ(c(1), 5);
    EXPECT_EQ(c(2), 9);
}

TEST(slice_col, col_mutation_reflected) {
    ysc::matrix<int, 3, 4> m{};
    m.col(2)(1) = 99;
    EXPECT_EQ(m(1, 2), 99);
}

TEST(slice_col, col_out_of_range) {
    ysc::matrix<int, 3, 4> m{};
    EXPECT_THROW((void)m.col(4), std::out_of_range);
    EXPECT_NO_THROW((void)m.col(3));
}

TEST(slice_col, const_col_read) {
    const ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto c = m.col(0); // column 0: 1, 5, 9
    EXPECT_EQ(c(0), 1);
    EXPECT_EQ(c(1), 5);
    EXPECT_EQ(c(2), 9);
}

// row/col restricted to order==2 — verified at compile time.
// Uses void_t SFINAE instead of !requires{} to work around a GCC 15 bug where constraint
// failures inside requires-expressions produce hard errors for constrained function templates.
namespace {
template <class M, class = void> struct has_row_m : std::false_type {};
template <class M>
struct has_row_m<M, std::void_t<decltype(std::declval<M&>().row(std::size_t{}))>> : std::true_type {
};

template <class M, class = void> struct has_col_m : std::false_type {};
template <class M>
struct has_col_m<M, std::void_t<decltype(std::declval<M&>().col(std::size_t{}))>> : std::true_type {
};
} // namespace

static_assert(!has_row_m<ysc::matrix<int, 3, 4, 5>>::value);
static_assert(!has_col_m<ysc::matrix<int, 5>>::value);
