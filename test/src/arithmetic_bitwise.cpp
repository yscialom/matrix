#include <matrix.hpp>

#include <cstdint>

#include <gtest/gtest.h>

//
// --- BITWISE AND ---
//

TEST(arithmetic_bitwise_and, compound_and_correct_values) {
    ysc::matrix<unsigned, 3> a{0b111U, 0b101U, 0b010U};
    const ysc::matrix<unsigned, 3> b{0b110U, 0b110U, 0b110U};
    a &= b;
    ASSERT_EQ(a, (ysc::matrix<unsigned, 3>{0b110U, 0b100U, 0b010U}));
}

TEST(arithmetic_bitwise_and, compound_and_returns_self) {
    ysc::matrix<unsigned, 2> a{0b11U, 0b10U};
    const ysc::matrix<unsigned, 2> b{0b10U, 0b10U};
    ysc::matrix<unsigned, 2>& ref = (a &= b);
    ASSERT_EQ(&ref, &a);
}

TEST(arithmetic_bitwise_and, compound_and_does_not_modify_rhs) {
    ysc::matrix<unsigned, 2> a{0b11U, 0b10U};
    const ysc::matrix<unsigned, 2> b{0b10U, 0b01U};
    const ysc::matrix<unsigned, 2> b_copy = b;
    a &= b;
    ASSERT_EQ(b, b_copy);
}

TEST(arithmetic_bitwise_and, binary_and_correct_values) {
    const ysc::matrix<unsigned, 3> a{0b111U, 0b101U, 0b010U};
    const ysc::matrix<unsigned, 3> b{0b110U, 0b110U, 0b110U};
    const auto r = a & b;
    ASSERT_EQ(r, (ysc::matrix<unsigned, 3>{0b110U, 0b100U, 0b010U}));
}

TEST(arithmetic_bitwise_and, binary_and_does_not_modify_operands) {
    const ysc::matrix<unsigned, 2> a{0b11U, 0b10U};
    const ysc::matrix<unsigned, 2> b{0b01U, 0b11U};
    const auto r = a & b;
    ASSERT_EQ(r, (ysc::matrix<unsigned, 2>{0b01U, 0b10U}));
    ASSERT_EQ(a, (ysc::matrix<unsigned, 2>{0b11U, 0b10U}));
    ASSERT_EQ(b, (ysc::matrix<unsigned, 2>{0b01U, 0b11U}));
}

TEST(arithmetic_bitwise_and, binary_and_mixed_types) {
    const ysc::matrix<uint8_t, 3> a{0xF0U, 0x0FU, 0xFFU};
    const ysc::matrix<uint16_t, 3> b{0x00FFU, 0x00FFU, 0x00FFU};
    const auto r = a & b;
    // uint8_t & uint16_t -> int (or wider integral via usual arithmetic conversions)
    ASSERT_EQ(r(0), 0xF0U & 0x00FFU);
    ASSERT_EQ(r(1), 0x0FU & 0x00FFU);
    ASSERT_EQ(r(2), 0xFFU & 0x00FFU);
}

TEST(arithmetic_bitwise_and, binary_and_2d_matrix) {
    const ysc::matrix<unsigned, 2, 2> a{0b1010U, 0b1100U, 0b0011U, 0b0101U};
    const ysc::matrix<unsigned, 2, 2> b{0b1111U, 0b1111U, 0b1111U, 0b1111U};
    const auto r = a & b;
    ASSERT_EQ(r, a);
}

//
// --- BITWISE OR ---
//

TEST(arithmetic_bitwise_or, compound_or_correct_values) {
    ysc::matrix<unsigned, 3> a{0b001U, 0b010U, 0b100U};
    const ysc::matrix<unsigned, 3> b{0b110U, 0b101U, 0b011U};
    a |= b;
    ASSERT_EQ(a, (ysc::matrix<unsigned, 3>{0b111U, 0b111U, 0b111U}));
}

TEST(arithmetic_bitwise_or, compound_or_returns_self) {
    ysc::matrix<unsigned, 2> a{0b00U, 0b01U};
    const ysc::matrix<unsigned, 2> b{0b10U, 0b10U};
    ysc::matrix<unsigned, 2>& ref = (a |= b);
    ASSERT_EQ(&ref, &a);
}

