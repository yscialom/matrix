#include <matrix.hpp>

#include <gtest/gtest.h>

#include <type_traits>


//
// --- TRIVIAL TYPE ---
//

TEST(fill, trivial_type_all_elements_set)
{
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    m.fill(42);
    for (auto v : m) {
        ASSERT_EQ(v, 42);
    }
}

TEST(fill, trivial_type_overwrite_zeros)
{
    ysc::matrix<int, 4> m{0, 0, 0, 0};
    m.fill(-1);
    for (auto v : m) {
        ASSERT_EQ(v, -1);
    }
}

TEST(fill, trivial_type_fill_zero)
{
    ysc::matrix<double, 3, 3> m{1, 2, 3, 4, 5, 6, 7, 8, 9};
    m.fill(0.0);
    for (auto v : m) {
        ASSERT_DOUBLE_EQ(v, 0.0);
    }
}


//
// --- USER-DEFINED TYPE ---
//

struct Point {
    int x, y;
    bool operator==(const Point&) const = default;
};

TEST(fill, user_defined_type)
{
    ysc::matrix<Point, 3> m{Point{1, 2}, Point{3, 4}, Point{5, 6}};
    m.fill(Point{0, 0});
    for (auto v : m) {
        ASSERT_EQ(v, (Point{0, 0}));
    }
}


//
// --- MATRIX OF MATRIX ---
//

TEST(fill, matrix_of_matrix)
{
    using Inner = ysc::matrix<int, 2>;
    ysc::matrix<Inner, 3> m;
    const Inner pattern{7, 7};
    m.fill(pattern);
    for (const auto& v : m) {
        ASSERT_EQ(v, pattern);
    }
}


//
// --- NOEXCEPT ---
//

TEST(fill, noexcept_for_nothrow_copy_assignable)
{
    static_assert(std::is_nothrow_copy_assignable_v<int>);
    ysc::matrix<int, 3> m;
    static_assert(noexcept(m.fill(0)));
}
