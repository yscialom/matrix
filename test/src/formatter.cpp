#include <matrix.hpp>

#include <gtest/gtest.h>

#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
#include <format>
#include <sstream>

TEST(formatter, format_1d) {
    // Acceptance criterion from US-025: std::format("{}", matrix<int,2>{1,2}) → "[1, 2]"
    ASSERT_EQ(std::format("{}", ysc::matrix<int, 2>{1, 2}), "[1, 2]");
}

TEST(formatter, format_2d) {
    ASSERT_EQ(std::format("{}", ysc::matrix<int, 2, 2>{1, 2, 3, 4}), "[[1, 2], [3, 4]]");
}

TEST(formatter, format_3d) {
    ysc::matrix<int, 2, 2, 2> m{1, 2, 3, 4, 5, 6, 7, 8};
    ASSERT_EQ(std::format("{}", m), "[[[1, 2], [3, 4]], [[5, 6], [7, 8]]]");
}

TEST(formatter, format_double) {
    ysc::matrix<double, 2> m{1.5, 2.5};
    ASSERT_EQ(std::format("{}", m), "[1.5, 2.5]");
}

TEST(formatter, matches_ostream) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    std::ostringstream oss;
    oss << m;
    ASSERT_EQ(std::format("{}", m), oss.str());
}

#else

TEST(formatter, not_available) {
    GTEST_SKIP() << "std::format not available on this compiler";
}

#endif // defined(__cpp_lib_format) && __cpp_lib_format >= 201907L
