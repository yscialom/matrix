# EPIC I — Packaging & v1.0.0 preparation

| US | Title | Priority | Status |
|----|-------|----------|--------|
| US-038 | Zero-dimension edge case | P2 | ✅ Done |
| US-040 | Enriched `examples/` directory | P1 | ✅ Done |
| US-041 | 100% coverage gate | P1 | ✅ Done |
| US-042 | Tag `v1.0.0` | P0 (final) | ⬜ To do |
| US-043 | Complete Doxygen documentation | P1 | ✅ Done |
| US-045 | CMake packaging: `ysc-matrix` target, alias, install, find_package | P0 | ✅ Done |
| US-046 | Doc fixes + `.gitignore` | P0 | ✅ Done |
| US-047 | README marketing + `mainpage.md` v1 | P0 | ✅ Done |
| US-048 | CI consumer test job | P0 | ✅ Done |
| US-049 | CI auto-generated amalgamation | P0 | ✅ Done |

> US-044 (`matrix(matrix_view)`) is attached to [EPIC H — Views & reshape](epic-h-vues-reshape.md).

---

## US-038 — Zero-dimension edge case

**Priority:** P2 — **Depends on:** US-008

### Specification
- `matrix<int, 0>` or `matrix<int, 2, 0, 3>` must compile
- `linear_size == 0`; `_data` is `std::array<T, 0>`
- `front()`, `back()` become UB → mark `[[deprecated]]` when `empty() == true`? **Decision:** no, conformant with `std::array`. Clear documentation.
- `fill()` is a no-op
- Iterators: `begin() == end()`

### Acceptance criteria
- [ ] Compiles without warning
- [ ] Dedicated tests `empty_matrix.cpp`

---

## US-040 — Enriched `examples/` directory

**Priority:** P1 — **Depends on:** US-045, US-053, US-054, US-056, US-060, US-061, US-062, US-063, US-069 — **Epic:** I

### Story
As a C++ developer discovering the library, I want to find on one hand progressive, compilable examples covering **all** major features (EPIC A-K), and on the other hand showcase examples crossing these features on real use-cases, so I can quickly judge whether `ysc::matrix` meets my needs.

### Technical specification

Directory `examples/` with **10 files** organised into two groups.

#### Group 1 — Progressive topical examples (feature coverage)

- `01_basics.cpp` — construction, access, iteration.
  - Must include: `operator()` / `at()`, detailed exception message from `at()` (US-056), construction from `std::array` and `std::span` (US-053), linear and multi-index `generate` (US-053 + US-069), `rows()` / `cols()` (US-054).
- `02_arithmetic.cpp` — element-wise + scalar + Hadamard + unary.
  - Must include: unary `operator-()` `constexpr` (US-059), bitwise operators `&` / `|` / `^` / `~` / `<<=` / `>>=` on a `matrix<unsigned, …>` (US-063).
- `03_views.cpp` — `slice`, `row`, `col`, `reshape`, `flatten`, **N-D `submatrix`** (US-061), view composition (US-052), view const-correctness (`matrix_view<const T, …>` vs `matrix_view<T, …>`).
- `04_algorithms.cpp` — `apply`, `map`, scalar reductions (`sum`, `min`, `max`, `all`, `any`), **axis reductions** (`sum<0>()`, `min<1>()`, etc. — US-060), **`enumerate()`** for coordinate-value iteration (US-062).
- `05_interop_stl.cpp` — `std::ranges`, `std::sort` on `flatten()`, `std::format`, `std::unordered_set<matrix<…>>` (via `std::hash`), `std::span` interop.
- `06_linear_algebra.cpp` — `dot`, `transpose`, `matmul` matrix × matrix **and matrix × 1D vector** (US-054), illustrated by an Ax=b solution via normal equations.

#### Group 2 — Showcase examples (cross-cutting, use-case oriented)

Each showcase is a standalone programme that produces readable output and demonstrates the value of `ysc::matrix` on a concrete domain.

- `07_game_of_life.cpp` — Conway's automaton on an 8×8 grid, ~10 animated generations in the console.
  - Uses: `enumerate()` (US-062), `submatrix<3, 3>` for the Moore neighbourhood (US-061), `map` for the transition, `sum()` on sub-blocks, `operator<<` for display.
  - Showcases the **`enumerate` + `submatrix`** combination as a 2D stencil tool.
