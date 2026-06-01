// 10_pagerank.cpp — PageRank by power iteration on a 6-node graph.
//
// Showcase: build a stochastic transition matrix at compile time with
// generate() multi-index (US-069), iterate the power method using
// matrix × vector products (US-054), normalize via sum + apply, and
// detect convergence with dot.
//
// Graph (directed):
//   0 → {1, 2}        out-degree 2
//   1 → {2}                       1
//   2 → {0, 3}                    2
//   3 → {4, 5}                    2
//   4 → {3}                       1
//   5 → {3, 4}                    2
//
// Damping factor d = 0.85. The transition matrix is column-stochastic
// (each column sums to 1), so PageRank is the dominant eigenvector of
// d·M + (1-d)/N · 1·1ᵀ; here we apply the equivalent iteration
//     v ← d · M·v + (1-d)/N · 1.

#include <matrix.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <exception>
#include <format>
#include <iostream>
#include <ranges>

namespace {

constexpr std::size_t N = 6;

// out_targets[j] is the set of nodes j links to, encoded as a bitmask.
constexpr std::array<unsigned, N> out_targets{
    (1U << 1) | (1U << 2), // 0 → {1, 2}
    (1U << 2),             // 1 → {2}
    (1U << 0) | (1U << 3), // 2 → {0, 3}
    (1U << 4) | (1U << 5), // 3 → {4, 5}
    (1U << 3),             // 4 → {3}
    (1U << 3) | (1U << 4), // 5 → {3, 4}
};

constexpr unsigned popcount(unsigned x) noexcept {
    unsigned c = 0;
    while (x != 0) {
        c += x & 1U;
        x >>= 1U;
    }
    return c;
}

void run() {
    using std::cout;

    // ─── transition matrix via generate (US-069) ─────────────────────────────
    // M(i, j) = 1 / out_degree(j)  if j → i, else 0.
    const auto M = ysc::generate<double, N, N>([](std::size_t i, std::size_t j) {
        const unsigned targets = out_targets.at(j);
        const bool j_links_to_i = (targets & (1U << i)) != 0U;
        if (!j_links_to_i) {
            return 0.0;
        }
        return 1.0 / static_cast<double>(popcount(targets));
    });

    cout << "transition matrix M (column-stochastic):\n";
    for (std::size_t i = 0; i < N; ++i) {
        cout << "  ";
        for (std::size_t j = 0; j < N; ++j) {
            cout << std::format("{:5.2f} ", M(i, j));
        }
        cout << '\n';
    }

    // Each column should sum to 1 — verify with sum<0>() (axis reduction).
    cout << "\ncolumn sums (must be all 1) = " << M.sum<0>() << '\n';

    // ─── power iteration ─────────────────────────────────────────────────────
    constexpr double damping = 0.85;
    constexpr double teleport = (1.0 - damping) / static_cast<double>(N);
    constexpr double tol = 1e-9;
    constexpr int max_iter = 200;

    // Initialise the rank vector to a uniform distribution.
    auto v = ysc::full<double, N>(1.0 / static_cast<double>(N));

    int iter = 0;
    for (; iter < max_iter; ++iter) {
        // v_new = damping · M·v + teleport · 1
        auto v_new = ysc::matmul(M, v); // matrix × vector (US-054)
        v_new *= damping;
        v_new += teleport;

        // L1-normalise (sum + apply): ranks should sum to 1.
        const double s = v_new.sum();
        v_new.apply([s](double& x) { x /= s; });

        // Convergence test via dot: ||v_new - v||² = (v_new-v)·(v_new-v).
        const auto delta = v_new - v;
        const double dist2 = ysc::dot(delta, delta);
        v = v_new;
        if (std::sqrt(dist2) < tol) {
            break;
        }
    }

    // ─── ranking ─────────────────────────────────────────────────────────────
    std::array<std::pair<std::size_t, double>, N> ranking{};
    for (std::size_t i = 0; i < N; ++i) {
        ranking.at(i) = {i, v(i)};
    }
    std::ranges::sort(ranking, [](const auto& a, const auto& b) { return a.second > b.second; });

    cout << "\nconverged after " << iter << " iterations\n";
    cout << "PageRank vector = " << v << '\n';
    cout << "sum (sanity)    = " << v.sum() << '\n';
    cout << "\nranking:\n";
    for (std::size_t k = 0; k < N; ++k) {
        cout << std::format("  #{}  node {} : {:.4f}\n", k + 1, ranking.at(k).first,
                            ranking.at(k).second);
    }
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