TEST(arithmetic_bitwise_or, compound_or_does_not_modify_rhs) {
    ysc::matrix<unsigned, 2> a{0b00U, 0b01U};
    const ysc::matrix<unsigned, 2> b{0b10U, 0b10U};
    const ysc::matrix<unsigned, 2> b_copy = b;
    a |= b;
    ASSERT_EQ(b, b_copy);
}

TEST(arithmetic_bitwise_or, binary_or_correct_values) {
    const ysc::matrix<unsigned, 3> a{0b001U, 0b010U, 0b100U};
    const ysc::matrix<unsigned, 3> b{0b110U, 0b101U, 0b011U};
    const auto r = a | b;
    ASSERT_EQ(r, (ysc::matrix<unsigned, 3>{0b111U, 0b111U, 0b111U}));
}

TEST(arithmetic_bitwise_or, binary_or_does_not_modify_operands) {
    const ysc::matrix<unsigned, 2> a{0b01U, 0b10U};
    const ysc::matrix<unsigned, 2> b{0b10U, 0b01U};
    const auto r = a | b;
    ASSERT_EQ(r, (ysc::matrix<unsigned, 2>{0b11U, 0b11U}));
    ASSERT_EQ(a, (ysc::matrix<unsigned, 2>{0b01U, 0b10U}));
    ASSERT_EQ(b, (ysc::matrix<unsigned, 2>{0b10U, 0b01U}));
}

TEST(arithmetic_bitwise_or, binary_or_mixed_types) {
    const ysc::matrix<uint8_t, 2> a{0x00U, 0xF0U};
    const ysc::matrix<uint16_t, 2> b{0x000FU, 0x000FU};
    const auto r = a | b;
    ASSERT_EQ(r(0), 0x00U | 0x000FU);
    ASSERT_EQ(r(1), 0xF0U | 0x000FU);
}

//
// --- BITWISE XOR ---
//

TEST(arithmetic_bitwise_xor, compound_xor_correct_values) {
    ysc::matrix<unsigned, 3> a{0b111U, 0b101U, 0b010U};
    const ysc::matrix<unsigned, 3> b{0b110U, 0b110U, 0b110U};
    a ^= b;
    ASSERT_EQ(a, (ysc::matrix<unsigned, 3>{0b001U, 0b011U, 0b100U}));
}

TEST(arithmetic_bitwise_xor, compound_xor_returns_self) {
    ysc::matrix<unsigned, 2> a{0b11U, 0b00U};
    const ysc::matrix<unsigned, 2> b{0b01U, 0b10U};
    ysc::matrix<unsigned, 2>& ref = (a ^= b);
    ASSERT_EQ(&ref, &a);
}

TEST(arithmetic_bitwise_xor, compound_xor_does_not_modify_rhs) {
    ysc::matrix<unsigned, 2> a{0b11U, 0b00U};
    const ysc::matrix<unsigned, 2> b{0b01U, 0b10U};
    const ysc::matrix<unsigned, 2> b_copy = b;
    a ^= b;
    ASSERT_EQ(b, b_copy);
}

TEST(arithmetic_bitwise_xor, binary_xor_correct_values) {
    const ysc::matrix<unsigned, 3> a{0b111U, 0b101U, 0b010U};
    const ysc::matrix<unsigned, 3> b{0b110U, 0b110U, 0b110U};
    const auto r = a ^ b;
    ASSERT_EQ(r, (ysc::matrix<unsigned, 3>{0b001U, 0b011U, 0b100U}));
}

TEST(arithmetic_bitwise_xor, binary_xor_does_not_modify_operands) {
    const ysc::matrix<unsigned, 2> a{0b11U, 0b01U};
    const ysc::matrix<unsigned, 2> b{0b10U, 0b10U};
    const auto r = a ^ b;
    ASSERT_EQ(r, (ysc::matrix<unsigned, 2>{0b01U, 0b11U}));
    ASSERT_EQ(a, (ysc::matrix<unsigned, 2>{0b11U, 0b01U}));
    ASSERT_EQ(b, (ysc::matrix<unsigned, 2>{0b10U, 0b10U}));
}

TEST(arithmetic_bitwise_xor, xor_self_is_zero) {
    const ysc::matrix<unsigned, 3> a{1U, 2U, 3U};
    const auto r = a ^ a;
    ASSERT_EQ(r, (ysc::matrix<unsigned, 3>{0U, 0U, 0U}));
}