- `08_kalman_filter_1d.cpp` — Constant-velocity Kalman filter (position + velocity state) over 20 noisy measurements.
  - Uses: `constexpr` matrices F / H / Q / R, `matmul` matrix × matrix and matrix × vector (US-054), `transpose`, `identity<…>()`, `zeros<…>()`, `dot`, scalar arithmetic.
  - Showcases the **robotics / sensor fusion** domain; shows that all the necessary algebra fits in `<matrix.hpp>`.
- `09_sobel_edge_detection.cpp` — Sobel edge detector on a small synthetic 16×16 image.
  - Uses: `constexpr` kernels Gx and Gy = `transpose(Gx)`, `submatrix<3, 3>` for the neighbourhood (US-061), element-wise products, `sum()` for convolution, `map` for the magnitude, `std::format` for ASCII-art display.
  - Showcases the **image processing** domain; shows that classic kernels can be declared at compile time.
- `10_pagerank.cpp` — Power iteration on a small directed graph with 6 nodes.
  - Uses: multi-index `generate` to build the transition matrix (US-069), `matmul` matrix × vector (US-054), `transpose`, `sum` + `apply` for L1 normalisation, `dot` for convergence test, `std::format` for final ranking display.
  - Showcases the **graph algorithms / numerical linear algebra** domain.

#### Constraints common to all examples

- Each file is a standalone programme compilable independently, with its own `main()` producing useful textual output.
- Compilation without warning under `-Wall -Wextra -Wpedantic` (examples comply with the transverse DoD like all repo code: clang-format and clang-tidy green).
- No external dependency (no Eigen, no stb_image, etc.) — only the STL and `ysc::matrix`.
- Header included via `#include <matrix.hpp>` ("consumer" form as in the doc and the consumer-test job US-048), not a relative path.
- Two-digit numbering (`01_` to `10_`) to preserve lexicographic order.

#### Build & CMake options

- CMake option `YSC_MATRIX_BUILD_EXAMPLES=OFF` by default (`YSC_MATRIX_` prefix aligned with current usage).
- When `ON`, each `.cpp` produces an executable `example-NN-name` (distinct CMake target).
- Examples compile against the in-tree `ysc::matrix` target (not via `find_package`) — the consumer-test job (US-048) remains solely responsible for validating integration via packaging.

#### CI job

- New job `examples` (in `.github/workflows/ci.yml` or a separate `examples.yml`), triggered **only on `push: branches: [develop]`** — not on `pull_request`.
  - Rationale: examples change rarely, the PR CI budget is already saturated by the 30+ existing jobs; any eventual regression is detected at merge and corrected in a short hotfix.
- The job compiles examples with `YSC_MATRIX_BUILD_EXAMPLES=ON` on **a single** Ubuntu/GCC-13/Release runner (no full matrix — one compiler is sufficient for this non-regression gate).
- Each example is registered as a `ctest` test that verifies only that the executable terminates with exit code 0 (no output comparison — the showcases contain floating-point values and seeded randomness).

### Acceptance criteria
- [ ] Directory `examples/` contains the 10 files (`01`-`06` topical + `07`-`10` showcases) listed above.
- [ ] Each example compiles and runs without error (return code 0) on the CI reference config (Ubuntu/GCC-13/Release).
- [ ] No warning under `-Wall -Wextra -Wpedantic`, no clang-format warning, no clang-tidy warning (transverse DoD).
- [ ] CMake option `YSC_MATRIX_BUILD_EXAMPLES=OFF` by default; `-DYSC_MATRIX_BUILD_EXAMPLES=ON` activates the 10 targets.
- [ ] CI job `examples` triggered only on `push: branches: [develop]`, green on develop.
- [ ] The 10 files collectively cover, at minimum, all Doxygen groups: `ysc_construction`, `ysc_iterators`, `ysc_capacity`, `ysc_modifiers`, `ysc_enumerate`, `ysc_access`, `ysc_comparison`, `ysc_arithmetic`, `ysc_algorithms`, `ysc_linalg`, `ysc_io`, `ysc_hash`, `ysc_views`.

---

## US-041 — 100% coverage gate

**Priority:** P1 — **Depends on:** US-002, all functional US — **Status:** ✅ Done

### Specification

The coverage gate is enforced by the Codecov `codecov/patch` check status (target: 100% of diff lines, 100% of the project), automatically posted on each PR. This check is visible in the GitHub interface and constitutes the de facto gate: any coverage regression turns the check red, signalling to the maintainer not to merge.

