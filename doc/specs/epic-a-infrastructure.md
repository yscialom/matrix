# EPIC A — Infrastructure & CI/CD

| US | Title | Priority | Status |
|----|-------|----------|--------|
| US-001 | Multi-platform CI pipeline | P0 | ✅ Done |
| US-002 | Code coverage (gcov + lcov + Codecov) | P0 | ✅ Done |
| US-003 | Sanitizers (ASan + UBSan) | P1 | ✅ Done |
| US-004 | clang-format + CI verification | P1 | ✅ Done |
| US-005 | clang-tidy + CI verification | P1 | ✅ Done |
| US-006 | Doxygen doc published on GitHub Pages | P1 | ✅ Done |
| US-007 | Release automation (semver + GitHub Releases) | P2 | ✅ Done |

---

## US-001 — Multi-platform CI pipeline

**Priority:** P0 — **Depends on:** nothing — **Blocks:** US-002 to US-007

### Story
As a maintainer, I want a GitHub Actions workflow to compile and test the code on Linux/macOS/Windows × GCC/Clang/MSVC on every push and PR.

### Technical specification
- File: `.github/workflows/ci.yml`
- Triggers: `push` (all branches), `pull_request` (towards `develop` and `master`)
- Matrix:
  - `os: [ubuntu-24.04, macos-14, windows-2022]`
  - `compiler: [gcc-12, gcc-13, clang-15, clang-17, msvc, appleclang]` (filter by OS)
  - `build_type: [Debug, Release]`
- Steps:
  1. `actions/checkout@v4`
  2. Install compiler if needed
  3. `cmake -S . -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE`
  4. `cmake --build build --parallel`
  5. `ctest --test-dir build --output-on-failure`
- `fail-fast: false`
- Cache `~/.cache/ccache` (Linux/macOS) with `actions/cache@v4`

### Acceptance criteria
- [ ] All jobs green on the PR
- [ ] CI badge in `README.md`
- [ ] Total time < 6 min on warm cache

---

## US-002 — Code coverage (gcov + lcov + Codecov)

**Priority:** P0 — **Depends on:** US-001 — **Blocks:** US-041

### Technical specification
- New CI job `coverage` (Ubuntu + GCC only, Debug)
- Flags: `-O0 -g --coverage -fprofile-arcs -ftest-coverage`
- CMake option: `-DENABLE_COVERAGE=ON` controlling these flags
- Steps:
  1. Build + run tests
  2. `lcov --capture --directory build --output-file coverage.info`
  3. `lcov --remove coverage.info '*/test/*' '*/_deps/*' '/usr/*' --output-file coverage.info`
  4. `codecov-action@v4` with `coverage.info`
- Codecov badge in README

### Acceptance criteria
- [ ] Report visible on codecov.io
- [ ] Badge in README
- [ ] Initial coverage reported (≥ existing, no blocking threshold here — US-041 will add the threshold)

---

## US-003 — Sanitizers (ASan + UBSan)

**Priority:** P1 — **Depends on:** US-001

### Specification
- CI job `sanitizers` (Ubuntu + Clang 17, Debug)
- Flags: `-fsanitize=address,undefined -fno-omit-frame-pointer`
- CMake option: `-DENABLE_SANITIZERS=ON`
- Env variables: `ASAN_OPTIONS=detect_leaks=1`, `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`
- No TSan (no threads), no MSan (complex to provision)

### Acceptance criteria
- [ ] Job green on develop
- [ ] Any test failing under sanitizer = red build

---

## US-004 — clang-format + CI verification

**Priority:** P1 — **Depends on:** US-001

### Specification
- `.clang-format` file at the root:
  ```yaml
  BasedOnStyle: LLVM
  IndentWidth: 4
  ColumnLimit: 100
  AccessModifierOffset: -4
  PointerAlignment: Left
  AllowShortFunctionsOnASingleLine: InlineOnly
  ```
- CI job `format-check`: `clang-format --dry-run --Werror $(find src test -name '*.hpp' -o -name '*.cpp')`
- CMake target `format` that applies `clang-format -i`
- Pre-commit hook documented in `CONTRIBUTING.md` (optional creation)

### Acceptance criteria
- [ ] All files reformatted accordingly
- [ ] CI job blocks any unformatted PR

---

## US-005 — clang-tidy + CI verification

**Priority:** P1 — **Depends on:** US-001

### Specification
- `.clang-tidy` file at the root:
  ```yaml
  Checks: >
    bugprone-*,
    cppcoreguidelines-*,
    modernize-*,
    performance-*,
    portability-*,
    readability-*,
    -modernize-use-trailing-return-type,
    -readability-magic-numbers,
    -cppcoreguidelines-avoid-magic-numbers
  WarningsAsErrors: '*'
  HeaderFilterRegex: 'src/include/.*\.hpp$'
  ```
- CI job `lint`: `run-clang-tidy -p build` after cmake configure
- `compile_commands.json` generation via `CMAKE_EXPORT_COMPILE_COMMANDS=ON`

### Acceptance criteria
- [ ] No clang-tidy warning on delivered code
- [ ] CI red if a new warning is introduced

---

## US-006 — Doxygen doc published on GitHub Pages

**Priority:** P1 — **Depends on:** US-001

### Specification
- Workflow `.github/workflows/docs.yml` triggered on `push` to `develop`
- Steps: checkout, install Doxygen, `cmake --build build --target doc`, deployment to `gh-pages` via `peaceiris/actions-gh-pages@v3`
- The existing `doc/publish.sh` may be deleted or made local-only
- Public URL: `https://yscialom.github.io/matrix/`
- "docs" badge in README

### Acceptance criteria
- [ ] Doc accessible at the above URL
- [ ] Automatically updated on every merge to `develop`

---

## US-007 — Release automation (semver + GitHub Releases)

**Priority:** P2 — **Depends on:** US-001

### Specification
- Workflow `.github/workflows/release.yml` triggered on tag `v*.*.*`
- Steps: build, test, CHANGELOG generation via `git-cliff` (config `cliff.toml`), GitHub release creation with `softprops/action-gh-release@v1`
- **Conventional Commits** convention (feat:, fix:, chore:, etc.) — documented in `CONTRIBUTING.md`
- Bump of `VERSION_MAJOR/MINOR/PATCH` in `CMakeLists.txt` on each release (manual via PR, CI checks consistency with the tag)

### Acceptance criteria
- [ ] Release v2.0.0 created at the end of the roadmap (cf. US-042)
- [ ] CHANGELOG generated
