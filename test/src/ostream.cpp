#include <matrix.hpp>

#include <gtest/gtest.h>

#include <sstream>

TEST(ostream, print_1d) {
    ysc::matrix<int, 3> m{1, 2, 3};
    std::ostringstream oss;
    oss << m;
    ASSERT_EQ(oss.str(), "[1, 2, 3]");
}

TEST(ostream, print_2d) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    std::ostringstream oss;
    oss << m;
    ASSERT_EQ(oss.str(), "[[1, 2, 3], [4, 5, 6]]");
}

TEST(ostream, print_2d_spec) {
    // Acceptance criterion from US-024: [[1, 2], [3, 4]]
    std::ostringstream oss;
    oss << ysc::matrix<int, 2, 2>{1, 2, 3, 4};
    ASSERT_EQ(oss.str(), "[[1, 2], [3, 4]]");
}

TEST(ostream, print_3d) {
    ysc::matrix<int, 2, 2, 2> m{1, 2, 3, 4, 5, 6, 7, 8};
    std::ostringstream oss;
    oss << m;
    ASSERT_EQ(oss.str(), "[[[1, 2], [3, 4]], [[5, 6], [7, 8]]]");
}

TEST(ostream, print_double) {
    ysc::matrix<double, 2> m{1.5, 2.5};
    std::ostringstream oss;
    oss << m;
    ASSERT_EQ(oss.str(), "[1.5, 2.5]");
}

TEST(ostream, chaining) {
    ysc::matrix<int, 2> m{7, 8};
    std::ostringstream oss;
    oss << m << "!";
    ASSERT_EQ(oss.str(), "[7, 8]!");
}