Current coverage on `develop` is **100.0%** (lines and patch).

### Acceptance criteria

- [x] `codecov/patch` check red if diff coverage < 100% (enforced server-side by Codecov)
- [x] Project coverage `develop` at 100.0% (verified via Codecov: 479/479 lines)

---

## US-042 — Tag `v1.0.0`

**Priority:** P0 (final) — **Depends on:** all P0 and P1 US — **Epic:** I

### Story
As a maintainer, I want to bump the version to `v1.0.0` once all P0 and P1 user stories are satisfactory, by manually triggering the tag after a subjective review.

### Technical specification
- Bump version `CMakeLists.txt`: MAJOR=1, MINOR=0, PATCH=0
- CHANGELOG.md generated (US-055) and up to date
- Tag `v1.0.0` created manually by Yankel on `develop` after review
- Release workflow (US-007) triggered → GitHub release published with `matrix-amalgamated.hpp` (US-049)
- Documentation up to date, badges green

### Acceptance criteria
- [ ] Release `v1.0.0` visible on GitHub with asset `matrix-amalgamated.hpp`
- [ ] CHANGELOG.md complete
- [ ] CI, Codecov, Docs badges all green
- [ ] `find_package(ysc-matrix 1.0 CONFIG REQUIRED)` works (US-045)

---

## US-043 — Complete Doxygen documentation of the public API

**Priority:** P1 — **Depends on:** US-006, US-023 (all API US already merged) — **Blocks:** US-042 — **Epic:** I

### Story
As a library user, I want every public function to be documented with Doxygen (description, parameters, code example) and accessible in ≤ 2 clicks from the main documentation page.

### Technical specification

**Content of each documentation entry:**
- `@brief` — one sentence describing the behaviour
- `@tparam` for each template parameter (if applicable)
- `@param[in]`/`@param[out]` for each function parameter (if applicable)
- `@return` if the return value is non-void
- `@throws` if an exception may be raised
- A compilable example in `@code`…`@endcode`

**Doxyfile options:**
- `GENERATE_TREEVIEW = YES` (side navigation)
- `USE_MATHJAX = YES` (LaTeX formulae in the doc)
- `WARN_AS_ERROR = YES` so that every warning becomes an error

**Organisation by groups (`@defgroup` / `@ingroup`) — organised by use-case:**

| Group | Content |
|--------|---------|
| `ysc_construction` | Constructors, `operator=`, factories: `zeros`, `full`, `ones`, `identity`, `generate`; `matrix(std::array)`, `matrix(std::span)` |
| `ysc_access` | `operator()`, `at()` |
| `ysc_iterators` | `begin`, `end`, `cbegin`, `cend`, `rbegin`, `rend`, `crbegin`, `crend` and const variants |
| `ysc_capacity` | `size`, `max_size`, `empty`, `data`, `order`, `dimensions` |
| `ysc_modifiers` | `fill`, `swap` (member and friend), `front`, `back` |
| `ysc_comparison` | `operator==`, `operator<=>` |
| `ysc_arithmetic` | `operator+`, `-`, `*`, `/`, scalars, Hadamard, unary |
| `ysc_algorithms` | `apply`, `map`, `sum`, `min`, `max`, `all`, `any` |
| `ysc_linalg` | `transpose`, `matmul`, `dot` |
| `ysc_views` | `slice`, `row`, `col`, `rows`, `cols`, `reshape`, `flatten`; `matrix_view` (class), `contiguous`, `strided`, `all_t`, `all`, `const_matrix_view`; views on `matrix_view`: `row`, `col`, `fill`, `front`, `back` |
| `ysc_io` | `operator<<` (matrix + matrix_view contiguous), `std::formatter` (matrix + matrix_view contiguous) |
| `ysc_hash` | `std::hash<ysc::matrix<T, D...>>` |

**Main page (`@mainpage`):**
- File `doc/mainpage.md`
- Short library description, mention of `ysc::all` (sentinel) in the Views & slicing group
- Table listing the 12 groups with links (`@ref`)
- Complete "Quick Start" example
- Prominent link to Cookbook (US-050) and `examples/`
- "Non-goals" section: broadcasting, SIMD/blocking, dynamic dimensions, constructor from runtime `initializer_list`

