#include <matrix.hpp>
#include <matrix_view.hpp>

#include <algorithm>
#include <concepts>
#include <gtest/gtest.h>
#include <iterator>
#include <numeric>
#include <ranges>

// ─── compile-time assertions ──────────────────────────────────────────────────

static_assert(sizeof(ysc::matrix_view<int, ysc::contiguous, 3, 3>) == sizeof(int*));
static_assert(sizeof(ysc::matrix_view<double, ysc::contiguous, 5>) == sizeof(double*));

static_assert(std::contiguous_iterator<ysc::matrix_view<int, ysc::contiguous, 3>::iterator>);
static_assert(std::contiguous_iterator<ysc::matrix_view<int, ysc::contiguous, 3>::const_iterator>);

static_assert(ysc::matrix_view<int, ysc::contiguous, 2, 3>::order == 2);
static_assert(ysc::matrix_view<int, ysc::contiguous, 2, 3>::size() == 6);
static_assert(!ysc::matrix_view<int, ysc::contiguous, 2, 3>::empty());

static_assert(!std::is_default_constructible_v<ysc::matrix_view<int, ysc::contiguous, 3>>);
static_assert(
    std::is_constructible_v<ysc::matrix_view<int, ysc::contiguous, 3>, ysc::matrix<int, 3>&>);
// Non-const matrix_view cannot be constructed from a const matrix
static_assert(!std::is_constructible_v<ysc::matrix_view<int, ysc::contiguous, 3>,
                                       const ysc::matrix<int, 3>&>);

// ─── construction ─────────────────────────────────────────────────────────────

TEST(matrix_view_construct, from_matrix_implicit) {
    ysc::matrix<int, 3> m{1, 2, 3};
    ysc::matrix_view<int, ysc::contiguous, 3> v = m;
    EXPECT_EQ(v(0), m(0));
    EXPECT_EQ(v(1), m(1));
    EXPECT_EQ(v(2), m(2));
}

TEST(matrix_view_construct, copy_shares_pointer) {
    ysc::matrix<int, 3> m{10, 20, 30};
    ysc::matrix_view<int, ysc::contiguous, 3> v1 = m;
    // NOLINTNEXTLINE(performance-unnecessary-copy-initialization)
    ysc::matrix_view<int, ysc::contiguous, 3> v2 = v1;
    v2(0) = 99;
    EXPECT_EQ(m(0), 99);
}

TEST(matrix_view_construct, from_raw_pointer) {
    std::array<int, 6> buf{1, 2, 3, 4, 5, 6};
    ysc::matrix_view<int, ysc::contiguous, 2, 3> v{buf.data()};
    EXPECT_EQ(v(0, 0), 1);
    EXPECT_EQ(v(1, 2), 6);
}

// ─── operator() ───────────────────────────────────────────────────────────────

TEST(matrix_view_access, read_within_bounds) {
    ysc::matrix<int, 2, 2> m{1, 2, 3, 4};
    const ysc::matrix_view<int, ysc::contiguous, 2, 2> v = m;
    EXPECT_EQ(v(0, 0), 1);
    EXPECT_EQ(v(0, 1), 2);
    EXPECT_EQ(v(1, 0), 3);
    EXPECT_EQ(v(1, 1), 4);
}

TEST(matrix_view_access, write_reflects_in_matrix) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
    v(0, 0) = 99;
    v(1, 2) = 42;
    EXPECT_EQ(m(0, 0), 99);
    EXPECT_EQ(m(1, 2), 42);
}

TEST(matrix_view_access, write_in_matrix_visible_via_view) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
    m(1, 0) = 42;
    EXPECT_EQ(v(1, 0), 42);
}

// ─── at() ─────────────────────────────────────────────────────────────────────

TEST(matrix_view_at, nominal_read) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    const ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
    EXPECT_EQ(v.at(1, 2), 6);
}

TEST(matrix_view_at, nominal_write) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
    v.at(0, 1) = 77;
    EXPECT_EQ(m(0, 1), 77);
}

TEST(matrix_view_at, out_of_range_negative) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    const ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
    EXPECT_THROW((void)v.at(-1, 0), std::out_of_range);
}

TEST(matrix_view_at, out_of_range_too_large) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    const ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
    EXPECT_THROW((void)v.at(2, 0), std::out_of_range);
}

TEST(matrix_view_at, out_of_range_negative_mutable) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
    EXPECT_THROW((void)v.at(-1, 0), std::out_of_range);
}

TEST(matrix_view_at, out_of_range_too_large_mutable) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
    EXPECT_THROW((void)v.at(2, 0), std::out_of_range);
}

TEST(matrix_view_at, const_nominal_read) {
    ysc::matrix<int, 3> m{10, 20, 30};
    const ysc::matrix_view<int, ysc::contiguous, 3> v = m;
    EXPECT_EQ(v.at(2), 30);
}

// ─── iterators ────────────────────────────────────────────────────────────────

