#include "matrix.hpp"
#include <algorithm>
#include <concepts>
#include <gtest/gtest.h>
#include <iterator>
#include <ranges>

using M = ysc::matrix<int, 2, 3>;

static_assert(std::contiguous_iterator<M::iterator>);
static_assert(std::contiguous_iterator<M::const_iterator>);

TEST(iterators, begin_end_mutable_range_for) {
    M m{1, 2, 3, 4, 5, 6};
    int sum = 0;
    for (auto& v : m) {
        sum += v;
    }
    EXPECT_EQ(sum, 21);
}

TEST(iterators, begin_end_const_range_for) {
    const M m{1, 2, 3, 4, 5, 6};
    int sum = 0;
    for (const auto& v : m) {
        sum += v;
    }
    EXPECT_EQ(sum, 21);
}

TEST(iterators, cbegin_cend) {
    M m{10, 20, 30, 40, 50, 60};
    const int sum = std::accumulate(m.cbegin(), m.cend(), 0);
    EXPECT_EQ(sum, 210);
}

TEST(iterators, rbegin_rend_mutable) {
    M m{1, 2, 3, 4, 5, 6};
    auto it = m.rbegin();
    EXPECT_EQ(*it, 6);
    ++it;
    EXPECT_EQ(*it, 5);
    ++it;
    EXPECT_EQ(*it, 4);
    ++it;
    EXPECT_EQ(*it, 3);
    ++it;
    EXPECT_EQ(*it, 2);
    ++it;
    EXPECT_EQ(*it, 1);
    ++it;
    EXPECT_EQ(it, m.rend());
}

TEST(iterators, rbegin_rend_const) {
    const M m{1, 2, 3, 4, 5, 6};
    auto it = m.rbegin();
    EXPECT_EQ(*it, 6);
    ++it;
    EXPECT_EQ(*it, 5);
    EXPECT_EQ(std::distance(m.rbegin(), m.rend()), 6);
}

TEST(iterators, crbegin_crend) {
    M m{1, 2, 3, 4, 5, 6};
    auto it = m.crbegin();
    EXPECT_EQ(*it, 6);
    EXPECT_EQ(std::distance(m.crbegin(), m.crend()), 6);
}

TEST(iterators, begin_end_write_through_iterator) {
    M m{1, 2, 3, 4, 5, 6};
    for (auto& v : m) {
        v *= 2;
    }
    const std::array<int, 6> expected{2, 4, 6, 8, 10, 12};
    EXPECT_TRUE(std::ranges::equal(m, expected));
}

TEST(iterators, ranges_sort) {
    M m{6, 3, 1, 4, 5, 2};
    std::ranges::sort(m);
    EXPECT_TRUE(std::ranges::is_sorted(m));
    EXPECT_EQ(*m.begin(), 1);
    EXPECT_EQ(*std::prev(m.end()), 6);
}
