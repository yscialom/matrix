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
    auto const m = ysc::test::on_non_zero_memory<ysc::matrix<unsigned, 1>>(ysc::zero);
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

// Expect matrix elements to be aggregate-initializable (trival types, multi-dim)
TEST(construct_init, aggregate_trivial_types_multidim)
{
    ysc::matrix<int, 2, 2, 2> const m = { 1, 2, 3, 4, 5, 6, 7, 8 };
    ASSERT_EQ(m(0,0,0), 1);
    ASSERT_EQ(m(0,0,1), 2);
    ASSERT_EQ(m(1,1,1), 8);
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


//
// --- ASSIGNMENT OPERATORS ---
//

// Expect matrixes to be assignable for trivial types
TEST(assign_copy, trivial_type)
{
    ysc::matrix<int, 3> const m = { 1987, 04, 24 };
    ysc::matrix<int, 3> m_assign;
    m_assign = m;
    ASSERT_EQ(m_assign(0), 1987);
    ASSERT_EQ(m_assign(1), 04);
    ASSERT_EQ(m_assign(2), 24);
}

// Expect matrixes to be assignable for trivial types from a different source type
TEST(assign_copy, different_trivial_type)
{
    ysc::matrix<int, 3> const m = { 1987, 04, 24 };
    ysc::matrix<long, 3> m_assign;
    m_assign = m;
    ASSERT_EQ(m_assign(0), 1987);
    ASSERT_EQ(m_assign(1), 04);
    ASSERT_EQ(m_assign(2), 24);
}

// Expect matrixes to be assignable for user-defined types
TEST(assign_copy, user_defined_type)
{
    ysc::matrix<std::string, 3> const m = { "1987", "04", "24" };
    ysc::matrix<std::string, 3> m_assign;
    m_assign = m;
    ASSERT_EQ(m_assign(0), "1987");
    ASSERT_EQ(m_assign(1), "04");
    ASSERT_EQ(m_assign(2), "24");
}

// Expect matrixes to be assignable (move) for trivial types
TEST(assign_move, trivial_type)
{
    ysc::matrix<int, 3> m = { 1987, 04, 24 };
    ysc::matrix<int, 3> m_assign;
    m_assign = std::move(m);
    ASSERT_EQ(m_assign(0), 1987);
    ASSERT_EQ(m_assign(1), 04);
    ASSERT_EQ(m_assign(2), 24);
}

// Expect matrixes to be assignable (move) for trivial types from a different source type
TEST(assign_move, different_trivial_type)
{
    ysc::matrix<int, 3> m = { 1987, 04, 24 };
    ysc::matrix<long, 3> m_assign;
    m_assign = std::move(m);
    ASSERT_EQ(m_assign(0), 1987);
    ASSERT_EQ(m_assign(1), 04);
    ASSERT_EQ(m_assign(2), 24);
}

// Expect matrixes to be assignable (move) for user-defined types
TEST(assign_move, user_defined_type)
{
    ysc::matrix<std::string, 3> m = { "1987", "04", "24" };
    ysc::matrix<std::string, 3> m_assign;
    m_assign = std::move(m);
    ASSERT_EQ(m_assign(0), "1987");
    ASSERT_EQ(m_assign(1), "04");
    ASSERT_EQ(m_assign(2), "24");
}


//
// --- DESTRUCTOR ---
//

// Expect matrix elements to be destructed
TEST(destruct, user_defined_type)
{
    bool trigger = false;
    struct S {
        S(bool& flag) : _flag(flag) {}
        ~S() { _flag = true; }         // upon destruction: sets trigger to true
        bool& _flag;
    };

    {
        // trigger is false
        ysc::matrix<S, 1> const m = { trigger };
    }   // trigger should be toggled as m is destructed
    ASSERT_TRUE(trigger);
}
