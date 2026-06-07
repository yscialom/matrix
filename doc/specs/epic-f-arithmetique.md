# EPIC F — Arithmetic

| US | Title | Priority | Status |
|----|-------|----------|--------|
| US-026 | Element-wise addition/subtraction | P1 | ✅ Done |
| US-027 | Hadamard multiplication/division (element-wise) | P1 | ✅ Done |
| US-028 | Scalar arithmetic | P1 | ✅ Done |
| US-029 | Unary operators `+`/`-` | P2 | ✅ Done |

---

## US-026 — Element-wise addition/subtraction

**Priority:** P1 — **Depends on:** US-016

### Specification
```cpp
matrix& operator+=(const matrix& other);
matrix& operator-=(const matrix& other);
friend matrix operator+(matrix lhs, const matrix& rhs) { return lhs += rhs; }
friend matrix operator-(matrix lhs, const matrix& rhs) { return lhs -= rhs; }
```
Implementation: `std::transform` on `_data`.
- Type constraint: `requires requires(T a, T b) { a += b; }`

### Acceptance criteria
- [ ] `m1 + m2`, `m1 - m2`, `m1 += m2`, `m1 -= m2` work
- [ ] Test: matrix before/after identical, no modification of operand
- [ ] Compile-time error if T does not have `operator+=`

---

## US-027 — Hadamard multiplication/division (element-wise)

**Priority:** P1 — **Depends on:** US-026

### Specification
**Note:** `operator*` here is the **Hadamard product** (element-wise), NOT the matrix product — this is the STL/numpy convention. The matrix product will be in US-033 under the name `matmul`.
```cpp
matrix& operator*=(const matrix& other);  // Hadamard
matrix& operator/=(const matrix& other);
friend matrix operator*(matrix lhs, const matrix& rhs);
friend matrix operator/(matrix lhs, const matrix& rhs);
```
Clearly document in the docstring.

### Acceptance criteria
- [ ] Hadamard works
- [ ] Explicit doc on the semantics
- [ ] Test `arithmetic_hadamard.cpp`

---

## US-028 — Scalar arithmetic

**Priority:** P1 — **Depends on:** US-027

### Specification
```cpp
template<class Scalar> matrix& operator*=(const Scalar& s);
template<class Scalar> matrix& operator/=(const Scalar& s);
template<class Scalar> matrix& operator+=(const Scalar& s);
template<class Scalar> matrix& operator-=(const Scalar& s);
// + binary friend operators (commutative for *)
```

### Acceptance criteria
- [ ] `m * 2`, `2 * m`, `m / 2` work
- [ ] Test `arithmetic_scalar.cpp`

---

## US-029 — Unary operators `+`/`-`

**Priority:** P2 — **Depends on:** US-028

### Specification
```cpp
matrix operator+() const { return *this; }
matrix operator-() const;  // element-wise negation
```

### Acceptance criteria
- [ ] `-m` returns matrix with negated elements
- [ ] `+m == m`
