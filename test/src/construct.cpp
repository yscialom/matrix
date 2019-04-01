#include <matrix.hpp>
#include "utils.hpp"

#include <gtest/gtest.h>

#include <string>
#include <memory>


//
// --- DEFAULT CONSTRUCTORS ---
//

// Expect matrix elements to be default-initialized (uninitialized) for trivial types
TEST(construct_default, default_trivial_type)
{
    /*
     * Since reading uninitialized memory is UB, we can not rely solely on
     * reading the value of a matrix<unsigned> element after default-init.
     */
    auto const m = ysc::test::on_non_zero_memory<ysc::matrix<unsigned, 1>>();
    ASSERT_NE((*m)(0), 0);
}

// Expect matrix elements to be default-initialized for user-defined types
TEST(construct_default, default_user_defined_type)
{
    struct user_defined : ysc::test::SideEffect<>
    { user_defined() { trigger(); } };
    ysc::matrix<user_defined, 1> const m;
    ASSERT_TRUE(m(0).triggered());
}

// Expect matrix elements to be default-initialized for array types
TEST(construct_default, default_array_type)
{
    struct user_defined : ysc::test::SideEffect<>
    { user_defined() { trigger(); } };
    ysc::matrix<user_defined[1], 1> const m;
    ASSERT_TRUE(m(0)[0].triggered());
}


//
// --- AGGREGATE CONSTRUCTORS ---
//

// Expect matrix elements to be zero-initialized for trivial types on demand
TEST(construct_init, zero_trivial_type)
{
    /*
     * Since reading uninitialized memory is UB, we can not rely solely on
     * reading the value of a matrix<unsigned> element after default-init.
     */
    auto const m = ysc::test::on_non_zero_memory<ysc::matrix<unsigned, 1>>(ysc::matrix_zero);
    ASSERT_EQ((*m)(0), 0);
}

// Expect matrix elements to be aggregate-initializable (trival types)
TEST(construct_init, aggregate_trivial_types)
{
    ysc::matrix<int, 3> const m = { 1987, 04, 24 };
    ASSERT_EQ(m(0), 1987);
    ASSERT_EQ(m(1), 04);
    ASSERT_EQ(m(2), 24);
}

// Expect matrix elements to be aggregate-initializable (user-defined types)
TEST(construct_init, aggregate_user_defined_types)
{
    ysc::matrix<std::string, 3> const m = { "1987", "04", "24" };
    ASSERT_EQ(m(0), "1987");
    ASSERT_EQ(m(1), "04");
    ASSERT_EQ(m(2), "24");
}

// Expect matrix elements to be aggregate-initializable from mixed source types
TEST(construct_init, aggregate_mixed_types)
{
    ysc::matrix<long long, 3> const m = { 1987, '\x04', 24L };
    ASSERT_EQ(m(0), 1987);
    ASSERT_EQ(m(1), 04);
    ASSERT_EQ(m(2), 24);
}


//
// --- COPY CONSTRUCTORS ---
//

// Expect matrixes to be copyable for trivial types
TEST(construct_copy, trivial_type)
{
    ysc::matrix<int, 3> const m = { 1987, 04, 24 };
    auto const m_copy = m;
    ASSERT_EQ(m_copy(0), 1987);
    ASSERT_EQ(m_copy(1), 04);
    ASSERT_EQ(m_copy(2), 24);
}

// Expect matrixes to be copyable for trivial types from a different source type
TEST(construct_copy, different_trivial_type)
{
    ysc::matrix<int, 3> const m = { 1987, 04, 24 };
    ysc::matrix<long, 3> const m_copy = m;
    ASSERT_EQ(m_copy(0), 1987);
    ASSERT_EQ(m_copy(1), 04);
    ASSERT_EQ(m_copy(2), 24);
}

// Expect matrixes to be copyable for user-defined types
TEST(construct_copy, user_defined_type)
{
    ysc::matrix<std::string, 3> const m = { "1987", "04", "24" };
    auto const m_copy = m;
    ASSERT_EQ(m_copy(0), "1987");
    ASSERT_EQ(m_copy(1), "04");
    ASSERT_EQ(m_copy(2), "24");
}

//
// --- MOVE CONSTRUCTORS ---
//

// Expect matrixes to be movable for trivial types
TEST(construct_move, trivial_type)
{
    ysc::matrix<int, 3> m = { 1987, 04, 24 };
    auto const m_copy = std::move(m);
    ASSERT_EQ(m_copy(0), 1987);
    ASSERT_EQ(m_copy(1), 04);
    ASSERT_EQ(m_copy(2), 24);
}

// Expect matrixes to be movable for trivial types from a different source type
TEST(construct_move, different_trivial_type)
{
    ysc::matrix<int, 3> m = { 1987, 04, 24 };
    ysc::matrix<long, 3> const m_copy = std::move(m);
    ASSERT_EQ(m_copy(0), 1987);
    ASSERT_EQ(m_copy(1), 04);
    ASSERT_EQ(m_copy(2), 24);
}

// Expect matrixes to be movable for user-defined types
TEST(construct_move, user_defined_type)
{
    ysc::matrix<std::shared_ptr<int>, 1> m = { std::make_shared<int>(0) };
    auto const m_copy = std::move(m);
    ASSERT_EQ(*m_copy(0), 0);
    ASSERT_FALSE(static_cast<bool>(m(0)));
}
