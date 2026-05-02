#include "matrix.hpp"
#include <gtest/gtest.h>

using M23 = ysc::matrix<int, 2, 3>;
using M1 = ysc::matrix<int, 7>;

// compile-time guarantees
static_assert(M23::size() == 6);
static_assert(M23::max_size() == 6);
static_assert(M23::empty() == false);
static_assert(M1::size() == 7);

TEST(size, equals_product_of_dimensions) {
    EXPECT_EQ(M23::size(), 6u);
}

TEST(size, single_dimension) {
    EXPECT_EQ(M1::size(), 7u);
}

TEST(max_size, equals_size) {
    EXPECT_EQ(M23::max_size(), M23::size());
}

TEST(empty, non_empty_matrix_is_false) {
    EXPECT_FALSE(M23::empty());
}

TEST(data, mutable_points_to_first_element) {
    M23 m{1, 2, 3, 4, 5, 6};
    EXPECT_EQ(m.data(), &m(0, 0));
    EXPECT_EQ(*m.data(), 1);
}

TEST(data, const_points_to_first_element) {
    const M23 m{10, 20, 30, 40, 50, 60};
    EXPECT_EQ(m.data(), &m(0, 0));
    EXPECT_EQ(*m.data(), 10);
}

TEST(data, mutable_write_through) {
    M23 m{1, 2, 3, 4, 5, 6};
    *m.data() = 99;
    EXPECT_EQ(m(0, 0), 99);
}

TEST(data, contiguous_storage) {
    M23 m{1, 2, 3, 4, 5, 6};
    for (ysc::matrix<int, 2, 3>::size_type i = 0; i < M23::size(); ++i)
        EXPECT_EQ(*(m.data() + i), static_cast<int>(i + 1));
}
