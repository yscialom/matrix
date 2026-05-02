#include "matrix.hpp"
#include <concepts>
#include <cstddef>
#include <gtest/gtest.h>
#include <iterator>

using M = ysc::matrix<int, 2, 3>;

static_assert(std::same_as<M::value_type, int>);
static_assert(std::same_as<M::size_type, std::size_t>);
static_assert(std::same_as<M::difference_type, std::ptrdiff_t>);
static_assert(std::same_as<M::reference, int&>);
static_assert(std::same_as<M::const_reference, const int&>);
static_assert(std::same_as<M::pointer, int*>);
static_assert(std::same_as<M::const_pointer, const int*>);
static_assert(std::same_as<M::reverse_iterator, std::reverse_iterator<M::iterator>>);
static_assert(std::same_as<M::const_reverse_iterator, std::reverse_iterator<M::const_iterator>>);

TEST(typedefs, value_type_is_int) {
    static_assert(std::same_as<ysc::matrix<int, 3>::value_type, int>);
    static_assert(std::same_as<ysc::matrix<double, 2, 2>::value_type, double>);
}

TEST(typedefs, size_type_and_difference_type) {
    static_assert(std::same_as<M::size_type, std::size_t>);
    static_assert(std::same_as<M::difference_type, std::ptrdiff_t>);
}

TEST(typedefs, reference_and_pointer) {
    static_assert(std::same_as<M::reference, int&>);
    static_assert(std::same_as<M::const_reference, const int&>);
    static_assert(std::same_as<M::pointer, int*>);
    static_assert(std::same_as<M::const_pointer, const int*>);
}

TEST(typedefs, iterator_types_are_public) {
    static_assert(std::same_as<M::iterator, std::array<int, 6>::iterator>);
    static_assert(std::same_as<M::const_iterator, std::array<int, 6>::const_iterator>);
    static_assert(std::same_as<M::reverse_iterator, std::reverse_iterator<M::iterator>>);
    static_assert(
        std::same_as<M::const_reverse_iterator, std::reverse_iterator<M::const_iterator>>);
}
