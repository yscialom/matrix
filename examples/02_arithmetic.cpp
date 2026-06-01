// 02_arithmetic.cpp — arithmetic and bitwise operators.
//
// Covers:
//   * Element-wise +, -, *, / and their compound forms (Hadamard product).
//   * Scalar arithmetic (+, -, *, /) with operator commutativity for *.
//   * Unary + and -, including the constexpr requirement on operator-() from
//     US-059 (compile-time evaluation via static_assert).
//   * Bitwise operators on integer matrices — &, |, ^ (binary), ~ (unary),
//     <<= / >>= (scalar shifts) — added by US-063.

#include <matrix.hpp>

#include <exception>
#include <iostream>

namespace {

void run() {
    using std::cout;

    // ─── Element-wise ────────────────────────────────────────────────────────
    cout << "=== Element-wise ===\n";
    const ysc::matrix<int, 2, 2> a{1, 2, 3, 4};
    const ysc::matrix<int, 2, 2> b{10, 20, 30, 40};
    cout << "a + b              = " << (a + b) << '\n';
    cout << "b - a              = " << (b - a) << '\n';
    cout << "a * b (Hadamard)   = " << (a * b) << '\n';
    cout << "b / a              = " << (b / a) << '\n';

    // ─── Scalar ──────────────────────────────────────────────────────────────
    cout << "\n=== Scalar ===\n";
    cout << "a * 10             = " << (a * 10) << '\n';
    cout << "10 * a             = " << (10 * a) << '\n'; // commutative
    cout << "a + 100            = " << (a + 100) << '\n';
    cout << "(a * 10) / 5       = " << ((a * 10) / 5) << '\n';

    // ─── Unary ───────────────────────────────────────────────────────────────
    cout << "\n=== Unary ===\n";
    cout << "+a                 = " << +a << '\n';
    cout << "-a                 = " << -a << '\n';
    cout << std::boolalpha << "-(-a) == a ?       = " << (-(-a) == a) << '\n';

    // operator-() is constexpr (US-059): negation can run at compile time.
    constexpr ysc::matrix<int, 3> v{1, -2, 3};
    constexpr auto neg = -v;
    static_assert(neg(0) == -1 && neg(1) == 2 && neg(2) == -3,
                  "operator-() must be constexpr (US-059)");
    cout << "constexpr -v       = " << neg << '\n';

    // ─── Bitwise (US-063) ────────────────────────────────────────────────────
    cout << "\n=== Bitwise on unsigned matrices (US-063) ===\n";

    const ysc::matrix<unsigned, 4> flags{0b1100U, 0b1010U, 0b0011U, 0b0101U};
    const ysc::matrix<unsigned, 4> mask{0b1111U, 0b0000U, 0b1010U, 0b1100U};

    cout << std::hex;
    cout << "flags & mask       = " << (flags & mask) << '\n';
    cout << "flags | mask       = " << (flags | mask) << '\n';
    cout << "flags ^ mask       = " << (flags ^ mask) << '\n';

    // Unary ~ inverts all bits of every element (the high bits stay zero
    // because we only used the low 8 bits of `unsigned`).
    const ysc::matrix<unsigned, 3> bytes{0x00U, 0xF0U, 0xFFU};
    cout << "~bytes & 0xFF      = " << ((~bytes) & ysc::full<unsigned, 3>(0xFFU)) << '\n';
    cout << std::dec;

    // Compound scalar shifts: shift every element by the same amount.
    ysc::matrix<unsigned, 3> shifted{1U, 2U, 4U};
    shifted <<= 2;
    cout << "{1,2,4} <<= 2      = " << shifted << '\n';
    shifted >>= 1;
    cout << "then       >>= 1   = " << shifted << '\n';
}

} // namespace

int main() {
    try {
        run();
    } catch (const std::exception& e) {
        std::cerr << "unexpected exception: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