**CI job:**
- Extend the `docs` job (US-006) with `WARN_AS_ERROR = YES` in CI
- Or add a dedicated `doxygen-check` step (without deployment)

### Acceptance criteria
- [x] Every public function in `matrix.hpp`, `matrix_view.hpp`, `matrix_detail.hpp` has `@brief`, `@tparam`/`@param`/`@return`/`@throws` as applicable, and a `@code`…`@endcode` example
- [x] All functions are attached to one of the 12 groups above via `@ingroup`
- [x] The `@mainpage` page lists the 12 groups; each group is accessible in 1 click from the main page
- [x] `cmake --build build --target doc` produces zero Doxygen warnings
- [x] CI is red if a Doxygen warning is introduced (`WARN_AS_ERROR = YES`)
- [x] `GENERATE_TREEVIEW = YES` and `USE_MATHJAX = YES` active in `Doxyfile.in`

---

## US-045 — CMake packaging: `ysc-matrix` target, alias, install, `find_package`

**Priority:** P0 — **Depends on:** nothing — **Blocks:** US-047, US-048, US-049 — **Epic:** I

### Story
As a user, I want to be able to integrate `ysc::matrix` into my project via `find_package(ysc-matrix CONFIG REQUIRED)` or `FetchContent`, with the CMake target `ysc::matrix` correctly named and C++20 features propagated.

### Technical specification
- `src/CMakeLists.txt`: rename target `matrix` → `ysc-matrix`, add `add_library(ysc::matrix ALIAS ysc-matrix)`, add `target_compile_features(ysc-matrix INTERFACE cxx_std_20)`
- Root `CMakeLists.txt`: `project(ysc-matrix VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} LANGUAGES CXX)`
- Consumer-only mode (auto-disable when the lib is a `FetchContent` sub-project):
  ```cmake
  if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
      set(YSC_MATRIX_IS_TOP_LEVEL ON)
  endif()
  option(YSC_MATRIX_BUILD_TESTING   "Build tests"         ${YSC_MATRIX_IS_TOP_LEVEL})
  option(YSC_MATRIX_BUILD_DOCUMENTATION "Build Doxygen doc" ${YSC_MATRIX_IS_TOP_LEVEL})
  option(YSC_MATRIX_BUILD_EXAMPLES  "Build examples"      OFF)
  ```
- Generate and install `ysc-matrixConfig.cmake` + `ysc-matrixConfigVersion.cmake` via `CMakePackageConfigHelpers`
- `install(TARGETS ysc-matrix EXPORT ysc-matrixTargets)` + `install(FILES ...)` for the 3 headers (`matrix.hpp`, `matrix_view.hpp`, `matrix_detail.hpp`) into `include/ysc/`
- Document in README: `find_package(ysc-matrix CONFIG REQUIRED)` + `target_link_libraries(my_target PRIVATE ysc::matrix)`

### Acceptance criteria
- [ ] `target_link_libraries(my_target PRIVATE ysc::matrix)` works via `FetchContent`
- [ ] `cmake --install build && find_package(ysc-matrix CONFIG REQUIRED)` works
- [ ] `find_package(ysc-matrix 0.7 CONFIG REQUIRED)` checks the version constraint
- [ ] When consumed via `FetchContent`, ysc::matrix tests/docs do NOT trigger in the host project
- [ ] README updated with both integration methods

---

## US-046 — Documentation fixes and `.gitignore`

**Priority:** P0 — **Depends on:** nothing — **Epic:** I

### Story
As a contributor, I want the release documentation and `.gitignore` to be consistent with the actual state of the repo.

### Technical specification
- `CONTRIBUTING.md`: replace `./release.sh prepare M.m.p` with `./.github/github-release.sh prepare M.m.p` (and `finalize`)
- `.gitignore`: replace `/build` with `/build*/`
- `doc/Doxyfile.in`: remove or correct `EXAMPLE_PATH = @PROJECT_SOURCE_DIR@/doc/sample` (non-existent directory → Doxygen warning)

### Acceptance criteria
- [ ] `./.github/github-release.sh prepare X.Y.Z` is the command documented in `CONTRIBUTING.md`
- [ ] `build-cov/` and other `build*/` no longer appear as untracked in `git status`
- [ ] `cmake --build build --target doc` produces no warning about `EXAMPLE_PATH`

---

## US-047 — README and `mainpage.md`: v1 marketing positioning

