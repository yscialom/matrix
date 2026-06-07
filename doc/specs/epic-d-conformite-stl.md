# EPIC D — STL Compliance

| US | Title | Priority | Status |
|----|-------|----------|--------|
| US-015 | Member typedefs | P0 | ✅ Done |
| US-016 | Iterators (begin/end and co.) | P0 | ✅ Done |
| US-017 | `size()`, `empty()`, `data()`, `max_size()` | P1 | ✅ Done |
| US-018 | `front()`, `back()`, `fill()`, member `swap()` | P1 | ✅ Done |

---

## US-015 — Member typedefs

**Priority:** P0 — **Depends on:** US-008 — **Blocks:** US-016

### Specification
Add in `class matrix`:
```cpp
using value_type             = T;
using size_type              = std::size_t;
using difference_type        = std::ptrdiff_t;
using reference              = T&;
using const_reference        = const T&;
using pointer                = T*;
using const_pointer          = const T*;
using iterator               = typename std::array<T, linear_size>::iterator;
using const_iterator         = typename std::array<T, linear_size>::const_iterator;
using reverse_iterator       = std::reverse_iterator<iterator>;
using const_reverse_iterator = std::reverse_iterator<const_iterator>;
```

### Acceptance criteria
- [ ] `static_assert(std::same_as<matrix<int,3>::value_type, int>)` in tests
- [ ] All typedefs publicly accessible

---

## US-016 — Iterators (begin/end and co.)

**Priority:** P0 — **Depends on:** US-015

### Specification
**Linear** iteration (in row-major order) over all elements.
```cpp
constexpr iterator               begin()        noexcept { return _data.begin(); }
constexpr const_iterator         begin()  const noexcept { return _data.begin(); }
constexpr const_iterator         cbegin() const noexcept { return _data.cbegin(); }
constexpr iterator               end()          noexcept { return _data.end(); }
constexpr const_iterator         end()    const noexcept { return _data.end(); }
constexpr const_iterator         cend()   const noexcept { return _data.cend(); }
constexpr reverse_iterator       rbegin()       noexcept { return _data.rbegin(); }
constexpr const_reverse_iterator rbegin() const noexcept { return _data.rbegin(); }
constexpr const_reverse_iterator crbegin() const noexcept { return _data.crbegin(); }
constexpr reverse_iterator       rend()         noexcept { return _data.rend(); }
constexpr const_reverse_iterator rend()   const noexcept { return _data.rend(); }
constexpr const_reverse_iterator crend()  const noexcept { return _data.crend(); }
```

### Acceptance criteria
- [ ] `static_assert(std::contiguous_iterator<iterator>)`
- [ ] Range-for works: `for (auto& v : m) ...`
- [ ] `std::ranges::sort(m)` compiles and works
- [ ] Test `iterators.cpp` covers all variants

---

## US-017 — `size()`, `empty()`, `data()`, `max_size()`

**Priority:** P1 — **Depends on:** US-015

### Specification
```cpp
static constexpr size_type size()      noexcept { return linear_size; }
static constexpr bool      empty()     noexcept { return linear_size == 0; }
static constexpr size_type max_size()  noexcept { return linear_size; }
constexpr pointer          data()       noexcept { return _data.data(); }
constexpr const_pointer    data() const noexcept { return _data.data(); }
```
Note: `size()/max_size()/empty()` are `static` because they are compile-time. Document this.

### Acceptance criteria
- [ ] `static_assert(matrix<int,2,3>::size() == 6)` passes
- [ ] `data()` returns address of `_data[0]`

---

## US-018 — `front()`, `back()`, `fill()`, member `swap()`

**Priority:** P1 — **Depends on:** US-016, US-011

### Specification
```cpp
constexpr reference       front()       noexcept { return _data.front(); }
constexpr const_reference front() const noexcept { return _data.front(); }
constexpr reference       back()        noexcept { return _data.back(); }
constexpr const_reference back()  const noexcept { return _data.back(); }
void fill(const T& value)              { _data.fill(value); }   // already via US-011
void swap(matrix& other)      noexcept(/* T swap noexcept */);  // member, in addition to friend swap
```

### Acceptance criteria
- [ ] All tested on empty matrix → should not compile because `front/back` is UB on size 0 (cf. US-038)
- [ ] Test `accessors.cpp`
