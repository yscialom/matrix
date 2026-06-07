# User Stories — ysc::matrix roadmap

## Dashboard

### View by epic

| Epic | Status | Progress | Detail |
|--------|--------|-------------|--------|
| [**A — Infrastructure & CI/CD**](epic-a-infrastructure.md) | ✅ Complete | 7/7 | ✅ US-001, US-002, US-003, US-004, US-005, US-006, US-007 |
| [**B — C++20 Modernisation**](epic-b-modernisation-cpp20.md) | ✅ Complete | 3/3 | ✅ US-008, US-009, US-010 (merged US-019) |
| [**C — Technical debt**](epic-c-dette-technique.md) | ✅ Complete | 4/4 | ✅ US-011, US-012, US-013, US-014 |
| [**D — STL compliance**](epic-d-conformite-stl.md) | ✅ Complete | 4/4 | ✅ US-015, US-016, US-017, US-018 |
| [**E — Comparison & I/O**](epic-e-comparaison-io.md) | ✅ Complete | 7/7 | ✅ US-019, US-020, US-021, US-022, US-023, US-024, US-025 |
| [**F — Arithmetic**](epic-f-arithmetique.md) | ✅ Complete | 4/4 | ✅ US-026, ✅ US-027, ✅ US-028, ✅ US-029 |
| [**G — Algorithms**](epic-g-algorithmes.md) | ✅ Complete | 5/5 | ✅ US-030, ✅ US-031, ✅ US-032, ✅ US-033, ✅ US-034 |
| [**H — Views & reshape**](epic-h-vues-reshape.md) | ✅ Complete | 4/4 | ✅ US-035, ✅ US-036, ✅ US-037, ✅ US-044 |
| [**I — Packaging & v1.0.0 preparation**](epic-i-packaging.md) | 🔄 In progress | 9/10 | ✅ US-038, ✅ US-040, ✅ US-041, ✅ US-043, ✅ US-045, ✅ US-046, ✅ US-047, ✅ US-048, ✅ US-049, ⬜ US-042 |
| [**J — Ergonomics & polish**](epic-j-ergonomie.md) | ✅ Complete | 11/11 | ✅ US-039, ✅ US-050 to US-059 |
| [**K — Pre-v1 extensions**](epic-k-extensions.md) | ✅ Complete | 10/10 | ✅ US-060 to US-069 |

**Total: 68 / 69 US**

> The per-US table for each epic is in the corresponding file. Update these tables **and** the epic progress counter above whenever a US is closed.

---

## Global objectives

| Axis | Target |
|-----|-------|
| Standard | C++20 (concepts, `<=>`, `consteval`, `std::span`) |
| API | STL-compliant + linear algebra + views |
| Coverage | 100% lines & branches |
| Quality | clang-format, clang-tidy, sanitizers, Doxygen doc published |
| CI/CD | Multi-OS, multi-compiler, automated releases |

## Cross-cutting decisions (apply to all US)

### Tools & target versions
- **Supported compilers:** GCC ≥ 12, Clang ≥ 15, MSVC ≥ 19.30 (VS 2022), Apple Clang ≥ 14
- **CMake:** ≥ 3.20 (FetchContent stable, `target_compile_features(cxx_std_20)`)
- **Tests:** GoogleTest (already in place — no migration to Catch2/doctest)
- **Coverage:** gcov + lcov + Codecov.io
- **Doc:** Doxygen → GitHub Pages (`gh-pages` branch)
- **Format:** clang-format with LLVM-based style (4-space indent, 100 cols)
- **Linter:** clang-tidy with ruleset bugprone-*, modernize-*, performance-*, readability-*

### Code conventions
- Root namespace: `ysc`
- Internal details: `ysc::detail` (rename `_details` → `detail`, US-014)
- snake_case for functions/variables, PascalCase for template types
- Tests in `test/src/<feature>.cpp`; one file per functional US
- Test names: `TEST(MatrixXxx, BehaviorYyy)`

