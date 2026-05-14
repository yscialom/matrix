#include <matrix.hpp>
#include <matrix_view.hpp>

#include <gtest/gtest.h>
#include <type_traits>

// ─── compile-time assertions ──────────────────────────────────────────────────

static_assert(std::is_same_v<decltype(std::declval<ysc::matrix<int, 2, 3>&>().reshape<6>()),
                             ysc::matrix_view<int, ysc::contiguous, 6>>);
static_assert(std::is_same_v<decltype(std::declval<ysc::matrix<int, 2, 3>&>().reshape<1, 6>()),
                             ysc::matrix_view<int, ysc::contiguous, 1, 6>>);
static_assert(std::is_same_v<decltype(std::declval<const ysc::matrix<int, 2, 3>&>().reshape<6>()),
                             ysc::matrix_view<const int, ysc::contiguous, 6>>);
static_assert(std::is_same_v<decltype(std::declval<ysc::matrix<int, 2, 3>&>().flatten()),
                             ysc::matrix_view<int, ysc::contiguous, 6>>);
static_assert(std::is_same_v<decltype(std::declval<const ysc::matrix<int, 2, 3>&>().flatten()),
                             ysc::matrix_view<const int, ysc::contiguous, 6>>);

// ─── reshape ──────────────────────────────────────────────────────────────────

TEST(reshape, changes_dimension_interpretation) {
    ysc::matrix<int, 2, 6> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    auto v = m.reshape<3, 4>();
    EXPECT_EQ(v(1, 2), m(0, 6)); // flat index 6 = row1*4+col2 in 3x4 = row0*6+6 in 2x6
    EXPECT_EQ(v(0, 0), m(0, 0));
    EXPECT_EQ(v(2, 3), m(1, 5));
}

TEST(reshape, mutation_reflected_in_original) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto v = m.reshape<6>();
    v(3) = 99;
    EXPECT_EQ(m(1, 0), 99);
}

TEST(reshape, original_mutation_reflected_in_view) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto v = m.reshape<6>();
    m(0, 2) = 42;
    EXPECT_EQ(v(2), 42);
}

TEST(reshape, const_matrix_gives_const_view) {
    const ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto v = m.reshape<6>();
    static_assert(std::is_const_v<std::remove_reference_t<decltype(v(0))>>);
    EXPECT_EQ(v(0), 1);
    EXPECT_EQ(v(5), 6);
}

TEST(reshape, nd_to_nd) {
    ysc::matrix<int, 24> m{1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12,
                           13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24};
    auto v = m.reshape<2, 3, 4>();
    EXPECT_EQ(v(0, 0, 0), m(0));
    EXPECT_EQ(v(1, 2, 3), m(23));
    EXPECT_EQ(v(0, 1, 2), m(6));
}

// ─── flatten ─────────────────────────────────────────────────────────────────

TEST(flatten, basic) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto v = m.flatten();
    EXPECT_EQ(v(0), 1);
    EXPECT_EQ(v(3), 4);
    EXPECT_EQ(v(5), 6);
}

TEST(flatten, mutation_reflected_in_original) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto v = m.flatten();
    v(3) = 42;
    EXPECT_EQ(m(1, 0), 42);
}

TEST(flatten, original_mutation_reflected_in_view) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    auto v = m.flatten();
    m(1, 2) = 77;
    EXPECT_EQ(v(5), 77);
}

TEST(flatten, const_matrix_gives_const_view) {
    const ysc::matrix<int, 3> m{1, 2, 3};
    auto v = m.flatten();
    static_assert(std::is_const_v<std::remove_reference_t<decltype(v(0))>>);
    EXPECT_EQ(v(0), 1);
    EXPECT_EQ(v(2), 3);
}

TEST(flatten, on_1d_matrix_is_identity_view) {
    ysc::matrix<int, 6> m{10, 20, 30, 40, 50, 60};
    auto v = m.flatten();
    EXPECT_EQ(v(0), m(0));
    EXPECT_EQ(v(5), m(5));
}
