# EPIC J — Ergonomics & polish

| US | Title | Priority | Status |
|----|-------|----------|--------|
| US-039 | Benchmark suite (Google Benchmark) | P1 | ✅ Done |
| US-050 | Doxygen Cookbook | P1 | ✅ Done |
| US-051 | `matrix_view`: strided iterators + `front`/`back`/`fill` | P1 | ✅ Done |
| US-052 | `matrix_view`: I/O, const ctor, composable views | P1 | ✅ Done |
| US-053 | Additional constructors: `std::array`, `std::span`, generator | P1 | ✅ Done |
| US-054 | `matrix::rows()` / `cols()` + 1D vector `matmul` | P1 | ✅ Done |
| US-055 | Versioned `CHANGELOG.md` | P1 | ✅ Done |
| US-056 | Detailed exception messages in `at()` | P1 | ✅ Done |
| US-057 | Centralise `NOLINTNEXTLINE` in `matrix.hpp` | P1 | ✅ Done |
| US-058 | Optimise `matrix(matrix_view<strided>)` | P1 | ✅ Done |
| US-059 | `constexpr` `operator-()` + 64-bit hash combine | P1 | ✅ Done |

---

## US-039 — Benchmark suite (Google Benchmark)

**Priority:** P1 — **Depends on:** US-026, US-031, US-033 — **Epic:** J *(moved from EPIC I)*

### Specification
- New directory `bench/`
- Dependency via `FetchContent`: `google/benchmark`
- Benchmarks:
  - Construction (zeros vs default)
  - Element access (operator() vs at)
  - Iteration (range-for vs index)
  - Arithmetic (m+m, m*m Hadamard, matmul)
  - Comparison vs raw `std::array`
- Optional CI job `benchmark` (triggered manually via `workflow_dispatch`)
- Results archived as artefact

### Acceptance criteria
- [ ] `cmake --build build --target bench && ./build/bench/matrix-bench` works
- [ ] No regression vs raw `std::array` on equivalent operations (overhead < 5%)

---

## US-050 — Doxygen Cookbook

**Priority:** P1 — **Depends on:** US-043 — **Epic:** J

### Story
As a user, I want to find practical recipes for common operations without having to read the full API.

### Technical specification
- New file `doc/cookbook.md` declared as a Doxygen page (`\page cookbook Cookbook`)
- Minimum recipes:
  - "Iterating over rows and columns"
  - "Filling and transforming a matrix"
  - "Comparing matrices element-wise vs lexicographic"
  - "Working with views: contiguous vs strided"
  - "Interop with `std::ranges`, `std::format`, `std::hash`"
  - "Solving Ax=b with dot, transpose, matmul"
- `doc/Doxyfile.in`: enable `GENERATE_TREEVIEW = YES` and `USE_MATHJAX = YES`
- Prominent link to the Cookbook from `mainpage.md` and `README.md`

### Acceptance criteria
- [ ] `cmake --build build --target doc` includes the Cookbook page
- [ ] Each recipe contains a compilable code example (verified manually)
- [ ] The Cookbook is accessible in 1 click from the `@mainpage`
- [ ] `GENERATE_TREEVIEW = YES` and `USE_MATHJAX = YES` active in `Doxyfile.in`

---

## US-051 — `matrix_view`: strided iterators, `front`/`back`/`fill`

**Priority:** P1 — **Depends on:** US-036 — **Blocks:** US-052, US-058 — **Epic:** J

### Story
As a user, I want to iterate over a `matrix_view<T, strided, N>` with STL algorithms (`std::ranges`, `std::accumulate`) and access `front()`/`back()`.

### Technical specification
- Specialisation `matrix_view<T, strided, N>` (1D):
  - Add a `strided_iterator` (random-access, not contiguous): `operator*`, `operator++`, `operator--`, `operator+(n)`, `operator-(n)`, `operator-(it)`
  - Members: `begin()`, `end()`, `cbegin()`, `cend()`, `size()`, `front()`, `back()`, `fill(value)`
- For N-D strided views (N > 1): make `front()` and `back()` available (`(*this)(0,...,0)` and `(*this)(last,...,last)`)
- `fill()` on strided: loop over indices, no linear buffer access

### Acceptance criteria
- [x] `for (auto& v : col_view)` compiles and works for `col_view` from `m.col(j)`
- [x] `std::accumulate(v.begin(), v.end(), 0)` works on a 1D strided view
- [x] `std::ranges::sort(v)` compiles (random-access iterator)
- [x] `v.fill(42)` works on a strided view
- [x] `v.front()` and `v.back()` available on any strided view

---

## US-052 — `matrix_view`: I/O, const ctor, composable views

**Priority:** P1 — **Depends on:** US-051 — **Epic:** J

