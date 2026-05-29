#include "matrix.hpp"
#include <gtest/gtest.h>

// ─── matmul reference values ─────────────────────────────────────────────────
// All values independently verified by hand.

// Pair 1: 2×2 square matrices
// [[1,2],[3,4]] × [[5,6],[7,8]] = [[1*5+2*7, 1*6+2*8],[3*5+4*7, 3*6+4*8]]
//                               = [[19,22],[43,50]]
TEST(LinalgReference, MatmulSquare2x2) {
    constexpr ysc::matrix<int, 2, 2> a{1, 2, 3, 4};
    constexpr ysc::matrix<int, 2, 2> b{5, 6, 7, 8};
    constexpr auto c = ysc::matmul(a, b);
    static_assert(c(0, 0) == 19);
    static_assert(c(0, 1) == 22);
    static_assert(c(1, 0) == 43);
    static_assert(c(1, 1) == 50);
    EXPECT_EQ(c(0, 0), 19);
    EXPECT_EQ(c(0, 1), 22);
    EXPECT_EQ(c(1, 0), 43);
    EXPECT_EQ(c(1, 1), 50);
}

// Pair 2: 2×3 × 3×2
// [[1,2,3],[4,5,6]] × [[7,8],[9,10],[11,12]]
// row 0: [1*7+2*9+3*11, 1*8+2*10+3*12] = [58, 64]
// row 1: [4*7+5*9+6*11, 4*8+5*10+6*12] = [139, 154]
TEST(LinalgReference, MatmulRect2x3Times3x2) {
    constexpr ysc::matrix<int, 2, 3> a{1, 2, 3, 4, 5, 6};
    constexpr ysc::matrix<int, 3, 2> b{7, 8, 9, 10, 11, 12};
    constexpr auto c = ysc::matmul(a, b);
    static_assert(c(0, 0) == 58);
    static_assert(c(0, 1) == 64);
    static_assert(c(1, 0) == 139);
    static_assert(c(1, 1) == 154);
    EXPECT_EQ(c(0, 0), 58);
    EXPECT_EQ(c(0, 1), 64);
    EXPECT_EQ(c(1, 0), 139);
    EXPECT_EQ(c(1, 1), 154);
}

// Pair 3: identity × arbitrary matrix
// I₂ × [[5,6],[7,8]] = [[5,6],[7,8]]
TEST(LinalgReference, MatmulIdentityTimesMatrix) {
    constexpr ysc::matrix<int, 2, 2> m{5, 6, 7, 8};
    constexpr auto result = ysc::matmul(ysc::identity<int, 2>(), m);
    static_assert(result(0, 0) == 5);
    static_assert(result(0, 1) == 6);
    static_assert(result(1, 0) == 7);
    static_assert(result(1, 1) == 8);
    EXPECT_EQ(result, m);
}

// Pair 4: 3×3 rotation-like matrix product
// [[2,0,0],[0,3,0],[0,0,4]] × [[1,1,1],[1,1,1],[1,1,1]]
// row 0: [2,2,2], row 1: [3,3,3], row 2: [4,4,4]
TEST(LinalgReference, MatmulDiagonalTimesOnes) {
    constexpr ysc::matrix<int, 3, 3> diag{2, 0, 0, 0, 3, 0, 0, 0, 4};
    constexpr ysc::matrix<int, 3, 3> ones{1, 1, 1, 1, 1, 1, 1, 1, 1};
    constexpr auto c = ysc::matmul(diag, ones);
    static_assert(c(0, 0) == 2);
    static_assert(c(0, 1) == 2);
    static_assert(c(0, 2) == 2);
    static_assert(c(1, 0) == 3);
    static_assert(c(1, 1) == 3);
    static_assert(c(1, 2) == 3);
    static_assert(c(2, 0) == 4);
    static_assert(c(2, 1) == 4);
    static_assert(c(2, 2) == 4);
    EXPECT_EQ(c(0, 0), 2);
    EXPECT_EQ(c(1, 1), 3);
    EXPECT_EQ(c(2, 2), 4);
}

// ─── dot product reference values ────────────────────────────────────────────

// Pair 1: [1,2,3] · [4,5,6] = 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
TEST(LinalgReference, DotBasicIntegers) {
    constexpr ysc::matrix<int, 3> a{1, 2, 3};
    constexpr ysc::matrix<int, 3> b{4, 5, 6};
    constexpr int result = ysc::dot(a, b);
    static_assert(result == 32);
    EXPECT_EQ(result, 32);
}

