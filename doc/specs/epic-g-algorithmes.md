# EPIC G — Algorithmes

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-030 | `apply()` / `transform()` | P1 | ✅ Done |
| US-031 | Réductions : `sum`, `min`, `max`, `all`, `any` | P1 | ✅ Done |
| US-032 | `transpose()` (2D uniquement) | P1 | ✅ Done |
| US-033 | Produit matriciel `matmul` (2D) | P1 | ✅ Done |
| US-034 | Produit scalaire `dot` (1D) | P2 | ✅ Done |

---

## US-030 — `apply()` / `transform()`

**Priorité :** P1 — **Dépend de :** US-016

### Spécification
```cpp
template<std::invocable<T&> F>          void apply(F&& f);                 // mute en place
template<std::invocable<const T&> F>    auto map(F&& f) const -> matrix<std::invoke_result_t<F, const T&>, Dimensions...>;
```
- `apply` modifie en place
- `map` retourne nouvelle matrix (potentiellement type différent)

### Critères d'acceptation
- [ ] `m.apply([](int& v){ v *= 2; })` mute m
- [ ] `m.map([](int v){ return std::to_string(v); })` retourne `matrix<string, ...>`

---

## US-031 — Réductions : `sum`, `min`, `max`, `all`, `any`

**Priorité :** P1 — **Dépend de :** US-016

### Spécification
Méthodes membres const :
```cpp
T    sum()  const;
T    min()  const;   // requires linear_size > 0
T    max()  const;
bool all()  const;   // requires T convertible_to bool
bool any()  const;
```
Implémentations via `std::accumulate`/`std::ranges::min`/etc.

### Critères d'acceptation
- [x] Tous testés
- [x] `static_assert` sur exemples constexpr-able

---

## US-032 — `transpose()` (2D uniquement)

**Priorité :** P1 — **Dépend de :** US-016

### Spécification
```cpp
template<std::size_t R, std::size_t C>
constexpr matrix<T, C, R> transpose(const matrix<T, R, C>& m);
```
Fonction libre dans `ysc::`. N'existe que pour matrices d'ordre 2.

### Critères d'acceptation
- [ ] `transpose(matrix<int,2,3>{...})` retourne `matrix<int,3,2>`
- [ ] `transpose(transpose(m)) == m`

---

## US-033 — Produit matriciel `matmul` (2D)

**Priorité :** P1 — **Dépend de :** US-032

### Spécification
```cpp
template<class T, std::size_t M, std::size_t N, std::size_t P>
constexpr matrix<T, M, P> matmul(const matrix<T, M, N>& a, const matrix<T, N, P>& b);
```
- Implémentation naïve O(MNP) — pas d'optim BLAS pour cette US (ferait l'objet d'une optim ultérieure si benchmarké comme bottleneck dans US-039)
- Boucle triplement imbriquée, ordre i-k-j (cache-friendly avec layout row-major)

### Critères d'acceptation
- [ ] Test contre matrices identités, matrices non-carrées
- [ ] Test : `matmul(identity<int,3>(), m) == m`
- [ ] Compile-time check sur dimensions intérieures

---

## US-034 — Produit scalaire `dot` (1D)

**Priorité :** P2 — **Dépend de :** US-016

### Spécification
```cpp
template<class T, std::size_t N>
constexpr T dot(const matrix<T, N>& a, const matrix<T, N>& b);
```

### Critères d'acceptation
- [ ] `dot({1,2,3}, {4,5,6}) == 32`
