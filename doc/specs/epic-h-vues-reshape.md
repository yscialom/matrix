# EPIC H — Vues & reshape

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-035 | `matrix_view<T, D...>` (read/write view) | P2 | ✅ Done |
| US-036 | `slice()` générique N-D | P2 | ✅ Done |
| US-037 | `reshape()` et `flatten()` | P2 | ✅ Done |
| US-044 | Constructeur `matrix(matrix_view)` (owning ← view) | P2 | ✅ Done |

---

## US-035 — `matrix_view<T, D...>` (read/write view)

**Priorité :** P2 — **Dépend de :** US-016

### Spécification
- Nouvelle classe `matrix_view<T, Dimensions...>` non-owning
- Storage : `T* data_` + dimensions (template params)
- Mêmes interfaces d'accès que `matrix` (operator(), at, begin/end, etc.)
- Construction depuis `matrix& m` : `matrix_view<T, D...> v = m;`
- Pas de mdspan : on n'utilise pas C++23 ; cette classe est plus simple
- Header : `src/include/matrix_view.hpp`

### Critères d'acceptation
- [ ] Construction depuis matrix, mutation reflétée
- [ ] `static_assert(sizeof(matrix_view<int,3,3>) == sizeof(int*))`

---

## US-036 — `slice()` générique N-D

**Priorité :** P2 — **Dépend de :** US-035 — **Bloque :** US-037, US-044

### Spécification
- API `m.slice(spec0, spec1, ...)` — chaque argument : `ysc::all` (conserver) ou un entier (fixer)
- **Padding `all` à droite** : si `sizeof...(Specs) < order`, complétion implicite par `all` (`m.slice(0)` sur 3D ≡ `m.slice(0, all, all)`)
- Refonte de `matrix_view` en template `<T, Storage, Dims…>` avec deux **spécialisations partielles** :
  - `matrix_view<T, contiguous, …>` — anciennement `matrix_view<T, …>` (US-035, breaking change)
  - `matrix_view<T, strided, …>` — nouvelle, pour les vues non-contiguës
- `slice()` choisit `contiguous` si les dims fixées forment un préfixe, `strided` sinon
- Conversion implicite `contiguous` → `strided` (toute vue contiguë s'utilise comme strided)
- `slice()` **lève** `std::out_of_range` si un index fixé est hors-bornes
- Aliases ergonomiques `m.row(i)` ≡ `slice(i)` et `m.col(j)` ≡ `slice(all, j)`, contraints à `order == 2`

**Spécification détaillée :** voir `doc/US-036.md`.

### Critères d'acceptation
- [x] `m.slice(i, all, all)` retourne `matrix_view<T, contiguous, …>`
- [x] `m.slice(all, j, all)` retourne `matrix_view<T, strided, …>`
- [x] `m.slice(0)` sur 3D ≡ `m.slice(0, all, all)` ; `m.slice()` = vue complète
- [x] `m.slice(idx_hors_bornes, …)` lève `std::out_of_range`
- [x] Conversion implicite contiguous → strided testée
- [x] Mutation via vue reflétée dans la matrice d'origine
- [x] `row()` / `col()` refusés à la compilation pour `order ≠ 2`

---

## US-037 — `reshape()` et `flatten()`

**Priorité :** P2 — **Dépend de :** US-035

### Spécification
```cpp
template<std::size_t... NewD>
matrix_view<T, NewD...> reshape();   // requires (NewD * ... == linear_size)

matrix_view<T, linear_size> flatten();
```
Reshape = juste un changement de vue, zero-copy.

### Critères d'acceptation
- [x] `static_assert((NewD * ...) == linear_size)` à la compilation
- [x] Mutation via reshape reflétée

---

## US-044 — Constructeur `matrix(matrix_view)` (owning ← view)

**Priorité :** P2 — **Dépend de :** US-036 — **Épopée :** H

### Story
En tant qu'utilisateur de `ysc::matrix`, je veux pouvoir reconstruire une matrice owning à partir d'une vue (contiguë ou strided), de manière à matérialiser un résultat de `slice()`, `row()`, `col()` ou `reshape()` en une nouvelle matrice indépendante.

### Spécification
```cpp
template <class T, std::size_t... Dims> class matrix {
    // ...
    template <class Storage>
    explicit matrix(const matrix_view<T, Storage, Dims...>& v);
};

// Usage :
ysc::matrix<int, 3, 4> m{/*...*/};
auto v  = m.slice(ysc::all, 0);   // matrix_view<int, strided, 3>
auto m2 = ysc::matrix(v);          // matrix<int, 3>, copie owning
```

- Deux surcharges (sélection par `Storage`) :
  - `contiguous` : copie via `std::copy(v.begin(), v.end(), _data.begin())`.
  - `strided` : copie élément par élément via `operator()` (pas d'itérateurs sur strided dans US-036).
- Constructeur **`explicit`** pour éviter les conversions implicites surprises.
- Pas d'allocation, pas d'exception (en dehors de celles éventuelles du copy ctor de `T`).

### Critères d'acceptation
- [x] `auto m2 = ysc::matrix(v);` compile pour `v` issu de `slice(...)`, `row(...)`, `col(...)`.
- [x] `m2` est indépendant : mutation de `m2` n'affecte pas la matrice source, et vice-versa.
- [x] Surcharge `contiguous` testée (copie de `m.slice(i, all, all)`).
- [x] Surcharge `strided` testée (copie de `m.col(j)`).
- [x] Constructeur Doxygen-documenté (`@brief`, `@tparam`, `@param`, `@code`…`@endcode`, `@ingroup`).
- [x] Build et tests verts, pas de warning clang-format ni clang-tidy.
