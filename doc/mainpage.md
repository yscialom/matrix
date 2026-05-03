@mainpage ysc::matrix — Multi-dimensional Array Library

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

Copy `src/include/matrix.hpp` into your project and add its directory to your include path.

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

## API Reference

The main entry point is the @ref ysc::matrix class.

Related types and concepts:
- @ref ysc::matrix_zero_t — tag type for explicit zero-initialization (`ysc::zero`)
- @ref ysc::integral_coordinates — concept constraining `operator()` and `at()` coordinates
- @ref ysc::matrix_convertible_from — concept constraining converting constructors and assignments
