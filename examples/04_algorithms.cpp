// 04_algorithms.cpp — algorithms on a matrix.
//
// Covers:
//   * apply(F)            — in-place mutation of every element.
//   * map(F)              — produce a new matrix from a transformation.
//   * sum / min / max     — scalar reductions over the whole matrix.
//   * all() / any()       — boolean reductions (predicate-free, on the values).
//   * sum<Axis>(), min<Axis>(), max<Axis>() — reductions along one axis (US-060).
//   * enumerate()         — iterate (coordinates, value) pairs (US-062).

#include <matrix.hpp>

#include <cmath>
#include <exception>
#include <iostream>

namespace {

void run() {
    using std::cout;

    const ysc::matrix<int, 2, 3> m{1, 2, 3, 4, 5, 6};
    cout << "m = " << m << "\n\n";

    // ─── apply / map ─────────────────────────────────────────────────────────
    cout << "=== apply / map ===\n";

    // map: pure, returns a new matrix; can change the element type.
    const auto squared = m.map([](int v) { return v * v; });
    cout << "m.map(x -> x*x) = " << squared << '\n';

    const auto as_double = m.map([](int v) { return std::sqrt(static_cast<double>(v)); });
    cout << "m.map(sqrt)     = " << as_double << '\n';

    // apply: in-place, return type stays the same.
    auto mm = m;
    mm.apply([](int& v) { v += 100; });
    cout << "after apply(+=100) = " << mm << '\n';

    // ─── scalar reductions ───────────────────────────────────────────────────
    cout << "\n=== scalar reductions ===\n";
    cout << "sum(m) = " << m.sum() << '\n'; // 21
    cout << "min(m) = " << m.min() << '\n'; // 1
    cout << "max(m) = " << m.max() << '\n'; // 6

    const ysc::matrix<int, 3> truthy{1, 2, 3};
    const ysc::matrix<int, 3> mixed{1, 0, 3};
    cout << std::boolalpha;
    cout << "truthy.all() = " << truthy.all() << " (expect true)\n";
    cout << "mixed.all()  = " << mixed.all() << " (expect false — one zero)\n";
    cout << "mixed.any()  = " << mixed.any() << " (expect true)\n";
    cout << std::noboolalpha;

    // ─── axis reductions (US-060) ────────────────────────────────────────────
    cout << "\n=== axis reductions (US-060) ===\n";
    // m is 2x3. Axis 0 has size 2 (rows), axis 1 has size 3 (cols).
    cout << "m.sum<0>() (collapse rows, one value per col) = " << m.sum<0>() << '\n';
    cout << "m.sum<1>() (collapse cols, one value per row) = " << m.sum<1>() << '\n';
    cout << "m.max<0>() = " << m.max<0>() << '\n';
    cout << "m.min<1>() = " << m.min<1>() << '\n';

    // ─── enumerate (US-062) ──────────────────────────────────────────────────
    cout << "\n=== enumerate() (US-062) ===\n";
    cout << "(coords, value) pairs:\n";
    for (auto [coords, value] : m.enumerate()) {
        cout << "  (" << coords[0] << ", " << coords[1] << ") -> " << value << '\n';
    }

    // enumerate() can also write into the matrix when called on a non-const
    // instance: e.g. set m_writable(i, j) = i * 10 + j.
    ysc::matrix<int, 2, 3> m_writable{};
    for (auto [coords, val] : m_writable.enumerate()) {
        val = static_cast<int>((coords[0] * 10) + coords[1]);
    }
    cout << "m_writable after enumerate() write = " << m_writable << '\n';
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
