# EPIC D — Conformité STL

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-015 | Typedefs membres | P0 | ✅ Done |
| US-016 | Itérateurs (begin/end et co.) | P0 | ✅ Done |
| US-017 | `size()`, `empty()`, `data()`, `max_size()` | P1 | ✅ Done |
| US-018 | `front()`, `back()`, `fill()`, `swap()` membre | P1 | ✅ Done |

---

## US-015 — Typedefs membres

**Priorité :** P0 — **Dépend de :** US-008 — **Bloque :** US-016

### Spécification
Ajouter dans `class matrix` :
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

### Critères d'acceptation
- [ ] `static_assert(std::same_as<matrix<int,3>::value_type, int>)` dans tests
- [ ] Tous les typedefs accessibles publiquement

---

## US-016 — Itérateurs (begin/end et co.)

**Priorité :** P0 — **Dépend de :** US-015

### Spécification
Itération **linéaire** (en row-major) sur tous les éléments.
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

### Critères d'acceptation
- [ ] `static_assert(std::contiguous_iterator<iterator>)`
- [ ] Range-for fonctionne : `for (auto& v : m) ...`
- [ ] `std::ranges::sort(m)` compile et fonctionne
- [ ] Test `iterators.cpp` couvre toutes les variantes

---

## US-017 — `size()`, `empty()`, `data()`, `max_size()`

**Priorité :** P1 — **Dépend de :** US-015

### Spécification
```cpp
static constexpr size_type size()      noexcept { return linear_size; }
static constexpr bool      empty()     noexcept { return linear_size == 0; }
static constexpr size_type max_size()  noexcept { return linear_size; }
constexpr pointer          data()       noexcept { return _data.data(); }
constexpr const_pointer    data() const noexcept { return _data.data(); }
```
Note : `size()/max_size()/empty()` sont `static` car compile-time. Documenter.

### Critères d'acceptation
- [ ] `static_assert(matrix<int,2,3>::size() == 6)` passe
- [ ] `data()` retourne adresse de `_data[0]`

---

## US-018 — `front()`, `back()`, `fill()`, `swap()` membre

**Priorité :** P1 — **Dépend de :** US-016, US-011

### Spécification
```cpp
constexpr reference       front()       noexcept { return _data.front(); }
constexpr const_reference front() const noexcept { return _data.front(); }
constexpr reference       back()        noexcept { return _data.back(); }
constexpr const_reference back()  const noexcept { return _data.back(); }
void fill(const T& value)              { _data.fill(value); }   // déjà via US-011
void swap(matrix& other)      noexcept(/* T swap noexcept */);  // membre, en plus du friend swap
```

### Critères d'acceptation
- [ ] Tous testés sur matrix vide → ne devrait pas compiler car `front/back` UB sur taille 0 (cf. US-038)
- [ ] Test `accessors.cpp`
