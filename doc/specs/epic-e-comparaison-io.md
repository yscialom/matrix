# EPIC E — Comparaison & I/O

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-019 | `operator==` et `operator<=>` (defaulted) | P1 | ✅ Done |
| US-020 | Spécialisation `std::hash` | P2 | ✅ Done |
| US-021 | Refactor du constructeur variadic | P0 | ✅ Done |
| US-022 | Constructeur depuis `std::initializer_list` imbriquée (2D minimum) | P2 | ✅ Done |
| US-023 | Factories `zeros`, `ones`, `full`, `identity` | P1 | ✅ Done |
| US-024 | `operator<<` (ostream) | P2 | ✅ Done |
| US-025 | Spécialisation `std::formatter` (C++20) | P2 | ✅ Done |

---

## US-019 — `operator==` et `operator<=>` (defaulted)

**Priorité :** P1 — **Dépend de :** US-008

### Spécification
```cpp
friend bool operator==(const matrix& lhs, const matrix& rhs) = default;
friend auto operator<=>(const matrix& lhs, const matrix& rhs) = default;
```
Comparaison **lexicographique** sur le storage linéaire.
- Note : matrices de dimensions différentes ne se comparent pas (types différents → erreur de compilation, voulu).

### Critères d'acceptation
- [ ] `m1 == m2`, `m1 != m2`, `m1 < m2`, etc. fonctionnent
- [ ] `static_assert(std::three_way_comparable<matrix<int,3>>)`
- [ ] Test `comparison.cpp`

---

## US-020 — Spécialisation `std::hash`

**Priorité :** P2 — **Dépend de :** US-016

### Spécification
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

### Critères d'acceptation
- [x] `std::unordered_set<matrix<int,3>>` compile et fonctionne
- [x] Hash égal pour matrices égales, distinct pour matrices différentes (statistique)

---

## US-021 — Refactor du constructeur variadic

**Priorité :** P0 — **Dépend de :** US-009 — **Bloque :** US-022, US-023

### Bug actuel
```cpp
template<class ... Args>
matrix(Args&& ... args) : _data{std::forward<Args>(args)...} {}
```
Trop greedy — capture des appels de copie dès qu'on ajoute d'autres ctors.

### Spécification
Contraindre :
```cpp
template<class... Args>
  requires (sizeof...(Args) == linear_size)
        && (std::convertible_to<Args, T> && ...)
        && (sizeof...(Args) > 0)
constexpr explicit(sizeof...(Args) == 1) matrix(Args&&... args)
    : _data{static_cast<T>(std::forward<Args>(args))...}
{}
```
- `explicit` quand 1 seul argument pour éviter conversions implicites surprenantes
- Contrainte sur le nombre = `linear_size` (pas "≤ linear_size") : aggregate-init partiel en moins

### Critères d'acceptation
- [ ] `matrix<int,3> m{1,2,3}` compile
- [ ] `matrix<int,3> m{1,2}` ne compile PAS (clear error)
- [ ] `matrix<int,3> m1; matrix<int,3> m2 = m1;` appelle bien le copy ctor (et non le variadic)

---

## US-022 — Constructeur depuis `std::initializer_list` imbriquée (2D minimum)

**Priorité :** P2 — **Dépend de :** US-021

### Spécification
Pour matrices 2D :
```cpp
template<std::size_t D1 = sizeof...(Dimensions)>
  requires (D1 == 2)
matrix(std::initializer_list<std::initializer_list<T>> init);
```
Permet : `matrix<int,2,3> m{{1,2,3},{4,5,6}};`
- Vérifier sizes au runtime, throw si mauvaise taille
- N-D : non couvert par cette US (complexité disproportionnée — alternative : factory `make_matrix`)

### Critères d'acceptation
- [x] Init 2D fonctionne
- [x] Mauvaise taille → `std::length_error`
- [x] Test `nested_init.cpp`

---

## US-023 — Factories `zeros`, `ones`, `full`, `identity`

**Priorité :** P1 — **Dépend de :** US-021

### Spécification
Fonctions libres dans `namespace ysc` :
```cpp
template<class T, std::size_t... D>
constexpr matrix<T, D...> zeros() noexcept;

template<class T, std::size_t... D>
constexpr matrix<T, D...> ones();          // = full(T{1})

template<class T, std::size_t... D>
constexpr matrix<T, D...> full(const T& v);

// identity uniquement pour matrices 2D carrées
template<class T, std::size_t N>
constexpr matrix<T, N, N> identity();
```

### Critères d'acceptation
- [x] Tous testés
- [x] `static_assert(zeros<int,2,3>()(0,0) == 0)`
- [x] `identity<int,3>()(i,j) == (i==j ? 1 : 0)`

---

## US-024 — `operator<<` (ostream)

**Priorité :** P2 — **Dépend de :** US-016

### Spécification
- Format : `[1, 2, 3]` pour 1D, `[[1, 2], [3, 4]]` pour 2D, récursif pour N-D
- Implémentation via récursion sur `order` ; helper `detail::print_recursive`
- Utilise `<<` de `T` (donc T doit être streamable, mais SFINAE/concept friendly : pas de hard error si T pas streamable, simplement opérateur indisponible)

### Critères d'acceptation
- [x] `std::cout << matrix<int,2,2>{1,2,3,4}` → `[[1, 2], [3, 4]]`
- [x] Test `ostream.cpp`

---

## US-025 — Spécialisation `std::formatter` (C++20)

**Priorité :** P2 — **Dépend de :** US-024

### Spécification
```cpp
template<class T, std::size_t... D, class CharT>
struct std::formatter<ysc::matrix<T, D...>, CharT>;
```
Permet `std::format("{}", m)`. Réutilise la logique de US-024.

### Critères d'acceptation
- [x] `std::format("{}", matrix<int,2>{1,2})` retourne `"[1, 2]"`
- [x] Si compilateur sans `<format>` complet (Apple Clang 14), guard `#if __cpp_lib_format >= 201907L`