### Story
As a user, I want to display a `matrix_view`, create a read-only view from a `const matrix&`, and chain slicing calls on a view.

### Technical specification
- `operator<<` for `matrix_view<T, contiguous, ...>` (delegates to helper `detail::print_recursive`)
- `std::formatter` for `matrix_view<T, contiguous, ...>` (with guard `__cpp_lib_format`)
- Ctor `matrix_view<const T, contiguous, ...>(const matrix<T, ...>&)` — **explicit** conversion, allows creating a read-only view from a `const matrix&`
- Alias: `template<class T, std::size_t... D> using const_matrix_view = matrix_view<const T, contiguous, D...>`
- `slice()`, `row()`, `col()` added on `matrix_view<T, contiguous, ...>` (composability)

### Acceptance criteria
- [ ] `std::cout << m.row(0)` compiles and displays the view
- [ ] `std::format("{}", m.row(0))` compiles (under guard)
- [ ] `const matrix<int,3,3> cm{...}; const_matrix_view<int,3,3> v{cm};` compiles
- [ ] `v.slice(1)` on a `matrix_view<T, contiguous, ...>` returns a new view
- [ ] `const_matrix_view<int,3>` is a valid alias

---

## US-053 — Additional constructors: `std::array`, `std::span`, generator

**Priority:** P1 — **Depends on:** US-021 — **Epic:** J

### Story
As a user, I want to construct a matrix from an existing buffer (`std::array`, `std::span`) or a generator function.

### Technical specification
- `explicit matrix(std::array<T, linear_size> data)` — copy from `std::array`
- `explicit matrix(std::span<const T, linear_size> data)` — copy from span (C++20)
- Free factory:
  ```cpp
  template<class T, std::size_t... D, std::invocable<std::size_t> F>
  constexpr matrix<std::invoke_result_t<F, std::size_t>, D...>
  generate(F f);  // f(linear_index) called for each element
  ```
- Multi-index variant (argument order = dimension order):
  ```cpp
  template<class T, std::size_t... D, class F>
    requires std::invocable<F, decltype((void(D), std::size_t{}))...>
  constexpr matrix<std::invoke_result_t<F, ...>, D...>
  generate(F f);  // f(i0, i1, ...) for N-D matrix
  ```

### Acceptance criteria
- [ ] `matrix<int,3> m(std::array<int,3>{1,2,3})` compiles and works
- [ ] `matrix<int,3> m(std::span<const int,3>{buf, 3})` compiles
- [ ] `auto m = ysc::generate<int,3,3>([](std::size_t i, std::size_t j){ return int(i+j); })` compiles
- [ ] Tests in `test/src/construct_from_buffer.cpp`

---

## US-054 — `matrix::rows()` / `cols()` + 1D vector `matmul`

**Priority:** P1 — **Depends on:** US-036, US-033, US-051 — **Epic:** J

### Story
As a user, I want to iterate over the rows and columns of a 2D matrix as a range, and multiply a 2D matrix by a 1D vector.

### Technical specification
- `m.rows()` (constraint `order == 2`): returns a `std::ranges::view` of `matrix_view<T, contiguous, C>` (one view per row)
- `m.cols()` (constraint `order == 2`): returns a `std::ranges::view` of `matrix_view<T, strided, R>` (one view per column)
- `matmul` overload for 1D vector:
  ```cpp
  template<class T, std::size_t M, std::size_t N>
  constexpr matrix<T, M> matmul(const matrix<T, M, N>& mat, const matrix<T, N>& vec);
  ```

### Acceptance criteria
- [ ] `for (auto row_view : m.rows())` compiles on a `matrix<T, R, C>` (2D)
- [ ] `for (auto col_view : m.cols())` compiles on a `matrix<T, R, C>` (2D)
- [ ] `m.rows()` and `m.cols()` refused at compile time for `order ≠ 2`
- [ ] `matmul(matrix<int,2,3>{...}, matrix<int,3>{...})` returns `matrix<int,2>`
- [ ] Tests in `test/src/rows_cols.cpp` and additional tests in `test/src/matmul.cpp`

---

## US-055 — Versioned `CHANGELOG.md`

**Priority:** P1 — **Depends on:** US-007 — **Epic:** J

### Story
As a user arriving on the GitHub repo, I want to see the history of changes directly in the repository without going to the Releases page.

### Technical specification
- `CHANGELOG.md` initialised at the root with all existing releases (generated via `git-cliff --config cliff.toml`)
- Format: sections `## [v0.x.0] - YYYY-MM-DD` with sub-sections feat / fix / chore
- The `release.yml` workflow updates `CHANGELOG.md` after each release and commits to `develop`:
  ```yaml
  - name: Update CHANGELOG
    run: |
      git-cliff --config cliff.toml --output CHANGELOG.md
      git add CHANGELOG.md
      git commit -m "chore(release): update CHANGELOG for ${{ github.ref_name }}"
      git push origin develop
  ```

