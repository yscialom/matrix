# EPIC F — Arithmétique

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-026 | Addition/Soustraction élément-par-élément | P1 | ✅ Done |
| US-027 | Multiplication/Division Hadamard (élément-par-élément) | P1 | ✅ Done |
| US-028 | Arithmétique scalaire | P1 | ✅ Done |
| US-029 | Opérateurs unaires `+`/`-` | P2 | ✅ Done |

---

## US-026 — Addition/Soustraction élément-par-élément

**Priorité :** P1 — **Dépend de :** US-016

### Spécification
```cpp
matrix& operator+=(const matrix& other);
matrix& operator-=(const matrix& other);
friend matrix operator+(matrix lhs, const matrix& rhs) { return lhs += rhs; }
friend matrix operator-(matrix lhs, const matrix& rhs) { return lhs -= rhs; }
```
Implémentation : `std::transform` sur `_data`.
- Contrainte de type : `requires requires(T a, T b) { a += b; }`

### Critères d'acceptation
- [ ] `m1 + m2`, `m1 - m2`, `m1 += m2`, `m1 -= m2` fonctionnent
- [ ] Test : matrice avant/après identique, pas de modif de l'opérande
- [ ] Compile-time error si T n'a pas `operator+=`

---

## US-027 — Multiplication/Division Hadamard (élément-par-élément)

**Priorité :** P1 — **Dépend de :** US-026

### Spécification
**Note :** `operator*` est ici la **multiplication de Hadamard** (élément-par-élément), PAS le produit matriciel — c'est le choix STL/numpy. Le produit matriciel sera dans US-033 sous nom `matmul`.
```cpp
matrix& operator*=(const matrix& other);  // Hadamard
matrix& operator/=(const matrix& other);
friend matrix operator*(matrix lhs, const matrix& rhs);
friend matrix operator/(matrix lhs, const matrix& rhs);
```
Documenter clairement dans la docstring.

### Critères d'acceptation
- [ ] Hadamard fonctionne
- [ ] Doc explicite sur la sémantique
- [ ] Test `arithmetic_hadamard.cpp`

---

## US-028 — Arithmétique scalaire

**Priorité :** P1 — **Dépend de :** US-027

### Spécification
```cpp
template<class Scalar> matrix& operator*=(const Scalar& s);
template<class Scalar> matrix& operator/=(const Scalar& s);
template<class Scalar> matrix& operator+=(const Scalar& s);
template<class Scalar> matrix& operator-=(const Scalar& s);
// + opérateurs binaires friend (commute pour *)
```

### Critères d'acceptation
- [ ] `m * 2`, `2 * m`, `m / 2` fonctionnent
- [ ] Test `arithmetic_scalar.cpp`

---

## US-029 — Opérateurs unaires `+`/`-`

**Priorité :** P2 — **Dépend de :** US-028

### Spécification
```cpp
matrix operator+() const { return *this; }
matrix operator-() const;  // négation élément-par-élément
```

### Critères d'acceptation
- [ ] `-m` retourne matrice avec éléments négatifs
- [ ] `+m == m`
