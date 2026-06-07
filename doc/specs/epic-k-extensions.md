# EPIC K — Pre-v1 Extensions

| US | Title | Priority | Status |
|----|-------|----------|--------|
| US-060 | Axis reductions (`sum<Axis>()`, etc.) | P2 | ✅ Done |
| US-061 | `submatrix`: N-D sub-block extraction | P2 | ✅ Done |
| US-062 | `enumerate()`: coordinate iterator | P2 | ✅ Done |
| US-063 | Bitwise operators for integer types | P2 | ✅ Done |
| US-064 | ASan test: dangling view detection | P2 | ✅ Done |
| US-065 | Linalg reference tests (pre-computed values) | P2 | ✅ Done |
| US-066 | CI Windows: vcpkg cache | P2 | ✅ Done |
| US-067 | Repo hygiene: `.editorconfig`, `SECURITY.md`, `CODE_OF_CONDUCT.md`, Dependabot | P2 | ✅ Done |
| US-068 | Migration guide: SemVer v1.0.0 stability promise | P2 | ✅ Done |
| US-069 | `generate` with multi-index callable | P2 | ✅ Done |

---

## US-060 — Axis reductions (`sum<Axis>()`, etc.)

**Priority:** P2 — **Depends on:** US-031 — **Epic:** K

### Story
As a user, I want to compute the sum, min, and max of a matrix along a given axis, obtaining a lower-dimensional matrix.

### Technical specification
- `template<std::size_t Axis> constexpr auto sum() const -> matrix<T, /* dims without Axis */>`
- Same for `min<Axis>()` and `max<Axis>()`
- Implementation: metaprogramming to deduce the resulting dimensions (remove the `Axis` dimension from the `Dims...` pack)
- Constraint: `static_assert(Axis < order)`

### Acceptance criteria
- [x] `matrix<int,2,3>{{1,2,3},{4,5,6}}.sum<0>()` == `matrix<int,3>{5,7,9}` (column-wise sum)
- [x] `matrix<int,2,3>{{1,2,3},{4,5,6}}.sum<1>()` == `matrix<int,2>{6,15}` (row-wise sum)
- [x] Compilation error if `Axis >= order` (constraint `requires(Axis < order)`)
- [x] Tests in `test/src/reductions_axis.cpp`

---

## US-061 — `submatrix`: N-D sub-block extraction

**Priority:** P2 — **Depends on:** US-036 — **Epic:** K

### Story
As a user working on image processing or convolution kernels, I want to extract a sub-block from an N-D matrix.

### Technical specification
```cpp
template<std::size_t... NewD>
matrix_view<T, strided, NewD...>
submatrix(std::array<std::size_t, order> origin) const;
```
- Compile-time constraint: `sizeof...(NewD) == order`
- Runtime verification: `origin[i] + NewD[i] <= Dims[i]` for each `i`, otherwise `std::out_of_range`
- Returns a strided view (strides computed on the source matrix)

### Acceptance criteria
- [ ] `m.submatrix<2,2>({1,1})` on a `matrix<int,4,4>` returns a correct 2×2 view
- [ ] Mutation via the view is reflected in the source matrix
- [ ] `m.submatrix<3,3>({2,2})` on a `matrix<int,4,4>` raises `std::out_of_range`
- [ ] Tests in `test/src/submatrix.cpp`

---

## US-062 — `enumerate()`: coordinate iterator

**Priority:** P2 — **Depends on:** US-016 — **Epic:** K

### Story
As a user, I want to iterate over the elements of a matrix with their multi-dimensional coordinates (like `np.ndenumerate`).

### Technical specification
- `m.enumerate()`: returns a range of `std::pair<std::array<std::size_t, order>, T&>`
- Row-major traversal (consistent with the linear iterator)
- Implementation: adaptor on the linear iterator, conversion `linear_index → coordinates` via `detail::index_to_coordinates`

### Acceptance criteria
- [ ] `for (auto& [coords, val] : m.enumerate())` compiles
- [ ] `coords` is a correct `std::array<std::size_t, order>` for each element
- [ ] Mutation via `val` is reflected in `m`
- [ ] Tests in `test/src/enumerate.cpp`

---

## US-063 — Bitwise operators for integer types

**Priority:** P2 — **Depends on:** US-026 — **Epic:** K

### Story
As a user working on binary masks (image, cryptography), I want to apply bitwise operators on `matrix<unsigned, ...>`.