**Priority:** P0 — **Depends on:** US-045 (blocking for FetchContent/find_package) — **Blocks:** US-042 — **Status:** ✅ Done — **Epic:** I

### Story
As a C++ developer discovering the project on GitHub, I want to understand in less than 30 seconds whether `ysc::matrix` is right for me.

### Technical specification (implemented)

**README structure:**
1. Title + marketing description ("header-only C++20, zero-overhead, STL-compatible") + badges (CI, codecov, docs)
2. "Quick Start" section — code block ~15 lines (construction, `operator()`, `at()`, compile-time metadata, range-for)
3. "Features" section — table of key features
4. "Installation" section — `FetchContent` (CMake) + manual installation
5. Link to API reference (Doxygen) + Cookbook
6. "Building and Testing" section

**`mainpage.md`:**
- Marketing description equivalent to the README
- "Quick Start" section with code block
- "Features" section in a table
- "Installation" section
- "Cookbook" section with prominent link
- "API Reference" section listing all Doxygen groups by use-case

### Acceptance criteria
- [x] README presents the project at the top with a clear marketing description (header-only C++20, zero-overhead, STL-native)
- [x] README contains a functional Quick Start (~15 lines of code)
- [x] README contains a features table
- [x] README contains an Installation section with FetchContent
- [x] README contains links to the Doxygen documentation and the Cookbook
- [x] `mainpage.md` contains the same marketing description, Quick Start, Features and Installation
- [x] `mainpage.md` lists all Doxygen groups by use-case (API Reference)
- [x] `mainpage.md` contains a prominent link to the Cookbook
- [x] All README snippets compile (verified manually)
- [x] CI `doc` green (`WARN_AS_ERROR = YES` since US-043)

---

## US-048 — CI "consumer test" job

**Priority:** P0 — **Depends on:** US-045 — **Epic:** I

### Story
As a maintainer, I want a CI job to automatically verify that an external project can consume `ysc::matrix` via `FetchContent`.

### Technical specification
- New job `consumer-test` in `.github/workflows/ci.yml`
- Creates a temporary directory `consumer_test/` with a minimal `CMakeLists.txt`
- `FetchContent_Declare` the lib via the current commit SHA (`github.sha`)
- Target `consumer`: programme including `<matrix.hpp>`, constructing `matrix<int,2,3>`, accessing `m(0,0)`
- `cmake configure + build + ctest` on Ubuntu/GCC only
- Directory `consumer_test/` added to `.gitignore`

### Acceptance criteria
- [ ] Job `consumer-test` green on `develop`
- [ ] The job fails if the `ysc::matrix` alias is absent or misconfigured
- [ ] The job fails if `cxx_std_20` is not propagated and the consumer uses C++20 concepts

---

## US-049 — CI auto-generated amalgamation

**Priority:** P0 — **Depends on:** US-007 — **Blocks:** US-042 — **Epic:** I

### Story
As a user wanting minimal integration, I want to be able to download a single header file `matrix-amalgamated.hpp` from the GitHub release.

### Technical specification
- Script `utils/amalgamate.py` versioned in the repo:
  - Concatenates `matrix_detail.hpp` + `matrix_view.hpp` + `matrix.hpp` (in that order)
  - Removes internal `#include` directives (`#include <matrix_detail.hpp>`, `#include <matrix_view.hpp>`)
  - Retains all system `#include` directives (`<algorithm>`, `<array>`, etc.)
  - Wraps each section in a tag comment (`// === BEGIN matrix_detail.hpp ===`)
- Result: `matrix-amalgamated.hpp` — drop-in, `#include <matrix-amalgamated.hpp>` suffices
- The `release.yml` workflow invokes `python3 utils/amalgamate.py -o matrix-amalgamated.hpp` and attaches the file as a release asset
- The old `matrix.hpp.gz` (incomplete, does not contain all 3 headers) is replaced in the assets
- **No** amalgamated file checked into the repo (added to `.gitignore`)

### Acceptance criteria
- [ ] `python3 utils/amalgamate.py` produces a `matrix-amalgamated.hpp` file
- [ ] `g++ -std=c++20 -x c++ matrix-amalgamated.hpp /dev/null` compiles without error
- [ ] CI `release` job attaches `matrix-amalgamated.hpp` as a release asset
- [ ] The old incomplete `matrix.hpp.gz` is replaced or removed from the assets