TEST(matrix_view_iterators, range_for_read) {
    ysc::matrix<int, 4> m{1, 2, 3, 4};
    const ysc::matrix_view<int, ysc::contiguous, 4> v = m;
    int sum = 0;
    for (const auto& x : v) {
        sum += x;
    }
    EXPECT_EQ(sum, 10);
}

TEST(matrix_view_iterators, range_for_write_reflects_in_matrix) {
    ysc::matrix<int, 3> m{1, 2, 3};
    ysc::matrix_view<int, ysc::contiguous, 3> v = m;
    for (auto& x : v) {
        x *= 2;
    }
    EXPECT_EQ(m(0), 2);
    EXPECT_EQ(m(1), 4);
    EXPECT_EQ(m(2), 6);
}

TEST(matrix_view_iterators, begin_equals_data) {
    ysc::matrix<int, 3> m{1, 2, 3};
    ysc::matrix_view<int, ysc::contiguous, 3> v = m;
    EXPECT_EQ(v.begin(), v.data());
}

TEST(matrix_view_iterators, end_minus_begin_equals_size) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    const ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
    EXPECT_EQ(std::distance(v.begin(), v.end()), static_cast<std::ptrdiff_t>(v.size()));
}

TEST(matrix_view_iterators, cbegin_cend_accumulate) {
    ysc::matrix<int, 4> m{10, 20, 30, 40};
    const ysc::matrix_view<int, ysc::contiguous, 4> v = m;
    const int total = std::accumulate(v.cbegin(), v.cend(), 0);
    EXPECT_EQ(total, 100);
}

TEST(matrix_view_iterators, rbegin_rend) {
    ysc::matrix<int, 3> m{1, 2, 3};
    const ysc::matrix_view<int, ysc::contiguous, 3> v = m;
    std::vector<int> rev(v.rbegin(), v.rend());
    EXPECT_EQ(rev[0], 3);
    EXPECT_EQ(rev[2], 1);
}

TEST(matrix_view_iterators, ranges_sort_through_view) {
    ysc::matrix<int, 5> m{5, 3, 1, 4, 2};
    ysc::matrix_view<int, ysc::contiguous, 5> v = m;
    std::ranges::sort(v);
    EXPECT_TRUE(std::ranges::is_sorted(m));
}

// ─── front() / back() ────────────────────────────────────────────────────────

TEST(matrix_view_accessors, front_read) {
    ysc::matrix<int, 3> m{10, 20, 30};
    const ysc::matrix_view<int, ysc::contiguous, 3> v = m;
    EXPECT_EQ(v.front(), 10);
}

TEST(matrix_view_accessors, front_write_reflects) {
    ysc::matrix<int, 3> m{10, 20, 30};
    ysc::matrix_view<int, ysc::contiguous, 3> v = m;
    v.front() = 99;
    EXPECT_EQ(m(0), 99);
}

TEST(matrix_view_accessors, back_read) {
    ysc::matrix<int, 3> m{10, 20, 30};
    const ysc::matrix_view<int, ysc::contiguous, 3> v = m;
    EXPECT_EQ(v.back(), 30);
}

TEST(matrix_view_accessors, back_write_reflects) {
    ysc::matrix<int, 3> m{10, 20, 30};
    ysc::matrix_view<int, ysc::contiguous, 3> v = m;
    v.back() = 99;
    EXPECT_EQ(m(2), 99);
}

// ─── capacity ────────────────────────────────────────────────────────────────

TEST(matrix_view_capacity, size_equals_linear_size) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    const ysc::matrix_view<int, ysc::contiguous, 2, 3> v = m;
    EXPECT_EQ(v.size(), 6U);
}

TEST(matrix_view_capacity, data_points_to_matrix_data) {
    ysc::matrix<int, 3> m{1, 2, 3};
    ysc::matrix_view<int, ysc::contiguous, 3> v = m;
    EXPECT_EQ(v.data(), m.data());
}

TEST(matrix_view_capacity, data_write_through) {
    ysc::matrix<int, 3> m{1, 2, 3};
    ysc::matrix_view<int, ysc::contiguous, 3> v = m;
    v.data()[1] = 42;
    EXPECT_EQ(m(1), 42);
}

// ─── fill() ──────────────────────────────────────────────────────────────────

TEST(matrix_view_fill, fill_modifies_matrix) {
    ysc::matrix<int, 2, 2> m{1, 2, 3, 4};
    ysc::matrix_view<int, ysc::contiguous, 2, 2> v = m;
    v.fill(0);
    EXPECT_EQ(m(0, 0), 0);
    EXPECT_EQ(m(0, 1), 0);
    EXPECT_EQ(m(1, 0), 0);
    EXPECT_EQ(m(1, 1), 0);
}

// ─── 1D regression ───────────────────────────────────────────────────────────

TEST(matrix_view_1d, single_coord_access) {
    ysc::matrix<int, 5> m{10, 20, 30, 40, 50};
    ysc::matrix_view<int, ysc::contiguous, 5> v = m;
    EXPECT_EQ(v(0), 10);
    EXPECT_EQ(v(2), 30);
    EXPECT_EQ(v(4), 50);
}
