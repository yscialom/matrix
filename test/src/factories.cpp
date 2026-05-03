#include <matrix.hpp>

#include <gtest/gtest.h>

//
// --- zeros ---
//

TEST(factories_zeros, all_elements_zero) {
    auto m = ysc::zeros<int, 2, 3>();
    for (std::size_t i = 0; i < 2; ++i)
        for (std::size_t j = 0; j < 3; ++j)
            ASSERT_EQ(m(i, j), 0);
}

TEST(factories_zeros, constexpr_eval) {
    static_assert(ysc::zeros<int, 2, 3>()(0, 0) == 0);
}

TEST(factories_zeros, noexcept_spec) {
    static_assert(noexcept(ysc::zeros<int, 2, 3>()));
}

//
// --- full ---
//

TEST(factories_full, all_elements_equal_value) {
    auto m = ysc::full<int, 2, 3>(42);
    for (std::size_t i = 0; i < 2; ++i)
        for (std::size_t j = 0; j < 3; ++j)
            ASSERT_EQ(m(i, j), 42);
}

TEST(factories_full, one_dimension) {
    auto m = ysc::full<double, 4>(3.14);
    for (std::size_t i = 0; i < 4; ++i)
        ASSERT_DOUBLE_EQ(m(i), 3.14);
}

//
// --- ones ---
//

TEST(factories_ones, all_elements_one) {
    auto m = ysc::ones<int, 2, 3>();
    for (std::size_t i = 0; i < 2; ++i)
        for (std::size_t j = 0; j < 3; ++j)
            ASSERT_EQ(m(i, j), 1);
}

//
// --- identity ---
//

TEST(factories_identity, diagonal_one_off_zero) {
    auto m = ysc::identity<int, 3>();
    for (std::size_t i = 0; i < 3; ++i)
        for (std::size_t j = 0; j < 3; ++j)
            ASSERT_EQ(m(i, j), i == j ? 1 : 0);
}

TEST(factories_identity, size_one) {
    auto m = ysc::identity<int, 1>();
    ASSERT_EQ(m(0, 0), 1);
}

TEST(factories_identity, double_type) {
    auto m = ysc::identity<double, 2>();
    ASSERT_DOUBLE_EQ(m(0, 0), 1.0);
    ASSERT_DOUBLE_EQ(m(0, 1), 0.0);
    ASSERT_DOUBLE_EQ(m(1, 0), 0.0);
    ASSERT_DOUBLE_EQ(m(1, 1), 1.0);
}
