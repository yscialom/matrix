// 08_kalman_filter_1d.cpp — 1D constant-velocity Kalman filter.
//
// Showcase: a small, complete robotics / sensor-fusion algorithm where
// every matrix is built at compile time (constexpr F, H, Q, R, I) and
// every step is a chain of matmul / transpose / matrix-vector products.
//
// State vector x = (position, velocity)ᵀ                          (size 2)
// Transition   F = [[1, dt], [0, 1]]                              (2×2)
// Observation  H = [[1, 0]]                                       (1×2)
// Process cov  Q                                                  (2×2)
// Measurement cov R (scalar, encoded as 1×1)
//
// Predict:  x ← F·x ,  P ← F·P·Fᵀ + Q
// Update :  y = z − H·x ,  S = H·P·Hᵀ + R ,  K = P·Hᵀ·S⁻¹
//           x ← x + K·y ,  P ← (I − K·H)·P

#include <matrix.hpp>

#include <exception>
#include <iomanip>
#include <iostream>
#include <random>

namespace {

constexpr std::size_t steps = 20;
constexpr double dt = 1.0;
constexpr double measurement_sigma = 0.5;

// clang-format off
constexpr ysc::matrix<double, 2, 2> F{1.0, dt,
                                      0.0, 1.0};
constexpr ysc::matrix<double, 1, 2> H{1.0, 0.0};
constexpr ysc::matrix<double, 2, 2> Q{1e-4, 0.0,
                                       0.0, 1e-4};
constexpr ysc::matrix<double, 1, 1> R{measurement_sigma * measurement_sigma};
// clang-format on

void run() {
    using std::cout;

    // True trajectory: x(t) = 0 + 1.0 · t.
    // Synthesize noisy measurements with a fixed seed for reproducibility.
    std::mt19937 rng(42);
    std::normal_distribution<double> noise(0.0, measurement_sigma);

    // Initial estimate: position guessed at 0.5 (off by 0.5), velocity unknown.
    auto x = ysc::matrix<double, 2>{0.5, 0.0};
    auto P = ysc::identity<double, 2>(); // start with high uncertainty
    P *= 1.0;
    const auto I2 = ysc::identity<double, 2>();

    cout << std::fixed << std::setprecision(3);
    cout << "step |   z   | x_true | x_est | v_est\n";
    cout << "-----+-------+--------+-------+------\n";

    for (std::size_t k = 0; k < steps; ++k) {
        const double t = static_cast<double>(k) * dt;
        const double x_true = t;
        const double z = x_true + noise(rng);

        // ── Predict ──────────────────────────────────────────────────────────
        x = ysc::matmul(F, x); // matrix × vector (US-054)
        P = ysc::matmul(ysc::matmul(F, P), ysc::transpose(F)) + Q;

        // ── Update ───────────────────────────────────────────────────────────
        const ysc::matrix<double, 1> z_vec{z};
        const auto y = z_vec - ysc::matmul(H, x);
        const auto S = ysc::matmul(ysc::matmul(H, P), ysc::transpose(H)) + R;
        const auto Ht = ysc::transpose(H);
        const auto K = ysc::matmul(P, Ht) / S(0, 0);

        x = x + ysc::matmul(K, y);
        P = ysc::matmul(I2 - ysc::matmul(K, H), P);

        cout << std::setw(4) << k                      //
             << " | " << std::setw(5) << z             //
             << " | " << std::setw(6) << x_true        //
             << " | " << std::setw(5) << x(0)          //
             << " | " << std::setw(5) << x(1) << '\n'; //
    }

    cout << "\nfinal P (estimate covariance) = " << P << '\n';
    // trace(P) via dot of the diagonal with the all-ones vector (matrix×vector,
    // US-054, applied to the identity row sums).
    const ysc::matrix<double, 2> diag{P(0, 0), P(1, 1)};
    const ysc::matrix<double, 2> ones{1.0, 1.0};
    cout << "trace(P)                       = " << ysc::dot(diag, ones) << '\n';
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
