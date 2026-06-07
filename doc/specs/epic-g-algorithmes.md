# EPIC G — Algorithms

| US | Title | Priority | Status |
|----|-------|----------|--------|
| US-030 | `apply()` / `transform()` | P1 | ✅ Done |
| US-031 | Reductions: `sum`, `min`, `max`, `all`, `any` | P1 | ✅ Done |
| US-032 | `transpose()` (2D only) | P1 | ✅ Done |
| US-033 | Matrix product `matmul` (2D) | P1 | ✅ Done |
| US-034 | Dot product `dot` (1D) | P2 | ✅ Done |

---

## US-030 — `apply()` / `transform()`

**Priority:** P1 — **Depends on:** US-016

### Specification
```cpp
template<std::invocable<T&> F>          void apply(F&& f);                 // mutates in place
template<std::invocable<const T&> F>    auto map(F&& f) const -> matrix<std::invoke_result_t<F, const T&>, Dimensions...>;
```
- `apply` modifies in place
- `map` returns a new matrix (potentially a different type)

### Acceptance criteria
- [ ] `m.apply([](int& v){ v *= 2; })` mutates m
- [ ] `m.map([](int v){ return std::to_string(v); })` returns `matrix<string, ...>`

---

## US-031 — Reductions: `sum`, `min`, `max`, `all`, `any`

**Priority:** P1 — **Depends on:** US-016

### Specification
Const member methods:
```cpp
T    sum()  const;
T    min()  const;   // requires linear_size > 0
T    max()  const;
bool all()  const;   // requires T convertible_to bool
bool any()  const;
```
Implementations via `std::accumulate`/`std::ranges::min`/etc.

### Acceptance criteria
- [x] All tested
- [x] `static_assert` on constexpr-able examples

---

## US-032 — `transpose()` (2D only)

**Priority:** P1 — **Depends on:** US-016

### Specification
```cpp
template<std::size_t R, std::size_t C>
constexpr matrix<T, C, R> transpose(const matrix<T, R, C>& m);
```
Free function in `ysc::`. Only exists for order-2 matrices.

### Acceptance criteria
- [ ] `transpose(matrix<int,2,3>{...})` returns `matrix<int,3,2>`
- [ ] `transpose(transpose(m)) == m`

---

## US-033 — Matrix product `matmul` (2D)

**Priority:** P1 — **Depends on:** US-032

### Specification
```cpp
template<class T, std::size_t M, std::size_t N, std::size_t P>
constexpr matrix<T, M, P> matmul(const matrix<T, M, N>& a, const matrix<T, N, P>& b);
```
- Naive O(MNP) implementation — no BLAS optimisation for this US (would be subject to a later optimisation if benchmarked as a bottleneck in US-039)
- Triply nested loop, i-k-j order (cache-friendly with row-major layout)

### Acceptance criteria
- [ ] Test against identity matrices, non-square matrices
- [ ] Test: `matmul(identity<int,3>(), m) == m`
- [ ] Compile-time check on inner dimensions

---

## US-034 — Dot product `dot` (1D)

**Priority:** P2 — **Depends on:** US-016

### Specification
```cpp
template<class T, std::size_t N>
constexpr T dot(const matrix<T, N>& a, const matrix<T, N>& b);
```

### Acceptance criteria
- [ ] `dot({1,2,3}, {4,5,6}) == 32`
