#include <matrix.hpp>
#include <gtest/gtest.h>
#include "utils.hpp"

// Expect matrix elements to be zero-initialized for trivial types
TEST(construct, default_trivial_type)
{
    /*
     * Since reading uninitialized memory is UB, we can not rely solely on
     * reading the value of a matrix<unsigned> element after default-init.
     */
    std::array<unsigned char, sizeof(unsigned)> non_zero_memory;
    constexpr unsigned char non_zero = 0xC5;
    non_zero_memory.fill(non_zero);

    auto const& m = *new(non_zero_memory.data()) ysc::matrix<unsigned, 1>();
    ASSERT_EQ(m(0), 0);
}

// Expect matrix elements to be default-initialized for user-defined types
TEST(construct, default_user_defined_type)
{
    struct user_defined : ysc::test::SideEffect<>
    { user_defined() { trigger(); } };
    ysc::matrix<user_defined, 1> const m;
    ASSERT_TRUE(m(0).triggered());
}

// Expect matrix elements to be default-initialized for user-defined types
TEST(construct, value_user_defined_type)
{
    struct user_defined : ysc::test::SideEffect<>
    { user_defined() { trigger(); } };
    ysc::matrix<user_defined, 1> const m;
    ASSERT_TRUE(m(0).triggered());
}
