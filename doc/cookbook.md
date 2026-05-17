/**
\page cookbook Cookbook

# Cookbook — Practical Recipes for `ysc::matrix`

This page collects self-contained code snippets for common tasks.
Each recipe is a complete, compilable example.

---

## Iterating over rows and columns

Use `rows()` / `cols()` to obtain strided views, then iterate with a range-based `for`
or any STL algorithm.

```cpp
#include <matrix.hpp>
#include <numeric>    // std::accumulate
#include <iostream>

int main()
{
    ysc::matrix<int, 3, 4> m = {
         1,  2,  3,  4,
         5,  6,  7,  8,
         9, 10, 11, 12
    };

    // Iterate over rows (contiguous views)
    for (std::size_t i = 0; i < m.dimensions[0]; ++i)
    {
        auto row = m.rows(i);
        for (int v : row)
            std::cout << v << ' ';
        std::cout << '\n';
    }

    // Sum of the second column (strided view)
    auto col1 = m.cols(1);
    int col_sum = std::accumulate(col1.begin(), col1.end(), 0);
    // col_sum == 2 + 6 + 10 == 18
}
```

---

## Filling and transforming a matrix

`fill()` resets all elements; `apply()` mutates in place; `map()` returns a new matrix.

```cpp
#include <matrix.hpp>
#include <cmath>    // std::sqrt

int main()
{
    ysc::matrix<double, 3, 3> m;

    // Fill every element with the same value
    m.fill(1.0);

    // Mutate in place: square every element
    m.apply([](double& x) { x *= x; });

    // Produce a new matrix without touching the original
    auto sq = m.map([](const double& x) { return std::sqrt(x); });

    // Diagonal identity matrix
    ysc::matrix<int, 3, 3> identity{};
    identity.fill(0);
    for (std::size_t i = 0; i < 3; ++i)
        identity(i, i) = 1;
}
```

---

## Comparing matrices element-wise vs lexicographic

`operator==` tests exact equality; `operator<=>` gives a lexicographic total order on the
row-major storage.

```cpp
#include <matrix.hpp>

int main()
{
    ysc::matrix<int, 2, 3> a = {1, 2, 3, 4, 5, 6};
    ysc::matrix<int, 2, 3> b = {1, 2, 3, 4, 5, 6};
    ysc::matrix<int, 2, 3> c = {1, 2, 3, 4, 5, 7};

    bool eq  = (a == b);   // true  — all elements equal
    bool neq = (a != c);   // true  — last element differs

    // Lexicographic order (useful for sorted containers / std::sort)
    bool lt  = (a < c);    // true  — a comes before c in row-major order
    bool gt  = (c > a);    // true

    // Element-wise predicate via apply / map
    auto diff = a.map([&](const int& x) { return x; });  // copy of a
    // To test element-wise inequality you can compare map results:
    auto mask = a.map([&b_it = *b.cbegin()](const int&) { return 0; });
    (void)eq; (void)neq; (void)lt; (void)gt; (void)diff; (void)mask;
}
```

---

## Working with views: contiguous vs strided

`slice()` / `row()` / `col()` yield lightweight non-owning views.
Use `matrix_view<const T, ...>` (or the `const_matrix_view` alias) for read-only access.

```cpp
#include <matrix.hpp>
#include <matrix_view.hpp>
#include <numeric>   // std::iota

int main()
{
    ysc::matrix<int, 4, 4> m;
    std::iota(m.begin(), m.end(), 0);   // 0 … 15

    // Contiguous row view (cheap — pointer + static size)
    auto row0 = m.rows(0);              // {0, 1, 2, 3}

    // Strided column view
    auto col2 = m.cols(2);              // {2, 6, 10, 14}

    // Read-only view from a const matrix
    const ysc::matrix<int, 4, 4>& cm = m;
    ysc::const_matrix_view<int, 4, 4> ro{cm};  // explicit ctor

    // Composable slicing: view of view
    auto sub = row0.slice(0);           // first element as a 1D view

    // Owning copy from a view (deep copy)
    ysc::matrix<int, 4> col_copy{col2};

    (void)ro; (void)sub; (void)col_copy;
}
```

---

## Interop with `std::ranges`, `std::format`, `std::hash`

`ysc::matrix` satisfies `std::ranges::contiguous_range` and provides `std::hash` and
`std::formatter` specializations.

```cpp
#include <matrix.hpp>
#include <algorithm>    // std::ranges::sort, std::ranges::transform
#include <unordered_set>
#include <format>       // C++23 or later (guarded in the library)

int main()
{
    ysc::matrix<int, 2, 3> m = {3, 1, 4, 1, 5, 9};

    // std::ranges algorithms work out-of-the-box
    std::ranges::sort(m);               // sorts in row-major order: {1,1,3,4,5,9}

    ysc::matrix<double, 2, 3> out;
    std::ranges::transform(m, out.begin(),
        [](int x) { return x * 1.5; });

    // std::hash — use in unordered containers
    std::unordered_set<ysc::matrix<int, 2, 3>> seen;
    seen.insert(m);
    seen.insert(m);   // duplicate — not inserted again

    // std::format (requires __cpp_lib_format >= 201907L and Clang >= 17)
#if defined(__cpp_lib_format) && __cpp_lib_format >= 201907L \
    && (!defined(__clang__) || __clang_major__ >= 17)
    std::string s = std::format("{}", m);
    (void)s;
#endif

    (void)out;
}
```

---

## Solving Ax=b with `dot`, `transpose`, `matmul`

Combining `transpose()`, `matmul()`, and `dot()` covers many linear-algebra workflows.

```cpp
#include <matrix.hpp>

int main()
{
    // --- Matrix–vector multiplication (Ax) ---
    ysc::matrix<double, 2, 3> A = {1, 0, 0,
                                    0, 1, 0};
    ysc::matrix<double, 3>    x = {3, 5, 7};

    // matmul supports matrix × column-vector
    auto Ax = ysc::matmul(A, x);        // matrix<double, 2>: {3, 5}

    // --- Dot product ---
    ysc::matrix<double, 3> u = {1, 2, 3};
    ysc::matrix<double, 3> v = {4, 5, 6};
    double uv = ysc::dot(u, v);         // 1*4 + 2*5 + 3*6 == 32

    // --- Transpose ---
    ysc::matrix<double, 2, 3> M = {1, 2, 3,
                                    4, 5, 6};
    auto Mt = ysc::transpose(M);        // matrix<double, 3, 2>

    // --- Normal equations: (A^T A) x = A^T b ---
    // For a 3×2 system:
    ysc::matrix<double, 3, 2> B = {1, 2,
                                    3, 4,
                                    5, 6};
    ysc::matrix<double, 3>    b = {1, 0, 1};

    auto Bt    = ysc::transpose(B);     // 2×3
    auto BtB   = ysc::matmul(Bt, B);   // 2×2
    auto Btb   = ysc::matmul(Bt, b);   // 2

    (void)Ax; (void)uv; (void)Mt; (void)BtB; (void)Btb;
}
```

*/
