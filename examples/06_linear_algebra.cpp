// 06_linear_algebra.cpp — linear algebra essentials.
//
// Covers:
//   * dot(a, b)          — 1D vector dot product.
//   * transpose(m)       — 2D matrix transpose.
//   * matmul(A, B)       — matrix × matrix product.
//   * matmul(A, v)       — matrix × column-vector product (US-054).
//
// Showcase: solve a small overdetermined system Ax = b by the normal
// equations:
//      x = (Aᵀ A)⁻¹ Aᵀ b
// We fit y = β₀ + β₁·t on three samples; the 2×2 inverse is computed
// by hand to keep the example dependency-free.

#include <matrix.hpp>

#include <exception>
#include <iostream>

namespace {

// Closed-form inverse of a 2×2 matrix (assumes non-singular).
constexpr ysc::matrix<double, 2, 2> inv2x2(const ysc::matrix<double, 2, 2>& m) {
    const double det = (m(0, 0) * m(1, 1)) - (m(0, 1) * m(1, 0));
    return ysc::matrix<double, 2, 2>{m(1, 1) / det, -m(0, 1) / det, //
                                     -m(1, 0) / det, m(0, 0) / det};
}

void run() {
    using std::cout;

    // ─── dot product ─────────────────────────────────────────────────────────
    cout << "=== dot ===\n";
    const ysc::matrix<double, 3> u{1.0, 2.0, 3.0};
    const ysc::matrix<double, 3> v{0.5, 1.0, 1.5};
    cout << "u . v                = " << ysc::dot(u, v) << " (expect 7)\n";

    // ─── transpose ───────────────────────────────────────────────────────────
    cout << "\n=== transpose ===\n";
    const ysc::matrix<int, 2, 3> M{1, 2, 3, 4, 5, 6};
    cout << "M                    = " << M << '\n';
    cout << "transpose(M)         = " << ysc::transpose(M) << '\n';

    // ─── matmul matrix × matrix ──────────────────────────────────────────────
    cout << "\n=== matmul (mat × mat) ===\n";
    const ysc::matrix<double, 2, 3> A{1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
    const ysc::matrix<double, 3, 2> B{7.0, 8.0, 9.0, 10.0, 11.0, 12.0};
    cout << "A (2x3) * B (3x2)    = " << ysc::matmul(A, B) << '\n';

    // ─── matmul matrix × vector (US-054) ─────────────────────────────────────
    cout << "\n=== matmul (mat × vec, US-054) ===\n";
    const ysc::matrix<double, 3> x{1.0, 1.0, 1.0};
    cout << "A * x                = " << ysc::matmul(A, x) << " (row sums)\n";

    // ─── showcase: Ax = b by normal equations ────────────────────────────────
    // Fit y = β₀ + β₁·t to three samples (t_i, y_i):
    //   (0, 1.0), (1, 2.5), (2, 4.5)   — slope ≈ 1.75, intercept ≈ 0.83
    cout << "\n=== least-squares fit via normal equations ===\n";
    // clang-format off
    const ysc::matrix<double, 3, 2> Afit{
        1.0, 0.0,
        1.0, 1.0,
        1.0, 2.0,
    };
    // clang-format on
    const ysc::matrix<double, 3> bfit{1.0, 2.5, 4.5};

    const auto At = ysc::transpose(Afit);   // 2 x 3
    const auto AtA = ysc::matmul(At, Afit); // 2 x 2
    const auto Atb = ysc::matmul(At, bfit); // vector of size 2
    const auto beta = ysc::matmul(inv2x2(AtA), Atb);

    cout << "Aᵀ A                 = " << AtA << '\n';
    cout << "Aᵀ b                 = " << Atb << '\n';
    cout << "β = (Aᵀ A)⁻¹ Aᵀ b    = " << beta << '\n';
    cout << "  intercept β₀       = " << beta(0) << '\n';
    cout << "  slope     β₁       = " << beta(1) << '\n';

    // Residuals: r = b - A β
    const auto residuals = bfit - ysc::matmul(Afit, beta);
    cout << "residuals (b - Aβ)   = " << residuals << '\n';
    cout << "||r||²               = " << ysc::dot(residuals, residuals) << '\n';
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