### Technical specification
- Member operators: `operator&=`, `operator|=`, `operator^=`
- Scalar operators: `operator<<=`, `operator>>=` (shift by an integer)
- Unary operator: `operator~` (bitwise NOT)
- Binary friend operators: `operator&`, `operator|`, `operator^`
- Constraint: `requires std::integral<T>` on each operator
- Implementation: `std::transform` as for arithmetic operators

### Acceptance criteria
- [ ] `matrix<unsigned,3>{1,2,3} & matrix<unsigned,3>{3,3,3}` compiles and returns `{1,2,3}`
- [ ] `~matrix<unsigned,3>{0,0,0}` returns a matrix of `~0u`
- [ ] Compilation error for `matrix<double,3>` (non-integral)
- [ ] Tests in `test/src/arithmetic_bitwise.cpp`

---

## US-064 — ASan test: dangling view detection

**Priority:** P2 — **Depends on:** US-035, US-003 — **Epic:** K

### Story
As a maintainer, I want a test under ASan to detect use of a `matrix_view` after destruction of the source matrix, to validate the expected behaviour (UB → crash under sanitizer).

### Technical specification
- File `test/src/matrix_view_lifetime.cpp`, compiled only if `YSC_SANITIZERS_ENABLED`
- Scenario: construct a `matrix_view` on a local matrix in a sub-scope, let the matrix be destroyed, access the view → use-after-free detected by ASan
- The test is **intentionally a crash** under ASan (use-after-free). It must be excluded from the normal run and included in a dedicated test under sanitizers
- Use `GTEST_SKIP()` if `!defined(YSC_SANITIZERS_ENABLED)` to not block the normal build

### Acceptance criteria
- [ ] The file compiles with `ENABLE_SANITIZERS=ON`
- [ ] The test is skipped without `ENABLE_SANITIZERS`
- [ ] The UB behaviour is documented in the Doxygen docstring of `matrix_view`

---

## US-065 — Linalg reference tests (pre-computed values)

**Priority:** P2 — **Depends on:** US-033, US-034 — **Epic:** K

### Story
As a maintainer, I want a few sanity tests that compare the numerical results of `matmul` and `dot` against reference values.

### Technical specification
- New file `test/src/linalg_reference.cpp`
- Hard-coded reference values (computed with NumPy or by hand, no runtime dependency)
- Test matrices:
  - `matmul`: 2×2, 2×3×3×2, identity matrix × arbitrary matrix
  - `dot`: 3 pairs of 1D vectors
  - `transpose`: 2×3 → symmetry verification
- No external dependency (NumPy, Eigen) in the tests

### Acceptance criteria
- [ ] `matmul(A, B) == expected_AB` for ≥ 3 matrix pairs
- [ ] `dot(a, b) == expected` for ≥ 3 vector pairs
- [ ] `transpose(transpose(m)) == m` for ≥ 2 matrices
- [ ] All results verified via `static_assert` when `constexpr`

---

## US-066 — CI Windows: vcpkg cache

**Priority:** P2 — **Depends on:** US-001 — **Epic:** K

### Story
As a maintainer, I want the Windows CI not to re-download `vcpkg`/`gtest` on every run in order to speed up builds.

### Technical specification
- Add `actions/cache@v4` on `vcpkg_installed/` (or the vcpkg install directory) in the Windows job
- Key: hash of the vcpkg manifest file or the command `vcpkg install gtest:x64-windows`
- Restore-keys: fallback to a partial cache

### Acceptance criteria
- [ ] Second CI Windows run with identical code: `vcpkg install` step skipped (cache hit displayed)
- [ ] Windows CI time reduced by at least 30 seconds on warm cache

---

## US-067 — Repo hygiene: `.editorconfig`, `SECURITY.md`, `CODE_OF_CONDUCT.md`, Dependabot

**Priority:** P2 — **Depends on:** nothing — **Epic:** K

### Story
As an external contributor, I want the project to follow standard OSS conventions and make contribution easier.

### Technical specification
- `.editorconfig` at the root:
  - `indent_style = space`, `indent_size = 4` for C++ and CMake
  - `indent_size = 2` for YAML, JSON, Markdown
  - `end_of_line = lf`, `charset = utf-8`, `trim_trailing_whitespace = true`, `insert_final_newline = true`
  - Consistent with `.clang-format`
- `SECURITY.md`: instructions for reporting vulnerabilities via GitHub Issue (reduced surface: lib without I/O, header-only)
- `CODE_OF_CONDUCT.md`: Contributor Covenant v2.1 (standard text, adapted to the project name)
- `.github/dependabot.yml`:
  ```yaml
  version: 2
  updates:
    - package-ecosystem: "github-actions"
      directory: "/"
      schedule:
        interval: "monthly"
  ```

