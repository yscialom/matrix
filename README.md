# `ysc::matrix`

[![CI](https://github.com/yscialom/matrix/actions/workflows/ci.yml/badge.svg)](https://github.com/yscialom/matrix/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/yscialom/matrix/graph/badge.svg)](https://codecov.io/gh/yscialom/matrix)
[![docs](https://github.com/yscialom/matrix/actions/workflows/docs.yml/badge.svg)](https://yscialom.github.io/matrix/)

A **header-only C++20** template library providing a general-purpose multi-dimensional
container with static dimensions. Inspired by `std::array`, it extends it to N dimensions
while keeping the same zero-overhead, `constexpr`-friendly design.

Full API reference: [yscialom.github.io/matrix](https://yscialom.github.io/matrix/) —
[Cookbook](https://yscialom.github.io/matrix/cookbook.html)

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

Copy `src/include/matrix.hpp` into your project and add its directory to your include path.

---

## Quick Start

```cpp
#include <matrix.hpp>
#include <iostream>

int main()
{
    // A 3×3 matrix of integers, aggregate-initialized
    ysc::matrix<int, 3, 3> m = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    // Unchecked element access (performance path)
    std::cout << m(1, 1) << '\n';  // 5

    // Bounds-checked access (throws std::out_of_range if out of bounds)
    std::cout << m.at(2, 2) << '\n';  // 9

    // Compile-time metadata
    static_assert(m.order == 2);
    static_assert(m.dimensions[0] == 3);
    static_assert(m.size() == 9);

    // Range-based for loop (row-major order)
    for (int& x : m)
        x *= 2;

    // Works with any type, any number of dimensions
    ysc::matrix<double, 2, 3, 4> tensor;  // 3D, 24 elements
}
```

---

## Features

- **N-dimensional** — any number of dimensions, all sizes fixed at compile time
- **STL-compatible** — `begin`/`end`, `size()`, `data()`, `front()`, `back()`, `fill()`, `swap()`
- **Two access policies** — unchecked `operator()` (fast path) and bounds-checked `at()`
- **Aggregate initialization** — `matrix<int, 2, 3> m = {1, 2, 3, 4, 5, 6};`
- **Type conversion** — constructors and assignment from `matrix<U, Dims...>` when `U` converts to `T`
- **Comparison** — `==` and `<=>` (lexicographic, row-major order)
- **Compile-time metadata** — `order`, `dimensions`, `size()`, `empty()` all `static constexpr`
- **Zero-init tag** — `matrix<T, Dims...> m{ysc::zero};` for explicit zero-initialization
- **Zero overhead** — storage is `std::array<T, N>`, no heap, no virtual dispatch, no indirection

---

## Building and Testing

```bash
cmake -S . -B build
cmake --build build --target check
```

See [CONTRIBUTING.md](CONTRIBUTING.md) for the full development guide.
