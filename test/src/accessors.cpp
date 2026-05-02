#include <matrix.hpp>

#include <gtest/gtest.h>

//
// --- front() ---
//

TEST(accessors_front, returns_first_element) {
    ysc::matrix<int, 3> m{10, 20, 30};
    ASSERT_EQ(m.front(), 10);
}

TEST(accessors_front, const_returns_first_element) {
    const ysc::matrix<int, 3> m{10, 20, 30};
    ASSERT_EQ(m.front(), 10);
}

TEST(accessors_front, reference_allows_mutation) {
    ysc::matrix<int, 3> m{10, 20, 30};
    m.front() = 99;
    ASSERT_EQ(m(0), 99);
}

TEST(accessors_front, noexcept_guaranteed) {
    ysc::matrix<int, 3> m{1, 2, 3};
    static_assert(noexcept(m.front()));
    const ysc::matrix<int, 3> cm{1, 2, 3};
    static_assert(noexcept(cm.front()));
}

//
// --- back() ---
//

TEST(accessors_back, returns_last_element) {
    ysc::matrix<int, 3> m{10, 20, 30};
    ASSERT_EQ(m.back(), 30);
}

TEST(accessors_back, const_returns_last_element) {
    const ysc::matrix<int, 3> m{10, 20, 30};
    ASSERT_EQ(m.back(), 30);
}

TEST(accessors_back, reference_allows_mutation) {
    ysc::matrix<int, 3> m{10, 20, 30};
    m.back() = 99;
    ASSERT_EQ(m(2), 99);
}

TEST(accessors_back, noexcept_guaranteed) {
    ysc::matrix<int, 3> m{1, 2, 3};
    static_assert(noexcept(m.back()));
    const ysc::matrix<int, 3> cm{1, 2, 3};
    static_assert(noexcept(cm.back()));
}

TEST(accessors_back, 2d_last_element_is_row_major_last) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    ASSERT_EQ(m.back(), 6);
}

//
// --- swap() membre ---
//

TEST(accessors_swap_member, exchanges_contents) {
    ysc::matrix<int, 3> a{1, 2, 3};
    ysc::matrix<int, 3> b{4, 5, 6};
    a.swap(b);
    ASSERT_EQ(a(0), 4);
    ASSERT_EQ(a(1), 5);
    ASSERT_EQ(a(2), 6);
    ASSERT_EQ(b(0), 1);
    ASSERT_EQ(b(1), 2);
    ASSERT_EQ(b(2), 3);
}

TEST(accessors_swap_member, self_swap_is_stable) {
    ysc::matrix<int, 3> m{1, 2, 3};
    m.swap(m);
    ASSERT_EQ(m(0), 1);
    ASSERT_EQ(m(1), 2);
    ASSERT_EQ(m(2), 3);
}

TEST(accessors_swap_member, noexcept_for_nothrow_swappable) {
    static_assert(std::is_nothrow_swappable_v<int>);
    ysc::matrix<int, 3> a;
    ysc::matrix<int, 3> b;
    static_assert(noexcept(a.swap(b)));
}

//
// --- swap() friend (ADL) ---
//

TEST(accessors_swap_free, exchanges_contents) {
    ysc::matrix<int, 2, 2> a{1, 2, 3, 4};
    ysc::matrix<int, 2, 2> b{5, 6, 7, 8};
    using std::swap;
    swap(a, b);
    ASSERT_EQ(a(0, 0), 5);
    ASSERT_EQ(b(0, 0), 1);
}