### Acceptance criteria
- [ ] `.editorconfig` present and consistent with `.clang-format` (same indent_size)
- [ ] `SECURITY.md` present with clear instructions
- [ ] `CODE_OF_CONDUCT.md` present (Contributor Covenant v2.1)
- [ ] `.github/dependabot.yml` present with `github-actions` config

---

## US-068 — Migration guide: SemVer v1.0.0 stability promise

**Priority:** P2 — **Depends on:** US-042 — **Epic:** K

### Story
As a user adopting the lib from a v0.x version, I want to understand the stability guarantees and breaking changes introduced in v1.0.0.

### Technical specification
- New file `doc/migration.md`
- Content:
  - **SemVer promise since v1.0.0**: public API = everything outside `ysc::detail::`. `ysc::detail::` may change in a patch.
  - **Breaking changes since v0.x:**
    - CMake target rename `matrix` → `ysc-matrix`, alias `ysc::matrix` (US-045)
    - Hash value change (US-059): `std::unordered_set<matrix<...>>` serialised before v1.0.0 are invalidated
  - **Migration instructions** for each change (before/after)
- Link from `README.md` Installation section and from `mainpage.md`

### Acceptance criteria
- [ ] `doc/migration.md` present and readable from the GitHub repo
- [ ] All breaking changes v0.x → v1.0.0 listed with instructions
- [ ] The SemVer promise (public API vs `ysc::detail::`) is explicitly documented
- [ ] Link to `doc/migration.md` from `README.md` and `mainpage.md`

---

## US-069 — `generate` with multi-index callable

**Priority:** P2 — **Depends on:** US-053 — **Epic:** K

### Story
As a user constructing position-dependent matrices (identity by function, triangular, Vandermonde, XY gradient, i*j masks…), I want to pass to `ysc::generate` a callable that receives N-D coordinates rather than a linear index. Currently I have to manually unpack `i = k / cols; j = k % cols;`, which is verbose and fragile in 3D+.

### Technical specification

Add a **second overload** of `ysc::generate` alongside the existing one (`src/include/matrix.hpp:1632-1640`):

```cpp
template <class T, std::size_t... Dims, class F>
    requires std::invocable<F, /* sizeof...(Dims) × std::size_t */>
          && std::convertible_to<
                 std::invoke_result_t<F, /* idem */>, T>
constexpr matrix<T, Dims...> generate(F f);
```

- The existing overload (`std::invocable<F, std::size_t>`, linear row-major index) remains exposed and continues to compile as-is — **full backward compatibility**.
- Implementation: iterate over N-D indices via `detail::index_to_coordinates` (already available in `matrix_detail.hpp`, US-044) and invoke `f` with `std::apply` on the coordinate tuple.
- Doxygen `@brief @tparam @return @code`, group `ysc_factory` (or equivalent existing group).
- The callable may be `auto` (generic C++) or explicitly typed `std::size_t`.

**Edge case to clarify in the implementation**: if `sizeof...(Dims) == 1`, a callable `[](std::size_t k){ ... }` satisfies both `invocable<size_t>` and the new constraint (1 arg). Resolve in favour of the linear overload (backward compat) — for example via `requires(!std::invocable<F, std::size_t>)` on the new overload, or via a stricter concept.

### Acceptance criteria
- [ ] `auto I = ysc::generate<int, 3, 3>([](std::size_t i, std::size_t j){ return i == j ? 1 : 0; });` compiles and produces the 3×3 identity matrix.
- [ ] `static_assert(ysc::generate<int, 2, 2>([](auto i, auto j){ return int(i + j); })(1, 1) == 2);` passes (the overload is `constexpr`).
- [ ] 3D tensor: `auto m = ysc::generate<int, 2, 3, 4>([](auto i, auto j, auto k){ return int(i*100 + j*10 + k); });` compiles and `m(1, 2, 3) == 123`.
- [ ] The historical linear overload (`generate<int, N>([](std::size_t k){ return int(k); })`) compiles without modification.
- [ ] Dedicated test `test/src/generate_multi_index.cpp` covering: 1D (linear vs multi-index equivalent), 2D, 3D, non-trivial type, `constexpr` propagation.
- [ ] Doxygen updated with inline `@code` example.
- [ ] CI green on all platforms; clang-format / clang-tidy clean.
