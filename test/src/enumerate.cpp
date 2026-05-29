#include "matrix.hpp"
#include <gtest/gtest.h>
#include <vector>

// ─── constexpr static checks ─────────────────────────────────────────────────

static_assert([] {
    ysc::matrix<int, 3> m{10, 20, 30};
    std::size_t idx = 0;
    for (auto [coords, val] : m.enumerate()) {
        if (coords[0] != idx) {
            return false;
        }
        ++idx;
    }
    return idx == 3;
}());

static_assert([] {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    std::size_t n = 0;
    for (auto [coords, val] : m.enumerate()) {
        (void)coords;
        (void)val;
        ++n;
    }
    return n == 6;
}());

// ─── coordinates correctness ─────────────────────────────────────────────────

TEST(MatrixEnumerate, CoordinatesCorrect1D) {
    ysc::matrix<int, 4> m{10, 20, 30, 40};
    std::size_t expected = 0;
    for (auto [coords, val] : m.enumerate()) {
        EXPECT_EQ(coords.size(), 1U);
        EXPECT_EQ(coords[0], expected);
        ++expected;
    }
    EXPECT_EQ(expected, 4U);
}

TEST(MatrixEnumerate, CoordinatesCorrect2D) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    const std::vector<std::array<std::size_t, 2>> expected_coords = {{0, 0}, {0, 1}, {0, 2},
                                                                     {1, 0}, {1, 1}, {1, 2}};
    std::size_t idx = 0;
    for (auto [coords, val] : m.enumerate()) {
        ASSERT_LT(idx, expected_coords.size());
        EXPECT_EQ(coords, expected_coords[idx]);
        ++idx;
    }
    EXPECT_EQ(idx, 6U);
}

TEST(MatrixEnumerate, CoordinatesCorrect3D) {
    ysc::matrix<int, 2, 3, 4> m{ysc::zero};
    std::size_t linear = 0;
    for (auto [coords, val] : m.enumerate()) {
        EXPECT_EQ(coords.size(), 3U);
        // coords[0] varies slowest, coords[2] fastest
        EXPECT_EQ(coords[0], linear / 12);
        EXPECT_EQ(coords[1], (linear % 12) / 4);
        EXPECT_EQ(coords[2], linear % 4);
        ++linear;
    }
    EXPECT_EQ(linear, 24U);
}

// ─── value access ─────────────────────────────────────────────────────────────

TEST(MatrixEnumerate, ValueMatchesDirectAccess1D) {
    ysc::matrix<int, 5> m{10, 20, 30, 40, 50};
    for (auto [coords, val] : m.enumerate()) {
        EXPECT_EQ(val, m(coords[0]));
    }
}

TEST(MatrixEnumerate, ValueMatchesDirectAccess2D) {
    ysc::matrix<int, 3, 4> m{ysc::zero};
    int v = 1;
    for (std::size_t i = 0; i < 3; ++i) {
        for (std::size_t j = 0; j < 4; ++j) {
            m(i, j) = v++;
        }
    }
    for (auto [coords, val] : m.enumerate()) {
        EXPECT_EQ(val, m(coords[0], coords[1]));
    }
}

// ─── mutation ─────────────────────────────────────────────────────────────────

TEST(MatrixEnumerate, MutationReflectedInMatrix) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    for (auto [coords, val] : m.enumerate()) {
        (void)coords;
        val *= 10;
    }
    EXPECT_EQ(m, (ysc::matrix<int, 2, 3>{10, 20, 30, 40, 50, 60}));
}

TEST(MatrixEnumerate, MutationUsingCoordinates) {
    ysc::matrix<int, 3, 3> m{ysc::zero};
    for (auto [coords, val] : m.enumerate()) {
        // set diagonal to 1
        if (coords[0] == coords[1]) {
            val = 1;
        }
    }
    for (std::size_t i = 0; i < 3; ++i) {
        for (std::size_t j = 0; j < 3; ++j) {
            EXPECT_EQ(m(i, j), (i == j ? 1 : 0));
        }
    }
}

// ─── const overload ───────────────────────────────────────────────────────────

TEST(MatrixEnumerate, ConstOverloadCompiles) {
    const ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    std::size_t count = 0;
    for (const auto& [coords, val] : m.enumerate()) {
        static_assert(std::is_same_v<const int&, decltype(val)>);
        EXPECT_EQ(val, m(coords[0], coords[1]));
        ++count;
    }
    EXPECT_EQ(count, 6U);
}

TEST(MatrixEnumerate, ConstValueMatchesDirectAccess) {
    const ysc::matrix<int, 4> m{7, 8, 9, 10};
    for (const auto& [coords, val] : m.enumerate()) {
        EXPECT_EQ(val, m(coords[0]));
    }
}

// ─── row-major order ──────────────────────────────────────────────────────────

TEST(MatrixEnumerate, RowMajorOrderRespected2D) {
    ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    std::vector<int> visited;
    for (auto [coords, val] : m.enumerate()) {
        (void)coords;
        visited.push_back(val);
    }
    EXPECT_EQ(visited, (std::vector<int>{1, 2, 3, 4, 5, 6}));
}

// ─── empty matrix ─────────────────────────────────────────────────────────────

TEST(MatrixEnumerate, EmptyMatrixYieldsNoIterations) {
    ysc::matrix<int, 0> m;
    std::size_t count = 0;
    for (auto [coords, val] : m.enumerate()) {
        (void)coords;
        (void)val;
        ++count;
    }
    EXPECT_EQ(count, 0U);
}
