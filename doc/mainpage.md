@mainpage ysc::matrix — Multi-dimensional Array Library

<div align="center">
\image html ysc-matrix-banner.png ""
</div>

`ysc::matrix<T, Dims...>` is a **header-only C++20** template library providing a
general-purpose multi-dimensional container with static dimensions.

It has the performance of a raw C array with the ergonomics of a standard container:
no heap allocation, no overhead, full `constexpr` support, and a complete STL-compatible
interface.

---

## Quick Start

```cpp
#include <matrix.hpp>

// A 3×4 matrix of doubles
ysc::matrix<double, 3, 4> m;

// Aggregate initialization
ysc::matrix<int, 2, 3> grid = {1, 2, 3,
                                4, 5, 6};

// Element access — unchecked (fast path)
grid(0, 1) = 42;

// Element access — bounds-checked (throws std::out_of_range)
int v = grid.at(1, 2);

// Range-based for loop (row-major order)
for (int& x : grid)
    x *= 2;
```

---

## Installation

### CMake FetchContent (recommended)

```cmake
include(FetchContent)
FetchContent_Declare(
    ysc-matrix
    GIT_REPOSITORY https://github.com/yscialom/matrix.git
    GIT_TAG        v1.0.0
)
FetchContent_MakeAvailable(ysc-matrix)

target_link_libraries(my_target PRIVATE ysc::matrix)
```

### Manual

Download `matrix.hpp` from a [GitHub Release](https://github.com/yscialom/matrix/releases) and
copy it into your project.

```cpp
#include "matrix.hpp"
```

---

## Features

| Feature | Description |
|---------|-------------|
| N-dimensional | Any number of dimensions; all sizes fixed at compile time |
| STL-compatible | `begin`/`end`, `size()`, `data()`, `front()`, `back()`, `fill()`, `swap()` |
| Unchecked access | `operator()` — no bounds check, UB out-of-bounds (performance path) |
| Bounds-checked access | `at()` — throws `std::out_of_range` |
| Aggregate init | `matrix<int,2,3> m = {1,2,3,4,5,6};` |
| Type conversion | Converting constructors and assignment from `matrix<U, Dims...>` |
| Comparison | `==` and `<=>` (lexicographic on row-major storage) |
| Compile-time metadata | `order`, `dimensions`, `size()`, `empty()` — all `static constexpr` |
| Zero-init tag | `matrix<T, Dims...> m{ysc::zero};` — explicit zero-initialization |
| Zero overhead | Storage is `std::array<T, N>`, no heap, no virtual, no indirection |

---

## Cookbook

Looking for practical, copy-paste examples? The @ref cookbook page collects six
ready-to-use recipes covering the first use-cases — element access, factory
functions, algorithms, zero-copy views, and linear algebra — and showcases the
library's signature features: `constexpr` linear algebra, compile-time dimension
checks, and views that carry their shape in the type.

---

## API Reference

The API is organized into the following groups — see the [Topics](topics.html) page for a full listing:
- @ref ysc_construction — Construction (constructors, assignment, `zeros`, `full`, `ones`, `identity`, `generate`)
- @ref ysc_access — Element access (`operator()`, `at()`)
- @ref ysc_iterators — Iterators (`begin`, `end`, `rbegin`, `rend`, and const variants)
- @ref ysc_capacity — Capacity (`size()`, `empty()`, `data()`, `order`, `dimensions`)
- @ref ysc_modifiers — Modifiers (`fill`, `swap`, `front`, `back`)
- @ref ysc_comparison — Comparison (`==`, `<=>`)
- @ref ysc_arithmetic — Arithmetic operators (element-wise and scalar)
- @ref ysc_algorithms — Algorithms (`apply`, `map`, `sum`, `min`, `max`, `all`, `any`)
- @ref ysc_linalg — Linear algebra (`transpose`, `matmul`, `dot`)
- @ref ysc_views — Views (`matrix_view`, `row`, `col`, `slice`, `rows`, `cols`, `reshape`, `flatten`)
- @ref ysc_io — I/O (`operator<<`, `std::formatter`)
- @ref ysc_hash — Hash support (`std::hash` specialization)

For users upgrading from v0.x, see the [Migration Guide](migration.html) for breaking changes
and the stability promise.

You can also browse the full documentation for the main library classes:
- Owning matrix — @ref ysc::matrix
- Non-owning continuous view — @ref ysc::matrix_view< T, contiguous, Dimensions... >
- Non-owning strided view — @ref ysc::matrix_view< T, strided, Dimensions... >