//
// --- BITWISE NOT ---
//

TEST(arithmetic_bitwise_not, bitwise_not_correct_values) {
    const ysc::matrix<unsigned, 3> m{0U, 0xFFFFFFFFU, 0x0F0F0F0FU};
    const auto r = ~m;
    ASSERT_EQ(r(0), ~0U);
    ASSERT_EQ(r(1), ~0xFFFFFFFFU);
    ASSERT_EQ(r(2), ~0x0F0F0F0FU);
}

TEST(arithmetic_bitwise_not, bitwise_not_does_not_modify_operand) {
    const ysc::matrix<unsigned, 2> m{0U, 0xFFU};
    const ysc::matrix<unsigned, 2> m_copy = m;
    const auto r = ~m;
    (void)r;
    ASSERT_EQ(m, m_copy);
}

TEST(arithmetic_bitwise_not, double_not_is_identity) {
    const ysc::matrix<unsigned, 3> m{1U, 2U, 3U};
    ASSERT_EQ(~~m, m);
}

//
// --- SHIFTS ---
//

TEST(arithmetic_bitwise_shift, left_shift_correct_values) {
    ysc::matrix<unsigned, 3> m{1U, 2U, 4U};
    m <<= 1U;
    ASSERT_EQ(m, (ysc::matrix<unsigned, 3>{2U, 4U, 8U}));
}

TEST(arithmetic_bitwise_shift, left_shift_returns_self) {
    ysc::matrix<unsigned, 2> m{1U, 2U};
    ysc::matrix<unsigned, 2>& ref = (m <<= 1U);
    ASSERT_EQ(&ref, &m);
}

TEST(arithmetic_bitwise_shift, right_shift_correct_values) {
    ysc::matrix<unsigned, 3> m{8U, 4U, 2U};
    m >>= 1U;
    ASSERT_EQ(m, (ysc::matrix<unsigned, 3>{4U, 2U, 1U}));
}

TEST(arithmetic_bitwise_shift, right_shift_returns_self) {
    ysc::matrix<unsigned, 2> m{8U, 4U};
    ysc::matrix<unsigned, 2>& ref = (m >>= 1U);
    ASSERT_EQ(&ref, &m);
}

TEST(arithmetic_bitwise_shift, shift_2d_matrix) {
    ysc::matrix<unsigned, 2, 2> m{1U, 2U, 4U, 8U};
    m <<= 2U;
    ASSERT_EQ(m, (ysc::matrix<unsigned, 2, 2>{4U, 8U, 16U, 32U}));
}

//
// --- COMPILE-TIME CONSTRAINTS ---
//

struct no_bitwise {
    unsigned val;
    bool operator==(const no_bitwise&) const = default;
};

// Helper concepts — GCC does not support bare requires-expressions in static_assert
// when all candidates are constrained out (same workaround as in concepts.cpp).
template <class M>
concept matrix_and_assignable = requires(M& a, const M& b) { a &= b; };
template <class M>
concept matrix_or_assignable = requires(M& a, const M& b) { a |= b; };
template <class M>
concept matrix_xor_assignable = requires(M& a, const M& b) { a ^= b; };
template <class M>
concept matrix_bitnot_available = requires(const M& a) { ~a; };

static_assert(!matrix_and_assignable<ysc::matrix<no_bitwise, 2>>);
static_assert(!matrix_or_assignable<ysc::matrix<no_bitwise, 2>>);
static_assert(!matrix_xor_assignable<ysc::matrix<no_bitwise, 2>>);
static_assert(!matrix_bitnot_available<ysc::matrix<no_bitwise, 2>>);
static_assert(!matrix_and_assignable<ysc::matrix<double, 2>>);

static_assert(matrix_and_assignable<ysc::matrix<unsigned, 2>>);
static_assert(matrix_or_assignable<ysc::matrix<unsigned, 2>>);
static_assert(matrix_xor_assignable<ysc::matrix<unsigned, 2>>);
static_assert(matrix_bitnot_available<ysc::matrix<unsigned, 2>>);
static_assert(matrix_and_assignable<ysc::matrix<int, 2>>);

TEST(arithmetic_bitwise_constraints, concept_constraints_checked_at_compile_time) {
    // Runtime witness that the static_asserts above were checked.
    SUCCEED();
}
