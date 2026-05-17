/**
\page cookbook Cookbook

# Cookbook — Practical Recipes for `ysc::matrix`

Six self-contained recipes for the first things you will want to do with
`ysc::matrix`. Each one also highlights a property of the library that sets it
apart from raw C arrays, nested `std::array`, Eigen, or `std::mdspan`.

---

## 1. Hello, matrix

Aggregate-initialize a matrix, mutate one element through `operator()`, and
print it. Same syntax as a C array — but bounds-checking, comparison,
iteration, and `std::format` come for free.

```cpp
#include <matrix.hpp>
#include <iostream>

int main()
{
    ysc::matrix<int, 2, 3> m = {1, 2, 3,
                                4, 5, 6};

    // Unchecked write — fast path, UB out of bounds
    m(1, 2) = 42;

    // Bounds-checked read — throws std::out_of_range on bad coordinates
    int top_right = m.at(0, 2);

    std::cout << "m = " << m << '\n';
    std::cout << "m(0, 2) = " << top_right << '\n';
}
```

> **Why it matters.** The storage is a `std::array<int, 6>` — same layout as
> `int m[2][3]`, zero indirection, zero allocation. You only pay for what you
> use.

---

## 2. Algebra at compile time

Every linear-algebra primitive is `constexpr`. Move invariants from the
test suite into the build itself — broken dimensions or wrong values
become compile errors, not runtime surprises.

```cpp
#include <matrix.hpp>

int main()
{
    constexpr ysc::matrix<int, 3> u = {1, 2, 3};
    constexpr ysc::matrix<int, 3> v = {4, 5, 6};
    static_assert(ysc::dot(u, v) == 32);          // 1*4 + 2*5 + 3*6

    constexpr ysc::matrix<int, 2, 2> A = {1, 2,
                                          3, 4};
    static_assert(ysc::transpose(A)(0, 1) == 3);  // A^T has 3 at (0,1)
    static_assert(ysc::matmul(A, A)(0, 0) == 7);  // (A*A)(0,0) = 1+6
    static_assert(A.sum() == 10);
}
```

> **Why it matters.** Neither Eigen nor `std::mdspan` lets you evaluate a
> matrix product inside a `static_assert`. Here the whole API is `constexpr`,
> so unit-test-grade checks run during compilation.

---

## 3. Factory functions

Five constexpr factories cover the matrices you most often need to type
out by hand. All are type-safe and dimension-aware.

```cpp
#include <matrix.hpp>

int main()
{
    constexpr auto zeros    = ysc::zeros<double, 3, 3>();              // all 0.0
    constexpr auto ones     = ysc::ones<int, 4>();                     // {1,1,1,1}
    constexpr auto pi_block = ysc::full<double, 2, 2>(3.14);           // every entry = 3.14
    constexpr auto I        = ysc::identity<float, 3>();               // 3×3 identity
    constexpr auto squares  = ysc::generate<int, 2, 3>(
        [](std::size_t k) { return int(k * k); });                     // 0,1,4,9,16,25

    static_assert(I(2, 2) == 1.0f);
    static_assert(squares(1, 2) == 25);
}
```

> **Why it matters.** All five live in the same header you already include,
> all are `constexpr`, and all carry their dimensions in the type. No
> `Eigen::MatrixXd::Identity(n, n)` runtime sizing surprise.

---

## 4. Map, reduce, transform

Mutate in place with `apply`, build a new matrix with `map`, reduce with
`sum` / `min` / `max` / `all` / `any`. No need to drop down to STL
algorithms for the common cases.

```cpp
#include <matrix.hpp>
#include <iostream>

int main()
{
    ysc::matrix<double, 2, 3> m = {-2.0, -1.0, 0.0,
                                    1.0,  2.0, 3.0};

    // In-place: scale every element by 2
    m.apply([](double& x) { x *= 2.0; });

    // New matrix: square every element (note the changed value type below)
    ysc::matrix<double, 2, 3> sq = m.map([](double x) { return x * x; });

    double total    = sq.sum();
    double smallest = m.min();
    double largest  = m.max();
    bool   nonneg   = sq.map([](double x) { return x >= 0.0; }).all();

    std::cout << "sum = " << total << ", min = " << smallest
              << ", max = " << largest << ", all non-negative = "
              << std::boolalpha << nonneg << '\n';
}
```

> **Why it matters.** `apply` and `map` are the matrix-native shorthands for
> `std::ranges::for_each` and `std::ranges::transform`, but they keep the
> dimensions in the type — so `m.map(f)` produces a `matrix<U, Dims...>`,
> not a `std::vector`.

---

## 5. Zero-copy views and slicing

`row(i)`, `col(j)`, `reshape<…>()`, and `flatten()` return lightweight
non-owning views — pointer + static shape, no allocation. Use them like
`std::string_view` for matrices.

```cpp
#include <matrix.hpp>
#include <iostream>

int main()
{
    // Build a 4×4 matrix m where m(i,j) = 4*i + j  →  0..15
    auto m = ysc::generate<int, 4, 4>(
        [](std::size_t k) { return int(k); });

    auto r1 = m.row(1);          // contiguous view: {4, 5, 6, 7}
    auto c2 = m.col(2);          // strided view:    {2, 6, 10, 14}

    // Mutate the source through a view
    r1.fill(-1);                 // row 1 of m becomes {-1, -1, -1, -1}

    // Reshape and flatten are also zero-copy views
    auto flat = m.flatten();     // matrix_view<int, contiguous, 16>
    auto wide = m.reshape<2, 8>();

    // Owning copy from a view (deep copy via explicit ctor)
    ysc::matrix<int, 4> c2_owned{c2};

    std::cout << "m = " << m << '\n';
    std::cout << "c2_owned = " << c2_owned << '\n';
}
```

**Const-correctness propagates automatically.** On a non-const `matrix<T, …>`,
`row` / `col` / `slice` return `matrix_view<T, …>` (mutable). On a
`const matrix<T, …>` they return `matrix_view<const T, …>` — exactly the
type aliased as `ysc::const_matrix_view<T, …>`. You do not pick the view
type; the language does:

```cpp
ysc::matrix<int, 4, 4> mutable_m = ysc::zeros<int, 4, 4>();
const auto&            const_m   = mutable_m;

auto rm = mutable_m.row(0);  // matrix_view<int, contiguous, 4>       (writable)
auto rc = const_m.row(0);    // matrix_view<const int, contiguous, 4> (read-only)
```

> **Why it matters.** Slicing in Eigen returns expression templates with
> non-trivial lifetimes; `std::mdspan` is a view but not a container; nested
> `std::array` cannot slice at all. Here a view is a pointer + a compile-time
> shape, composable, and the const-ness is enforced by the type system.

---

## 6. Linear algebra essentials

`transpose`, `matmul`, and `dot` cover most everyday linear algebra.
`matmul` is overloaded for matrix × matrix and matrix × column-vector, and
mismatched inner dimensions are a compile error — not a runtime check.

```cpp
#include <matrix.hpp>
#include <iostream>

int main()
{
    // Matrix-vector product (Ax)
    ysc::matrix<double, 2, 3> A = {1, 0, 0,
                                   0, 1, 0};
    ysc::matrix<double, 3>    x = {3, 5, 7};
    auto Ax = ysc::matmul(A, x);              // matrix<double, 2>: {3, 5}

    // Dot product on 1-D vectors
    ysc::matrix<double, 3> u = {1, 2, 3};
    ysc::matrix<double, 3> v = {4, 5, 6};
    double uv = ysc::dot(u, v);               // 32

    // Normal equations  B^T B  and  B^T b  (no solver — that is for a future US)
    ysc::matrix<double, 3, 2> B = {1, 2,
                                   3, 4,
                                   5, 6};
    ysc::matrix<double, 3>    b = {1, 0, 1};
    auto BtB = ysc::matmul(ysc::transpose(B), B);   // 2×2
    auto Btb = ysc::matmul(ysc::transpose(B), b);   // length-2 vector

    std::cout << "Ax = "  << Ax  << '\n';
    std::cout << "uv = "  << uv  << '\n';
    std::cout << "BtB = " << BtB << ", Btb = " << Btb << '\n';
}
```

> **Why it matters.** `ysc::matmul(matrix<T, M, N>, matrix<T, K, P>)` does not
> compile when `N != K`. With Eigen's dynamic matrices that mistake is a
> runtime assertion; with raw arrays it is silent undefined behaviour. Static
> dimensions catch it at the build.

*/
