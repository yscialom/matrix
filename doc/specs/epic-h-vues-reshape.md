# EPIC H — Views & reshape

| US | Title | Priority | Status |
|----|-------|----------|--------|
| US-035 | `matrix_view<T, D...>` (read/write view) | P2 | ✅ Done |
| US-036 | Generic N-D `slice()` | P2 | ✅ Done |
| US-037 | `reshape()` and `flatten()` | P2 | ✅ Done |
| US-044 | Constructor `matrix(matrix_view)` (owning ← view) | P2 | ✅ Done |

---

## US-035 — `matrix_view<T, D...>` (read/write view)

**Priority:** P2 — **Depends on:** US-016

### Specification
- New non-owning class `matrix_view<T, Dimensions...>`
- Storage: `T* data_` + dimensions (template params)
- Same access interfaces as `matrix` (operator(), at, begin/end, etc.)
- Construction from `matrix& m`: `matrix_view<T, D...> v = m;`
- No mdspan: we do not use C++23; this class is simpler
- Header: `src/include/matrix_view.hpp`

### Acceptance criteria
- [ ] Construction from matrix, mutation reflected
- [ ] `static_assert(sizeof(matrix_view<int,3,3>) == sizeof(int*))`

---

## US-036 — Generic N-D `slice()`

**Priority:** P2 — **Depends on:** US-035 — **Blocks:** US-037, US-044

### Specification
- API `m.slice(spec0, spec1, ...)` — each argument: `ysc::all` (retain) or an integer (fix)
- **Right-padding `all`**: if `sizeof...(Specs) < order`, implicit completion with `all` (`m.slice(0)` on 3D ≡ `m.slice(0, all, all)`)
- Refactoring of `matrix_view` into template `<T, Storage, Dims…>` with two **partial specialisations**:
  - `matrix_view<T, contiguous, …>` — formerly `matrix_view<T, …>` (US-035, breaking change)
  - `matrix_view<T, strided, …>` — new, for non-contiguous views
- `slice()` chooses `contiguous` if the fixed dims form a prefix, `strided` otherwise
- Implicit conversion `contiguous` → `strided` (any contiguous view can be used as strided)
- `slice()` **raises** `std::out_of_range` if a fixed index is out-of-bounds
- Ergonomic aliases `m.row(i)` ≡ `slice(i)` and `m.col(j)` ≡ `slice(all, j)`, constrained to `order == 2`

**Detailed specification:** see `doc/US-036.md`.

### Acceptance criteria
- [x] `m.slice(i, all, all)` returns `matrix_view<T, contiguous, …>`
- [x] `m.slice(all, j, all)` returns `matrix_view<T, strided, …>`
- [x] `m.slice(0)` on 3D ≡ `m.slice(0, all, all)`; `m.slice()` = full view
- [x] `m.slice(out_of_bounds_idx, …)` raises `std::out_of_range`
- [x] Implicit conversion contiguous → strided tested
- [x] Mutation via view reflected in the source matrix
- [x] `row()` / `col()` refused at compile time for `order ≠ 2`

---

## US-037 — `reshape()` and `flatten()`

**Priority:** P2 — **Depends on:** US-035

### Specification
```cpp
template<std::size_t... NewD>
matrix_view<T, NewD...> reshape();   // requires (NewD * ... == linear_size)

matrix_view<T, linear_size> flatten();
```
Reshape = just a view change, zero-copy.

### Acceptance criteria
- [x] `static_assert((NewD * ...) == linear_size)` at compile time
- [x] Mutation via reshape reflected

---

## US-044 — Constructor `matrix(matrix_view)` (owning ← view)

**Priority:** P2 — **Depends on:** US-036 — **Epic:** H

### Story
As a user of `ysc::matrix`, I want to be able to reconstruct an owning matrix from a view (contiguous or strided), so as to materialise a result of `slice()`, `row()`, `col()` or `reshape()` into a new independent matrix.

### Specification
```cpp
template <class T, std::size_t... Dims> class matrix {
    // ...
    template <class Storage>
    explicit matrix(const matrix_view<T, Storage, Dims...>& v);
};

// Usage:
ysc::matrix<int, 3, 4> m{/*...*/};
auto v  = m.slice(ysc::all, 0);   // matrix_view<int, strided, 3>
auto m2 = ysc::matrix(v);          // matrix<int, 3>, owning copy
```

- Two overloads (selection by `Storage`):
  - `contiguous`: copy via `std::copy(v.begin(), v.end(), _data.begin())`.
  - `strided`: element-by-element copy via `operator()` (no iterators on strided in US-036).
- **`explicit`** constructor to avoid surprising implicit conversions.
- No allocation, no exception (apart from those possibly thrown by the copy ctor of `T`).

### Acceptance criteria
- [x] `auto m2 = ysc::matrix(v);` compiles for `v` from `slice(...)`, `row(...)`, `col(...)`.
- [x] `m2` is independent: mutating `m2` does not affect the source matrix, and vice versa.
- [x] `contiguous` overload tested (copy of `m.slice(i, all, all)`).
- [x] `strided` overload tested (copy of `m.col(j)`).
- [x] Constructor Doxygen-documented (`@brief`, `@tparam`, `@param`, `@code`…`@endcode`, `@ingroup`).
- [x] Build and tests green, no clang-format or clang-tidy warnings.
