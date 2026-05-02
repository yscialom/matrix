#include <gtest/gtest.h>
#include "matrix.hpp"
#include <string>

// Helper concept to test at() callability (GCC 15 does not support bare requires
// expressions in static_assert when all overload candidates are constrained out)
template<class M, class... Args>
concept at_callable = requires(M& m, Args... args) { m.at(args...); };

// integral_coordinates
static_assert(ysc::integral_coordinates<int>);
static_assert(ysc::integral_coordinates<int, int>);
static_assert(ysc::integral_coordinates<unsigned int, std::size_t>);
static_assert(!ysc::integral_coordinates<double>);
static_assert(!ysc::integral_coordinates<int, double>);
static_assert(!ysc::integral_coordinates<float>);

// matrix_convertible_from
static_assert(ysc::matrix_convertible_from<int, int>);
static_assert(ysc::matrix_convertible_from<double, int>);
static_assert(ysc::matrix_convertible_from<int, double>);
static_assert(!ysc::matrix_convertible_from<int, std::string>);
static_assert(!ysc::matrix_convertible_from<std::string, int>);

// operator() accepts integral coordinates, rejects floating-point
static_assert( std::is_invocable_v<ysc::matrix<int, 2, 3>&, int, int>);
static_assert(!std::is_invocable_v<ysc::matrix<int, 2, 3>&, double, double>);
static_assert(!std::is_invocable_v<ysc::matrix<int, 2, 3>&, float, float>);

// at() accepts integral coordinates, rejects floating-point
static_assert( at_callable<ysc::matrix<int, 2, 3>, int, int>);
static_assert(!at_callable<ysc::matrix<int, 2, 3>, double, double>);
static_assert(!at_callable<ysc::matrix<int, 2, 3>, float, float>);

// Converting constructor is constrained (positive case)
static_assert(std::is_constructible_v<ysc::matrix<double, 3>, ysc::matrix<int, 3> const&>);

TEST(concepts, operator_call_integral_coords)
{
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    EXPECT_EQ(m(0, 0), 1);
    EXPECT_EQ(m(1, 2), 6);
}

TEST(concepts, converting_ctor_constrained)
{
    // const source: the more-specialized converting ctor wins over the variadic ctor
    const ysc::matrix<int, 3> mi{1, 2, 3};
    ysc::matrix<double, 3> md(mi);
    EXPECT_DOUBLE_EQ(md(0), 1.0);
    EXPECT_DOUBLE_EQ(md(1), 2.0);
    EXPECT_DOUBLE_EQ(md(2), 3.0);
}

TEST(concepts, converting_assign_constrained)
{
    const ysc::matrix<int, 3> mi{4, 5, 6};
    ysc::matrix<double, 3> md;
    md = mi;
    EXPECT_DOUBLE_EQ(md(0), 4.0);
    EXPECT_DOUBLE_EQ(md(1), 5.0);
    EXPECT_DOUBLE_EQ(md(2), 6.0);
}
