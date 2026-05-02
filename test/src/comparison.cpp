#include <matrix.hpp>

#include <gtest/gtest.h>

#include <compare>


//
// --- EQUALITY ---
//

TEST(comparison, equal_same_values)
{
    ysc::matrix<int, 2, 3> m1{1, 2, 3, 4, 5, 6};
    ysc::matrix<int, 2, 3> m2{1, 2, 3, 4, 5, 6};
    ASSERT_TRUE(m1 == m2);
    ASSERT_FALSE(m1 != m2);
}

TEST(comparison, equal_different_values)
{
    ysc::matrix<int, 2, 3> m1{1, 2, 3, 4, 5, 6};
    ysc::matrix<int, 2, 3> m2{1, 2, 3, 4, 5, 7};
    ASSERT_FALSE(m1 == m2);
    ASSERT_TRUE(m1 != m2);
}

TEST(comparison, equal_self)
{
    ysc::matrix<int, 3> m{10, 20, 30};
    ASSERT_TRUE(m == m);
    ASSERT_FALSE(m != m);
}


//
// --- THREE-WAY (lexicographic) ---
//

TEST(comparison, less_than)
{
    ysc::matrix<int, 3> m1{1, 2, 3};
    ysc::matrix<int, 3> m2{1, 2, 4};
    ASSERT_TRUE(m1 < m2);
    ASSERT_TRUE(m1 <= m2);
    ASSERT_FALSE(m1 > m2);
    ASSERT_FALSE(m1 >= m2);
}

TEST(comparison, greater_than)
{
    ysc::matrix<int, 3> m1{1, 3, 0};
    ysc::matrix<int, 3> m2{1, 2, 9};
    ASSERT_TRUE(m1 > m2);
    ASSERT_TRUE(m1 >= m2);
    ASSERT_FALSE(m1 < m2);
    ASSERT_FALSE(m1 <= m2);
}

TEST(comparison, lexicographic_first_element_wins)
{
    ysc::matrix<int, 3> lo{0, 99, 99};
    ysc::matrix<int, 3> hi{1, 0, 0};
    ASSERT_TRUE(lo < hi);
}

TEST(comparison, ordering_equal)
{
    ysc::matrix<int, 2> m{5, 5};
    ASSERT_TRUE(m >= m);
    ASSERT_TRUE(m <= m);
}


//
// --- STATIC ASSERTIONS ---
//

TEST(comparison, three_way_comparable_concept)
{
    static_assert(std::three_way_comparable<ysc::matrix<int, 3>>);
    static_assert(std::three_way_comparable<ysc::matrix<double, 2, 2>>);
}
