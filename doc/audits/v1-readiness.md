# v1.0.0 Readiness Audit — Summary

**Closing date:** 2026-06-07
**Overall status:** GREEN — 11 axes / 11 green, no blocker identified for `v1.0.0`.

This cross-cutting audit was conducted after the 68/69 functional user stories (US-042 — tag — excluded). Each axis was the subject of a dedicated session, a `chore/v1-check-NN-<slug>` branch and, most often, a fixes PR. The detailed history of each axis is in the Git history (branches `chore/v1-check-NN-*` and related PRs).

## Summary by axis

| Axis | Subject | PR | Verdict | Fixes delivered |
|----|------|----|--------|--------------------|
| 1 | Public API freeze audit | (integrated) | ✅ Green | F1, F2 |
| 2 | Header self-containment & includes | (integrated) | ✅ Green | 6 direct `#include` added |
| 3 | Doxygen completeness & 0 warning | (integrated) | ✅ Green | `EXCLUDE_SYMBOLS` + 14 `@code` snippets corrected |
| 4 | Documentation cross-check | (integrated) | ✅ Green | typos + `find_package` version + SemVer section |
| 5 | Coverage & pathological types | #140 | ✅ Green | new pathological_types tests |
| 6 | CI matrix audit | (integrated) | ✅ Green | mutable `@main` eliminated + 2 workflow fixes |
| 7 | Sanitizers full sweep | #142 | ✅ Green | examples covered by ASan/UBSan |
| 8 | Lint cleanliness | #143 | ✅ Green | 5 warnings + 105 NOLINT justified |
| 9 | Packaging E2E | #144 | ✅ Green | no fixes required (6/6 scenarios green) |
| 10 | Benchmarks & overhead claim | #145 | ✅ Green | benchmarks completed (fill/swap, small, int) |
| 11 | README adoption overhaul | (in progress) | ✅ Green | README overhaul (Quick Start above-the-fold) |

## Notable fixes (by axis)

- **Axis 1 — F1.** `matrix_view<contiguous>::operator matrix_view<strided>()` made `constexpr`.
- **Axis 1 — F2.** `matrix::enumerate_range` constructor made `private` (+ `friend class matrix;`).
- **Axis 2.** 6 direct `#include` added (`<tuple>`, `<cstddef>`, `<concepts>`, `<type_traits>`, `<initializer_list>`, `<utility>`); the historical `<stdexcept>` bug was already fixed.
- **Axis 3.** `EXCLUDE_SYMBOLS = ysc::detail ysc::detail::*` added to `Doxyfile.in`; 14 `@code` snippets corrected, including a real bug `auto& [coords, val] : m.enumerate()` (the iterator returns a temporary, bind as `auto`).
- **Axis 4.** Typos `Dimentions`/`aa matrix` corrected; `find_package` version 0.7 → 1.0 in README; *Stability & Semantic Versioning* section added to README and `mainpage.md`.
- **Axis 5.** 100% coverage confirmed via Codecov (479/479 lines); `test/src/pathological_types.cpp` added, covering `std::unique_ptr<int>` (move-only), `throwing_copy`, and `no_eq` (`operator==` deleted).
- **Axis 6.** Mutable reference `eclipse-score/apt-install@main` eliminated (7 occurrences, replaced by direct `apt-get`); `docs.yml`: `BUILD_DOCUMENTATION` → `YSC_MATRIX_BUILD_DOCUMENTATION`; `benchmark.yml` runner pinned to `ubuntu-24.04`. Flakiness measured: 0% over 50 runs on `develop`.
- **Axis 7.** CI `sanitizers` job extended to examples (CMake option + flags applied via `ysc_add_example`).
- **Axis 8.** 5 `readability-math-missing-parentheses` warnings corrected (`slice.cpp`, `matrix_from_view.cpp`); 105 NOLINT* enriched with a justification and a named rule.
- **Axis 9.** No fixes: `find_package`, `FetchContent`, alias `ysc::matrix`, amalgamation (4,592 lines, ODR multi-TU OK, `detail::` encapsulated), examples ctest 11/11, CMake 3.20 minimum — all green.
- **Axis 10.** "Zero overhead vs `std::array`" claim confirmed: `operator()` assembly strictly identical to `std::array::operator[]` (`endbr64 + movslq + movl + ret`). Measured ratios ≤ 1.04 for in-place operations. Additions: `bench_fill_swap.cpp` (gap filled), `int` variants and small sizes, `std::array` comparisons.
- **Axis 11.** README overhauled: Quick Start above-the-fold (line 14), *Why `ysc::matrix`?* section listing the USPs, dedicated *Requirements* section, Installation moved to 28% of the height.

## Documented decisions

- **MSan rejected.** Heavy false positives without an instrumented libc++; CI infrastructure complexity; header-only library with no allocation of its own — expected benefit very low.
- **TSan rejected.** No thread primitives exposed; TSan would only cover user code, outside scope.
- **Apple Clang not pinned.** Depends on the default Xcode of `macos-14`; consistent with common open-source practice.
- **`CHANGELOG.md` frozen at 0.6.0.** The `release.yml` workflow generates the changelog but does not commit it to the repository. The overhaul of the release process is addressed globally by US-042.

## Pointers

- Working branches: `chore/v1-check-01-api-freeze` to `chore/v1-check-11-readme-overhaul`.
- Main PRs with delivered fixes: #140, #142, #143, #144, #145.
- To reproduce an axis locally, consult the `## Method` section of the corresponding branch via `git show chore/v1-check-NN-*:doc/v1-checks/NN-*.md` (detailed reports archived in Git history, removed from tip to tidy `doc/`).
