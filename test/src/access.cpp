#include <matrix.hpp>
#include "utils.hpp"

#include <gtest/gtest.h>


//
// --- ELEMENT ACCESS ---
//

// Access element of a constant matrix, no bound checking (access within bounds)
TEST(access, const_no_check_nominal)
{
    ysc::matrix<int, 2, 2> const m{0, 1, 2, 3};
    ASSERT_EQ(m(0,0), 0);
    ASSERT_EQ(m(0,1), 1);
    ASSERT_EQ(m(1,0), 2);
    ASSERT_EQ(m(1,1), 3);
}

// Access element of a constant matrix, no bound checking (access out of bounds)
TEST(access, const_no_check_outofbound)
{
    ysc::matrix<int, 2, 2> const m{0, 1, 2, 3};
    try {
        (void) m(-1, -1);
    } catch (...) {
        ASSERT_TRUE(false);
    }
}

// Access element of a mutable matrix, no bound checking (access within bounds)
TEST(access, mutable_no_check_nominal)
{
    ysc::matrix<int, 2, 2> m{0, 1, 2, 3};

    m(0,0) = 10;
    m(0,1) = 11;
    m(1,0) = 12;
    m(1,1) = 13;

    ASSERT_EQ(m(0,0), 10);
    ASSERT_EQ(m(0,1), 11);
    ASSERT_EQ(m(1,0), 12);
    ASSERT_EQ(m(1,1), 13);
}

// Access element of a mutable matrix, no bound checking (access out of bounds)
TEST(access, mutable_no_check_outofbound)
{
    ysc::matrix<int, 2, 2> m{0, 1, 2, 3};
    try {
        (void) m(-1, -1);
    } catch (...) {
        ASSERT_TRUE(false);
    }
}

// Access element of a constant matrix, with bound checking (access within bounds)
TEST(access, const_with_check_nominal)
{
    ysc::matrix<int, 2, 2> const m{0, 1, 2, 3};
    ASSERT_EQ(m.at(0,0), 0);
    ASSERT_EQ(m.at(0,1), 1);
    ASSERT_EQ(m.at(1,0), 2);
    ASSERT_EQ(m.at(1,1), 3);
}

// Access element of a constant matrix, with bound checking (access out of bounds)
TEST(access, const_with_check_outofbound)
{
    ysc::matrix<int, 2, 2> const m{0, 1, 2, 3};

    {
        bool out_of_range_exception_catch = false;
        try {
            (void) m.at(-1, 0);
        } catch (std::out_of_range&) {
            out_of_range_exception_catch = true;
        }
        ASSERT_TRUE(out_of_range_exception_catch);
    }
    {
        bool out_of_range_exception_catch = false;
        try {
            (void) m.at(2, 0);
        } catch (std::out_of_range&) {
            out_of_range_exception_catch = true;
        }
        ASSERT_TRUE(out_of_range_exception_catch);
    }
}

// Access element of a mutable matrix, with bound checking (access within bounds)
TEST(access, mutable_with_check_nominal)
{
    ysc::matrix<int, 2, 2> m{0, 1, 2, 3};

    m.at(0,0) = 10;
    m.at(0,1) = 11;
    m.at(1,0) = 12;
    m.at(1,1) = 13;

    ASSERT_EQ(m.at(0,0), 10);
    ASSERT_EQ(m.at(0,1), 11);
    ASSERT_EQ(m.at(1,0), 12);
    ASSERT_EQ(m.at(1,1), 13);
}

// Access element of a mutable matrix, with bound checking (access out of bounds)
TEST(access, mutable_with_check_outofbound)
{
    ysc::matrix<int, 2, 2> m{0, 1, 2, 3};

    {
        bool out_of_range_exception_catch = false;
        try {
            (void) m.at(-1, 0);
        } catch (std::out_of_range&) {
            out_of_range_exception_catch = true;
        }
        ASSERT_TRUE(out_of_range_exception_catch);
    }
    {
        bool out_of_range_exception_catch = false;
        try {
            (void) m.at(0, 2);
        } catch (std::out_of_range&) {
            out_of_range_exception_catch = true;
        }
        ASSERT_TRUE(out_of_range_exception_catch);
    }
}

