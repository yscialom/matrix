#include <matrix.hpp>
#include <gtest/gtest.h>

TEST(DUMMY, DUMMYCASE)
{
    ysc::matrix<unsigned, 1> m;
    ASSERT_EQ(m(0), 0);
}
    

int main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
