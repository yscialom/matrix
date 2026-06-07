<p align="center">
  <img src="doc/logo/ysc-matrix-banner.png" alt="ysc::matrix banner">
</p>

# `ysc::matrix`

[![CI](https://github.com/yscialom/matrix/actions/workflows/ci.yml/badge.svg)](https://github.com/yscialom/matrix/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/yscialom/matrix/graph/badge.svg)](https://codecov.io/gh/yscialom/matrix)
[![docs](https://github.com/yscialom/matrix/actions/workflows/docs.yml/badge.svg)](https://yscialom.github.io/matrix/)

> A **header-only C++20** multi-dimensional container with **static dimensions** —
> the performance of a raw C array, the ergonomics of `std::array`, extended to N dimensions.

## Quick Start

```cpp
#include <matrix.hpp>
#include <iostream>

int main()
{
    // A 3×3 matrix of ints, aggregate-initialized
    ysc::matrix<int, 3, 3> m = {1, 2, 3,
                                4, 5, 6,
                                7, 8, 9};

    m(1, 1) = 42;                 // unchecked write (fast path)
    int v = m.at(2, 2);           // bounds-checked read

    for (int& x : m) x *= 2;      // range-based for, row-major

    auto t = ysc::transpose(m);   // constexpr-friendly linear algebra
    std::cout << t << '\n';
}
```

## Why `ysc::matrix`?

- **Zero overhead.** Storage is `std::array<T, N>` — no heap, no virtual, no indirection.
  Benchmarked against raw `std::array` on every hot path.
- **`constexpr` end to end.** `dot`, `matmul`, `transpose`, reductions — evaluable inside
  `static_assert`. Move invariants from your test suite into the build itself.
- **Dimensions in the type.** `matrix<T, M, N>` × `matrix<T, N, P>` → compile-time inner-dim
  check; mismatched shapes never reach runtime.
- **STL-compatible.** Iterators, `size()`, `data()`, `front`/`back`, `fill`, `swap`, `==`,
  `<=>`, `std::hash`, `std::formatter`.
- **Zero-copy views.** `row()`, `col()`, `slice()`, `reshape()`, `flatten()` return
  `matrix_view` — pointer + static shape, const-correct by type.
- **Header-only.** Drop `matrix.hpp` into your project, or consume via CMake.

## Documentation

- [**API reference**](https://yscialom.github.io/matrix/) — full Doxygen, hosted on GitHub Pages
- [**Cookbook**](https://yscialom.github.io/matrix/cookbook.html) — six task-oriented recipes
- [**Migration guide**](doc/migration.md) — v0.x → v1.0.0 breaking changes

## Installation

### CMake `FetchContent` (recommended)

```cmake
include(FetchContent)
FetchContent_Declare(ysc-matrix
    GIT_REPOSITORY https://github.com/yscialom/matrix.git
    GIT_TAG        v1.0.0)
FetchContent_MakeAvailable(ysc-matrix)

target_link_libraries(my_target PRIVATE ysc::matrix)
```

Tests and documentation auto-disable when the library is consumed as a subproject.

### CMake `find_package` (system install)

```bash
cmake -S path/to/ysc-matrix -B build
cmake --install build --prefix /usr/local
```

```cmake
find_package(ysc-matrix 1.0 CONFIG REQUIRED)
target_link_libraries(my_target PRIVATE ysc::matrix)
```

### Single header (manual)

Download `matrix.hpp` from a [GitHub Release](https://github.com/yscialom/matrix/releases) and
`#include` it.

## Requirements

- **C++20** compiler — GCC ≥ 12, Clang ≥ 15, MSVC ≥ 19.30 (VS 2022), Apple Clang ≥ 14
- **CMake** ≥ 3.20 (only if consumed via CMake)
- **No runtime dependencies.** Header-only, no link step.

## Stability & Semantic Versioning

Starting with v1.0.0, `ysc::matrix` follows [Semantic Versioning](https://semver.org/):

- **Public API** = everything in namespace `ysc` *except* `ysc::detail::`.
  Breaking changes to the public API require a **major version bump**.
- **`ysc::detail::`** is internal. It may change in any release, including patch releases.
  Do **not** depend on it directly.

## Building from source

```bash
cmake -S . -B build
cmake --build build --target check
```

See [CONTRIBUTING.md](CONTRIBUTING.md) for the full development guide.

## Upgrading from v0.x

See the [Migration Guide](doc/migration.md) for breaking changes and upgrade instructions.
