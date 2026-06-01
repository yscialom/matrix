// 01_basics.cpp — construction, access, iteration.
//
// Covers:
//   * Construction: default, parameter pack, factories (zeros / ones /
//     identity), from std::array (US-053), from std::span (US-053), generate
//     with linear index (US-053) and multi-index (US-069).
//   * Access: operator()(coords...) (unchecked) and at() (bounds-checked),
//     including the detailed std::out_of_range message from US-056.
//   * Iteration: linear range-for, rows() and cols() (US-054).

#include <matrix.hpp>

#include <array>
#include <cstddef>
#include <exception>
#include <iostream>
#include <span>
#include <stdexcept>

namespace {

void run() {
    using std::cout;
    using std::size_t;

    // ─── Construction ────────────────────────────────────────────────────────
    cout << "=== Construction ===\n";

    // Factories
    auto m_zero = ysc::zeros<int, 2, 3>();
    auto m_one = ysc::ones<double, 3>();
    auto m_id = ysc::identity<double, 3>();
    cout << "zeros<int,2,3>     = " << m_zero << '\n';
    cout << "ones<double,3>     = " << m_one << '\n';
    cout << "identity<double,3> = " << m_id << '\n';

    // Parameter pack — row-major order
    const ysc::matrix<int, 2, 3> m_pack{1, 2, 3, 4, 5, 6};
    cout << "from pack          = " << m_pack << '\n';

    // From std::array (US-053)
    const std::array<int, 6> arr{10, 20, 30, 40, 50, 60};
    const ysc::matrix<int, 2, 3> m_from_array{arr};
    cout << "from std::array    = " << m_from_array << '\n';

    // From std::span (US-053)
    const std::span<const int, 6> sp{arr};
    const ysc::matrix<int, 2, 3> m_from_span{sp};
    cout << "from std::span     = " << m_from_span << '\n';

    // generate() with a linear index — index runs over the flattened storage
    // (US-053).
    const auto m_squares = ysc::generate<int, 5>([](size_t i) { return static_cast<int>(i * i); });
    cout << "squares (1D)       = " << m_squares << '\n';

    // generate() with a multi-index — callable receives one coordinate per
    // dimension (US-069).
    const auto m_table = ysc::generate<int, 3, 3>(
        [](size_t i, size_t j) { return static_cast<int>((i + 1) * (j + 1)); });
    cout << "mult table (2D)    = " << m_table << '\n';

    // ─── Access ──────────────────────────────────────────────────────────────
    cout << "\n=== Access ===\n";

    // Unchecked, fast: undefined behaviour on out-of-bounds.
    cout << "m_table(2, 2)      = " << m_table(2, 2) << '\n';

    // Checked: throws std::out_of_range on out-of-bounds.
    cout << "m_table.at(0, 1)   = " << m_table.at(0, 1) << '\n';

    // Detailed exception message (US-056).
    try {
        (void)m_table.at(5, 0);
    } catch (const std::out_of_range& e) {
        cout << "at(5, 0) threw     : " << e.what() << '\n';
    }

    // ─── Iteration ───────────────────────────────────────────────────────────
    cout << "\n=== Iteration ===\n";

    // Linear range-for: row-major traversal.
    cout << "linear             = [ ";
    for (const int v : m_table) {
        cout << v << ' ';
    }
    cout << "]\n";

    // rows() returns one view per row (US-054).
    cout << "rows():\n";
    for (auto row_view : m_table.rows()) {
        cout << "  [ ";
        for (const int v : row_view) {
            cout << v << ' ';
        }
        cout << "]\n";
    }

    // cols() returns one (strided) view per column (US-054).
    cout << "cols():\n";
    for (auto col_view : m_table.cols()) {
        cout << "  [ ";
        for (const int v : col_view) {
            cout << v << ' ';
        }
        cout << "]\n";
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
