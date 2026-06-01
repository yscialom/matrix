// 05_interop_stl.cpp — interoperability with the C++20/23 standard library.
//
// Covers:
//   * std::ranges algorithms on a matrix (range-of-T in row-major order).
//   * std::sort on flatten() — sort all elements in place.
//   * std::format("{}", m) — same output as operator<<.
//   * std::unordered_set<matrix<…>> via std::hash specialisation.
//   * std::span <-> matrix conversion both ways.

#include <matrix.hpp>

#include <array>
#include <cstddef>
#include <exception>
#include <format>
#include <iostream>
#include <ranges>
#include <span>
#include <unordered_set>

namespace {

void run() {
    using std::cout;

    // ─── std::ranges over a matrix ───────────────────────────────────────────
    cout << "=== std::ranges ===\n";
    ysc::matrix<int, 2, 3> m{3, 1, 4, 1, 5, 9};
    cout << "m                          = " << m << '\n';

    // count_if / max_element / find — all work because matrix exposes
    // standard random-access iterators in row-major order.
    const auto positives = std::ranges::count_if(m, [](int v) { return v > 2; });
    cout << "count_if(v > 2)            = " << positives << " (expect 4)\n";
    cout << "max_element               -> " << *std::ranges::max_element(m) << '\n';

    // Transform into a separate matrix via std::ranges::transform.
    ysc::matrix<int, 2, 3> doubled{};
    std::ranges::transform(m, doubled.begin(), [](int v) { return v * 2; });
    cout << "doubled                    = " << doubled << '\n';

    // ─── std::sort on flatten() ──────────────────────────────────────────────
    cout << "\n=== std::sort on flatten() ===\n";
    auto flat = m.flatten(); // matrix_view<int, contiguous, 6>
    std::ranges::sort(flat);
    cout << "after sort, m              = " << m << '\n';

    // ─── std::format ─────────────────────────────────────────────────────────
    cout << "\n=== std::format ===\n";
    cout << std::format("formatted m = {}\n", m);
    cout << std::format("(row 0 view)= {}\n", m.row(0));

    // ─── std::unordered_set<matrix<…>> via std::hash ─────────────────────────
    cout << "\n=== std::unordered_set<matrix<…>> ===\n";
    std::unordered_set<ysc::matrix<int, 2, 2>> seen;
    seen.insert({1, 2, 3, 4});
    seen.insert({1, 2, 3, 4}); // duplicate
    seen.insert({4, 3, 2, 1});
    cout << "unique matrices kept       = " << seen.size() << " (expect 2)\n";

    // ─── std::span <-> matrix ────────────────────────────────────────────────
    cout << "\n=== std::span <-> matrix ===\n";

    // matrix -> span (via data() + the static size())
    ysc::matrix<int, 2, 3> source{10, 20, 30, 40, 50, 60};
    std::span<int, ysc::matrix<int, 2, 3>::size()> sp_out{source.data(),
                                                          ysc::matrix<int, 2, 3>::size()};
    cout << "span over source = [ ";
    for (int v : sp_out) {
        cout << v << ' ';
    }
    cout << "]\n";

    // span -> matrix (US-053)
    const std::array<int, 4> buf{7, 8, 9, 10};
    const std::span<const int, 4> sp_in{buf};
    const ysc::matrix<int, 2, 2> from_span{sp_in};
    cout << "matrix from span = " << from_span << '\n';
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