// Pair 2: orthogonal unit vectors → 0
// [1,0,0] · [0,1,0] = 0
TEST(LinalgReference, DotOrthogonalVectors) {
    constexpr ysc::matrix<int, 3> a{1, 0, 0};
    constexpr ysc::matrix<int, 3> b{0, 1, 0};
    constexpr int result = ysc::dot(a, b);
    static_assert(result == 0);
    EXPECT_EQ(result, 0);
}

// Pair 3: [2,4,6] · [1,2,3] = 2 + 8 + 18 = 28
TEST(LinalgReference, DotScaledVector) {
    constexpr ysc::matrix<int, 3> a{2, 4, 6};
    constexpr ysc::matrix<int, 3> b{1, 2, 3};
    constexpr int result = ysc::dot(a, b);
    static_assert(result == 28);
    EXPECT_EQ(result, 28);
}

// Pair 4: [1,0,0,0] · [0,0,0,1] = 0  (perpendicular in 4D)
TEST(LinalgReference, DotPerpendicular4D) {
    constexpr ysc::matrix<int, 4> a{1, 0, 0, 0};
    constexpr ysc::matrix<int, 4> b{0, 0, 0, 1};
    constexpr int result = ysc::dot(a, b);
    static_assert(result == 0);
    EXPECT_EQ(result, 0);
}

// Pair 5: [3,4] · [3,4] = 9 + 16 = 25  (squared norm of a (3,4,5) vector)
TEST(LinalgReference, DotSquaredNorm) {
    constexpr ysc::matrix<int, 2> v{3, 4};
    constexpr int result = ysc::dot(v, v);
    static_assert(result == 25);
    EXPECT_EQ(result, 25);
}

// ─── transpose reference values ──────────────────────────────────────────────

// Matrix 1: 2×3 → verify all 6 elements and double-transpose
TEST(LinalgReference, TransposeThenRetranspose2x3) {
    constexpr ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    constexpr auto tt = ysc::transpose(ysc::transpose(m));
    static_assert(tt(0, 0) == 1);
    static_assert(tt(0, 1) == 2);
    static_assert(tt(0, 2) == 3);
    static_assert(tt(1, 0) == 4);
    static_assert(tt(1, 1) == 5);
    static_assert(tt(1, 2) == 6);
    EXPECT_EQ(tt, m);
}

// Matrix 2: 3×4 → double-transpose roundtrip
// [[1,2,3,4],[5,6,7,8],[9,10,11,12]]
TEST(LinalgReference, TransposeThenRetranspose3x4) {
    constexpr ysc::matrix<int, 3, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
    constexpr auto tt = ysc::transpose(ysc::transpose(m));
    static_assert(tt(0, 0) == 1);
    static_assert(tt(2, 3) == 12);
    EXPECT_EQ(tt, m);
}

// Matrix 3: 2×3 transposed values verified individually
// m = [[10,20,30],[40,50,60]]
// t = [[10,40],[20,50],[30,60]]
TEST(LinalgReference, TransposeValuesVerified2x3) {
    constexpr ysc::matrix<int, 2, 3> m{10, 20, 30, 40, 50, 60};
    constexpr auto t = ysc::transpose(m);
    static_assert(t(0, 0) == 10);
    static_assert(t(0, 1) == 40);
    static_assert(t(1, 0) == 20);
    static_assert(t(1, 1) == 50);
    static_assert(t(2, 0) == 30);
    static_assert(t(2, 1) == 60);
    EXPECT_EQ(t(0, 0), 10);
    EXPECT_EQ(t(0, 1), 40);
    EXPECT_EQ(t(1, 0), 20);
    EXPECT_EQ(t(1, 1), 50);
    EXPECT_EQ(t(2, 0), 30);
    EXPECT_EQ(t(2, 1), 60);
}

// Matrix 4: 4×4 symmetry check via double-transpose
// [[1,2,3,4],[5,6,7,8],[9,10,11,12],[13,14,15,16]]
TEST(LinalgReference, TransposeThenRetranspose4x4) {
    constexpr ysc::matrix<int, 4, 4> m{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    constexpr auto tt = ysc::transpose(ysc::transpose(m));
    static_assert(tt(0, 0) == 1);
    static_assert(tt(3, 3) == 16);
    EXPECT_EQ(tt, m);
}
