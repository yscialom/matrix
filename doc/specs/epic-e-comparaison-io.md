# EPIC E — Comparison & I/O

| US | Title | Priority | Status |
|----|-------|----------|--------|
| US-019 | `operator==` and `operator<=>` (defaulted) | P1 | ✅ Done |
| US-020 | `std::hash` specialisation | P2 | ✅ Done |
| US-021 | Refactor of the variadic constructor | P0 | ✅ Done |
| US-022 | Constructor from nested `std::initializer_list` (2D minimum) | P2 | ✅ Done |
| US-023 | Factories `zeros`, `ones`, `full`, `identity` | P1 | ✅ Done |
| US-024 | `operator<<` (ostream) | P2 | ✅ Done |
| US-025 | `std::formatter` specialisation (C++20) | P2 | ✅ Done |

---

## US-019 — `operator==` and `operator<=>` (defaulted)

**Priority:** P1 — **Depends on:** US-008

### Specification
```cpp
friend bool operator==(const matrix& lhs, const matrix& rhs) = default;
friend auto operator<=>(const matrix& lhs, const matrix& rhs) = default;
```
**Lexicographic** comparison on the linear storage.
- Note: matrices of different dimensions do not compare (different types → compilation error, intentional).

### Acceptance criteria
- [ ] `m1 == m2`, `m1 != m2`, `m1 < m2`, etc. work
- [ ] `static_assert(std::three_way_comparable<matrix<int,3>>)`
- [ ] Test `comparison.cpp`

---

## US-020 — `std::hash` specialisation

**Priority:** P2 — **Depends on:** US-016

### Specification
```cpp
namespace std {
  template<class T, std::size_t... D>
  struct hash<ysc::matrix<T, D...>> {
    std::size_t operator()(const ysc::matrix<T, D...>& m) const noexcept {
      std::size_t h = 0;
      std::hash<T> hasher;
      for (const auto& v : m) {
        h ^= hasher(v) + 0x9e3779b9 + (h << 6) + (h >> 2);  // boost::hash_combine
      }
      return h;
    }
  };
}
```

### Acceptance criteria
- [x] `std::unordered_set<matrix<int,3>>` compiles and works
- [x] Equal hash for equal matrices, distinct for different matrices (statistical)

---

## US-021 — Refactor of the variadic constructor

**Priority:** P0 — **Depends on:** US-009 — **Blocks:** US-022, US-023

### Current bug
```cpp
template<class ... Args>
matrix(Args&& ... args) : _data{std::forward<Args>(args)...} {}
```
Too greedy — captures copy calls as soon as other constructors are added.

### Specification
Constrain:
```cpp
template<class... Args>
  requires (sizeof...(Args) == linear_size)
        && (std::convertible_to<Args, T> && ...)
        && (sizeof...(Args) > 0)
constexpr explicit(sizeof...(Args) == 1) matrix(Args&&... args)
    : _data{static_cast<T>(std::forward<Args>(args))...}
{}
```
- `explicit` when 1 single argument to avoid surprising implicit conversions
- Constraint on count = `linear_size` (not "≤ linear_size"): no partial aggregate-init

### Acceptance criteria
- [ ] `matrix<int,3> m{1,2,3}` compiles
- [ ] `matrix<int,3> m{1,2}` does NOT compile (clear error)
- [ ] `matrix<int,3> m1; matrix<int,3> m2 = m1;` correctly calls the copy ctor (not the variadic)

---

## US-022 — Constructor from nested `std::initializer_list` (2D minimum)

**Priority:** P2 — **Depends on:** US-021

### Specification
For 2D matrices:
```cpp
template<std::size_t D1 = sizeof...(Dimensions)>
  requires (D1 == 2)
matrix(std::initializer_list<std::initializer_list<T>> init);
```
Allows: `matrix<int,2,3> m{{1,2,3},{4,5,6}};`
- Verify sizes at runtime, throw if wrong size
- N-D: not covered by this US (disproportionate complexity — alternative: factory `make_matrix`)

### Acceptance criteria
- [x] 2D init works
- [x] Wrong size → `std::length_error`
- [x] Test `nested_init.cpp`

---

## US-023 — Factories `zeros`, `ones`, `full`, `identity`

**Priority:** P1 — **Depends on:** US-021

### Specification
Free functions in `namespace ysc`:
```cpp
template<class T, std::size_t... D>
constexpr matrix<T, D...> zeros() noexcept;

template<class T, std::size_t... D>
constexpr matrix<T, D...> ones();          // = full(T{1})

template<class T, std::size_t... D>
constexpr matrix<T, D...> full(const T& v);

// identity only for square 2D matrices
template<class T, std::size_t N>
constexpr matrix<T, N, N> identity();
```

### Acceptance criteria
- [x] All tested
- [x] `static_assert(zeros<int,2,3>()(0,0) == 0)`
- [x] `identity<int,3>()(i,j) == (i==j ? 1 : 0)`

---

## US-024 — `operator<<` (ostream)

**Priority:** P2 — **Depends on:** US-016

### Specification
- Format: `[1, 2, 3]` for 1D, `[[1, 2], [3, 4]]` for 2D, recursive for N-D
- Implementation via recursion on `order`; helper `detail::print_recursive`
- Uses `<<` of `T` (so T must be streamable, but SFINAE/concept friendly: no hard error if T is not streamable, operator simply unavailable)

### Acceptance criteria
- [x] `std::cout << matrix<int,2,2>{1,2,3,4}` → `[[1, 2], [3, 4]]`
- [x] Test `ostream.cpp`

---

## US-025 — `std::formatter` specialisation (C++20)

**Priority:** P2 — **Depends on:** US-024

### Specification
```cpp
template<class T, std::size_t... D, class CharT>
struct std::formatter<ysc::matrix<T, D...>, CharT>;
```
Allows `std::format("{}", m)`. Reuses the logic from US-024.

### Acceptance criteria
- [x] `std::format("{}", matrix<int,2>{1,2})` returns `"[1, 2]"`
- [x] If compiler lacks full `<format>` (Apple Clang 14), guard `#if __cpp_lib_format >= 201907L`