### Acceptance criteria
- [x] `CHANGELOG.md` present at the repo root, readable via GitHub
- [x] Contains releases v0.2.0 to v0.6.0 (first releases available in git history)
- [x] `release.yml` workflow updates and commits `CHANGELOG.md` on each new release

---

## US-056 — Detailed exception messages in `at()`

**Priority:** P1 — **Depends on:** nothing — **Epic:** J

### Story
As a debugging developer, I want `matrix::at()` and `matrix_view::at()` to indicate the offending coordinate and the dimension size in the exception message.

### Technical specification
- `matrix::at()` (`matrix.hpp`, two overloads): message format `"matrix::at: coordinate N is out of bounds for dimension K (size=S)"`
- `matrix_view::at()` (`matrix_view.hpp`, two overloads): same format `"matrix_view::at: coordinate N is out of bounds for dimension K (size=S)"`
- Message construction: `std::to_string` (or `std::format` with guard)
- Zero performance impact (only on the exception path)

### Acceptance criteria
- [ ] `try { m.at(99); } catch (const std::out_of_range& e) { ... }` — `e.what()` contains `"99"` and the dimension size
- [ ] Message format documented in `@throws` Doxygen
- [ ] Tests in `test/src/access.cpp`

---

## US-057 — Centralise `NOLINTNEXTLINE` in `matrix.hpp`

**Priority:** P1 — **Depends on:** nothing — **Epic:** J

### Story
As a maintainer, I want to reduce the visual noise of the ~19 clang-tidy suppressions in `matrix.hpp`.

### Technical specification

Two options (choose the one that minimises the diff):

**Option A:** disable `cppcoreguidelines-pro-bounds-pointer-arithmetic` and `cppcoreguidelines-pro-bounds-constant-array-index` in `.clang-tidy` under `src/include/` with a commented justification.

**Option B:** centralise index calculations in `detail::` helpers (already partially done with `coordinates_to_index`, `index_to_coordinates`) and put a single `NOLINTNEXTLINE` per helper.

Expected result: `grep -c NOLINT src/include/matrix.hpp` ≤ 5 (instead of ~19).

### Acceptance criteria
- [ ] `grep -c NOLINT src/include/matrix.hpp` returns ≤ 5
- [ ] No orphan suppression (all justified by a comment)
- [ ] CI clang-tidy remains green

---

## US-058 — Optimise `matrix(matrix_view<strided>)`

**Priority:** P1 — **Depends on:** US-044, US-051 — **Epic:** J

### Story
As a maintainer, I want the `matrix(matrix_view<strided>)` constructor to iterate in O(N) rather than recalculating coordinates via `index_to_coordinates` for each element.

### Technical specification
- Replace the loop `for i in [0, linear_size) → index_to_coordinates(i) → std::apply → operator()` with `std::copy(v.begin(), v.end(), _data.begin())` using the strided iterators from US-051
- The `contiguous` constructor remains unchanged (already optimal via `std::copy`)

### Acceptance criteria
- [ ] The strided ctor uses iterators (no `index_to_coordinates` in a loop)
- [ ] Existing tests `test/src/matrix_from_view.cpp` remain green
- [ ] `constexpr` preserved if the strided iterators are `constexpr`

---

## US-059 — `constexpr` `operator-()` + 64-bit hash combine

**Priority:** P1 — **Depends on:** nothing — **Epic:** J

### Story
As a user, I want the unary `-` operator to be `constexpr` and the hash function to have good distribution on 64-bit platforms.

### Technical specification

**Unary `operator-()`:**
```cpp
[[nodiscard]] constexpr matrix operator-() const
    noexcept(noexcept(-std::declval<T const&>())) {
    return map([](const T& v) { return -v; });
}
```
(Currently without `constexpr` or conditional `noexcept` — `matrix.hpp:858-864`)

**64-bit hash combine:**
Replace in `std::hash<ysc::matrix<T,D...>>::operator()`:
```cpp
// Before (32-bit seed, sub-optimal distribution on 64-bit):
h ^= hasher(v) + 0x9e3779b9 + (h << 6) + (h >> 2);
// After (Knuth 64-bit constant, Boost 1.81+):
h ^= hasher(v) + 0x9E3779B97F4A7C15ULL + (h << 12) + (h >> 4);
```

### Acceptance criteria
- [ ] `static_assert((-matrix<int,2>{1,-1})(0) == -1)` passes (constexpr)
- [ ] `static_assert((-matrix<int,2>{1,-1})(1) == 1)` passes
- [ ] Existing tests `test/src/hash.cpp` remain green
- [ ] `noexcept` of `operator-()` propagates correctly (tested with a type whose `operator-` is noexcept)
