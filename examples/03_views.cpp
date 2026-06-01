// 03_views.cpp — non-owning views over a matrix.
//
// Covers:
//   * slice(), row(i), col(j) on a matrix.
//   * reshape<...>() and flatten() — zero-copy re-interpretation.
//   * submatrix<R, C, ...>({origin...}) — arbitrary N-D sub-block (US-061).
//   * Composition: matrix_view → row / col / slice (US-052).
//   * Const-correctness: matrix_view<T, …> on a mutable matrix vs
//     matrix_view<const T, …> on a const matrix.
//
// Note: operator<< is defined for contiguous matrix_view only.
// Strided views (m.col(j), submatrix(...), slice(all, j), …) can be
// materialized into a ysc::matrix to be printed.

#include <matrix.hpp>

#include <exception>
#include <iostream>
#include <type_traits>

namespace {

template <class V> void print_2d(const char* label, const V& v) {
    std::cout << label << " = [";
    for (std::size_t i = 0; i < V::dimensions[0]; ++i) {
        std::cout << (i == 0 ? "" : ", ") << "[";
        for (std::size_t j = 0; j < V::dimensions[1]; ++j) {
            std::cout << (j == 0 ? "" : ", ") << v(i, j);
        }
        std::cout << "]";
    }
    std::cout << "]\n";
}

void run() {
    using std::cout;

    // clang-format off
    ysc::matrix<int, 4, 4> m{ 1,  2,  3,  4,
                              5,  6,  7,  8,
                              9, 10, 11, 12,
                             13, 14, 15, 16};
    // clang-format on

    // ─── row / col / slice ───────────────────────────────────────────────────
    cout << "=== row / col / slice ===\n";

    // row(i) is contiguous and prints directly.
    cout << "m.row(1)                       = " << m.row(1) << '\n';

    // col(j) is strided; materialize it into a matrix to print.
    cout << "m.col(2) (materialised)        = " << ysc::matrix{m.col(2)} << '\n';

    // slice with ysc::all to keep a dimension, integer to fix it.
    // slice(0) keeps row 0 (contiguous prefix); slice(ysc::all, 0) selects col 0 (strided).
    cout << "m.slice(0)                     = " << m.slice(0) << '\n';
    cout << "m.slice(all, 0) (materialised) = " << ysc::matrix{m.slice(ysc::all, 0)} << '\n';

    // ─── reshape / flatten ───────────────────────────────────────────────────
    cout << "\n=== reshape / flatten ===\n";

    auto flat = m.flatten();
    cout << "m.flatten()                    = " << flat << '\n';

    auto v2x8 = m.reshape<2, 8>();
    cout << "m.reshape<2,8>()               = " << v2x8 << '\n';

    // Mutations through the view propagate back to m (zero-copy).
    v2x8(0, 0) = -1;
    cout << "after v2x8(0,0)=-1, m(0,0)     = " << m(0, 0) << '\n';
    v2x8(0, 0) = 1; // restore

    // ─── submatrix N-D (US-061) ──────────────────────────────────────────────
    cout << "\n=== submatrix (US-061) ===\n";

    auto sub2x2 = m.submatrix<2, 2>({1, 1});
    print_2d("submatrix<2,2>@(1,1)", sub2x2);

    auto sub3x2 = m.submatrix<3, 2>({0, 2});
    print_2d("submatrix<3,2>@(0,2)", sub3x2);

    // 3D: a 1×2×2 sub-block of a 2×3×4 tensor.
    auto t = ysc::generate<int, 2, 3, 4>([](std::size_t i, std::size_t j, std::size_t k) {
        return static_cast<int>((i * 100) + (j * 10) + k);
    });
    auto sub3d = t.submatrix<1, 2, 2>({1, 1, 1});
    cout << "submatrix<1,2,2>@(1,1,1) of 2x3x4 tensor:\n";
    for (std::size_t j = 0; j < 2; ++j) {
        cout << "  [";
        for (std::size_t k = 0; k < 2; ++k) {
            cout << (k == 0 ? "" : ", ") << sub3d(0, j, k);
        }
        cout << "]\n";
    }

    // ─── view composition (US-052) ───────────────────────────────────────────
    cout << "\n=== view composition (US-052) ===\n";

    // reshape() returns a contiguous view; row / col then chain on it.
    cout << "m.reshape<2,8>().row(1)        = " << v2x8.row(1) << '\n';
    cout << "m.reshape<2,8>().col(3) (mat)  = " << ysc::matrix{v2x8.col(3)} << '\n';

    // Mutate through a chained view — the change reaches the underlying matrix.
    v2x8.row(0)(7) = 99;
    cout << "after v2x8.row(0)(7)=99 -> m(0,7) = " << m(0, 7) << '\n';
    v2x8.row(0)(7) = 8; // restore

    // ─── const-correctness ───────────────────────────────────────────────────
    cout << "\n=== const-correctness ===\n";

    const ysc::matrix<int, 2, 3> cm{1, 2, 3, 4, 5, 6};
    // cm.row(0) returns matrix_view<const int, contiguous, 3> — read-only.
    static_assert(
        std::is_same_v<decltype(cm.row(0)), ysc::matrix_view<const int, ysc::contiguous, 3>>);
    cout << "cm.row(0)                      = " << cm.row(0) << '\n';

    ysc::matrix<int, 2, 3> mm{1, 2, 3, 4, 5, 6};
    // mm.row(0) returns matrix_view<int, contiguous, 3> — mutable.
    static_assert(std::is_same_v<decltype(mm.row(0)), ysc::matrix_view<int, ysc::contiguous, 3>>);

    mm.row(0)(1) = 42;
    cout << "after mm.row(0)(1)=42, mm(0,1) = " << mm(0, 1) << '\n';
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
