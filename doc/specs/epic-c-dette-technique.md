# EPIC C — Technical Debt

| US | Title | Priority | Status |
|----|-------|----------|--------|
| US-011 | Resolve the `feature-fill` conflict | P0 | ✅ Done |
| US-012 | CMake cleanup & typos | P0 | ✅ Done |
| US-013 | Fix bug: templated `operator=` without `return *this;` | P0 | ✅ Done |
| US-014 | Rename `_details` → `detail` | P2 | ✅ Done |

---

## US-011 — Resolve the `feature-fill` conflict

**Priority:** P0 — **Depends on:** nothing

### Story
The branch `origin/feature-fill` adds `fill()` but removes `at()` (created before the integration of `feature-at`). We need to integrate `fill()` without losing `at()`.

### Specification
- Cherry-pick only the functional content of `22accdb`:
  ```cpp
  void fill(const T& value) noexcept(std::is_nothrow_copy_assignable_v<T>)
  { _data.fill(value); }
  ```
- Do NOT import the removal of `at()`, nor the renaming `Coords→Args`, nor the `utils.hpp` modifications
- Add tests `fill.cpp`: fill on trivial, on user-defined, on matrix of matrix
- Delete `origin/feature-fill` after merge (command in the PR description)

### Acceptance criteria
- [ ] `at()` still present and functional
- [ ] `fill()` tested (100% coverage)
- [ ] Remote branch `feature-fill` deleted

---

## US-012 — CMake cleanup & typos

**Priority:** P0 — **Depends on:** nothing

### Specification
- `test/CMakeLists.txt` line 2: `Tets` → `Tests`
- Bump `cmake_minimum_required(VERSION 3.0)` → `3.20`
- `set(CMAKE_CXX_STANDARD 17)` remains until US-008 is merged

### Acceptance criteria
- [ ] No CMake warnings
- [ ] Identical build

---

## US-013 — Fix bug: templated `operator=` without `return *this;`

**Priority:** P0 — **Depends on:** nothing

### Bug
`src/include/matrix.hpp` lines 199-200 and 214-216:
```cpp
template<class U>
matrix& operator=(matrix<U, Dimensions...> const& other)
{ std::copy(cbegin(other._data), cend(other._data), begin(_data)); }
// MISSING: return *this;
```
This is **UB** (function returning non-void without return). Same for the move version.

### Specification
- Add `return *this;` in both templated operators
- Regression test `assignment_returns_self.cpp`: `auto& ref = (m1 = m2); ASSERT_EQ(&ref, &m1);`

### Acceptance criteria
- [ ] Bug fixed
- [ ] Test added

---

## US-014 — Rename `_details` → `detail`

**Priority:** P2 — **Depends on:** nothing

### Specification
- More standard convention (cf. Boost, std)
- No user-facing breaking change (internal namespace)

### Acceptance criteria
- [x] All usages renamed
- [x] Build green
