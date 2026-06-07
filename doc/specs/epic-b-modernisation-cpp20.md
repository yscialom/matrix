# EPIC B — C++20 Modernisation

| US | Title | Priority | Status |
|----|-------|----------|--------|
| US-008 | Bump C++17 → C++20 | P0 | ✅ Done |
| US-009 | Replace SFINAE with Concepts | P1 | ✅ Done |
| US-010 | Spaceship operator `<=>` (merged with US-019) | P1 | ✅ Done |

---

## US-008 — Bump C++17 → C++20

**Priority:** P0 — **Depends on:** US-001 — **Blocks:** US-009, US-010

### Specification
- Root `CMakeLists.txt`: `set(CMAKE_CXX_STANDARD 20)` + `set(CMAKE_CXX_STANDARD_REQUIRED ON)` + `set(CMAKE_CXX_EXTENSIONS OFF)`
- Bump `cmake_minimum_required(VERSION 3.20)`
- Verify that the CI matrix tolerates C++20 (update US-001 if needed — Apple Clang 14 minimum)
- No code changes at this stage — just the standard upgrade

### Acceptance criteria
- [ ] CI green on all target compilers
- [ ] `__cplusplus >= 202002L` documented in README

---

## US-009 — Replace SFINAE with Concepts

**Priority:** P1 — **Depends on:** US-008

### Specification
- Introduce in `matrix.hpp`:
  ```cpp
  template<class T, class U>
  concept matrix_convertible_from = std::convertible_to<U, T>;

  template<class... Coords>
  concept integral_coordinates = (std::integral<std::remove_cvref_t<Coords>> && ...);
  ```
- Constrain the templates:
  - `operator()` and `at()`: `requires integral_coordinates<Coords...>`
  - Templated constructors/assignments: `requires matrix_convertible_from<T, U>`
- This produces readable error messages instead of SFINAE

### Acceptance criteria
- [ ] Compiling `matrix<int,3> m; m(1.5);` produces a readable message (not a wall of templates)
- [ ] Existing tests remain green
- [ ] New test `concepts.cpp`: `static_assert` on the concepts

---

## US-010 — Spaceship operator `<=>`

**Priority:** P1 — **Depends on:** US-008, US-019 (grouped)

### Note
This US is merged with US-019 (comparison) to avoid duplication.