### Memory layout & semantics
- **Row-major** (rightmost dimension contiguous) — RETAINED, do not change
- Storage: `std::array<T, (Dims*...)>` — RETAINED
- No overhead vs C-array: everything must remain `constexpr`-friendly and without dynamic branches on hot paths
- Iterator invalidation policy: never invalidated (except swap which follows the element)

### Error policy
- `operator()`: UB out-of-bounds (perf-critical)
- `at()`: `std::out_of_range`
- Type/size errors: at compile time (concepts/static_assert), never runtime

### PR granularity
- 1 US = 1 PR ≤ 400 lines diff (excluding tests)
- Each PR must leave the build and tests green
- No breaking changes without a major bump

### Definition of Done (DoD) — cross-cutting criteria

A US is considered **Done** when, in addition to its own acceptance criteria:

- Build and tests green (`cmake --build build --target check`)
- No clang-format warnings (`cmake --build build --target format-check`)
- No clang-tidy warnings (`cmake --build build --target lint`)
- **Documentation:** every public function (member or free) added or modified is documented with Doxygen:
  - `@brief` — one-sentence description
  - `@tparam` for each template parameter (if applicable)
  - `@param` for each function parameter (if applicable)
  - `@return` if the return value is non-void
  - A compilable example in `@code`…`@endcode`
- Every new function is attached to an `@ingroup` existing group (or a new `@defgroup` created), so it is reachable from the main Doxygen doc page in ≤ 2 clicks

---

## Epics and dependencies

```
EPIC A — Infrastructure & CI/CD
  US-001 → US-002 → US-003
                  → US-004
                  → US-005
                  → US-006
                  → US-007

EPIC B — C++20 Modernisation
  US-008 (bump standard) → US-009 (concepts) → US-010 (<=>)

EPIC C — Technical debt & cleanup
  US-011, US-012, US-013, US-014 (independent, can be done in parallel)

EPIC D — STL compliance
  US-015 (typedefs) → US-016 (iterators) → US-017 (size/data/empty)
                                         → US-018 (front/back/fill)

EPIC E — Comparison & I/O
  US-019 (==/<=>) → US-020 (hash)
  US-021 (refactor ctor) → US-022 (nested init-list) → US-023 (factories)
  US-024 (ostream) → US-025 (std::format)

EPIC F — Arithmetic
  US-026 (element-wise +/-) → US-027 (element-wise */÷)
  US-028 (scalar) → US-029 (unary)

EPIC G — Algorithms
  US-030 (apply) → US-031 (reductions)
  US-032 (transpose) → US-033 (matmul 2D) → US-034 (dot)

EPIC H — Views & reshape
  US-035 (matrix_view) → US-036 (slice/submatrix) → US-037 (reshape)

EPIC I — Packaging & v1.0.0 preparation
  US-038 (zero-dim edge case)
  US-046 (doc fixes + .gitignore)
  US-045 (CMake packaging) → US-047 (README), US-048 (CI consumer), US-049 (amalgamation)
  US-040 (examples) → depends US-045
  US-041 (100% coverage gate)
  US-043 (Doxygen) → US-050 (cookbook, EPIC J)
  US-047 (README) → US-042 (tag v1.0.0)
  US-049 (amalgamation) → US-042

EPIC J — Ergonomics & polish
  US-039 (benchmarks)
  US-050 (cookbook) → depends US-043
  US-051 (strided iterators) → US-052, US-058
  US-052 (view I/O + composables)
  US-053 (additional constructors)
  US-054 (rows/cols + matmul 1D) → depends US-036, US-033
  US-055 (CHANGELOG)
  US-056 (at() messages)
  US-057 (centralise NOLINT)
  US-058 (optimise strided ctor) → depends US-051
  US-059 (operator-() + hash 64-bit)

EPIC K — Pre-v1 extensions
  US-060 (axis reductions) → depends US-031
  US-061 (submatrix) → depends US-036
  US-062 (enumerate) → depends US-016
  US-063 (bitwise) → depends US-026
  US-064 (ASan dangling view) → depends US-035, US-003
  US-065 (linalg reference) → depends US-033, US-034
  US-066 (CI Windows cache)
  US-067 (repo hygiene)
  US-068 (migration guide) → depends US-042
```
