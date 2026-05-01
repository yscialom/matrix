#include <matrix.hpp>

#include <gtest/gtest.h>


// Verify copy-assign from different type returns *this
TEST(assign_returns_self, copy_different_type)
{
    ysc::matrix<int, 3> m1 = {1, 2, 3};
    ysc::matrix<long, 3> m2;
    auto& ref = (m2 = m1);
    ASSERT_EQ(&ref, &m2);
}

// Verify move-assign from different type returns *this
TEST(assign_returns_self, move_different_type)
{
    ysc::matrix<int, 3> m1 = {1, 2, 3};
    ysc::matrix<long, 3> m2;
    auto& ref = (m2 = std::move(m1));
    ASSERT_EQ(&ref, &m2);
}

// Verify copy-assign is chainable
TEST(assign_returns_self, chain_copy)
{
    ysc::matrix<int, 3> src = {1, 2, 3};
    ysc::matrix<long, 3> a, b;
    b = a = src;
    ASSERT_EQ(a(0), 1);
    ASSERT_EQ(b(0), 1);
}
