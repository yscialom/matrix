#include <matrix.hpp>
#include <gtest/gtest.h>
#include "utils.hpp"

// Expect matrix elements to be zero-initialized for trivial types
TEST(construct, default_trivial_type)
{
    ysc::matrix<unsigned, 1> m;
    ASSERT_EQ(m(0), 0);
}

// Expect matrix elements to be default-initialized for user-defined types
TEST(construct, default_user_defined_type)
{
    struct user_defined : ysc::test::SideEffect<>
    { user_defined() { trigger(); } };
    ysc::matrix<user_defined, 1> m;
    ASSERT_TRUE(m(0).triggered());
}


int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
