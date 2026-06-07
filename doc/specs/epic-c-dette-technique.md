# EPIC C — Dette technique

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-011 | Résoudre le conflit `feature-fill` | P0 | ✅ Done |
| US-012 | Cleanup CMake & typos | P0 | ✅ Done |
| US-013 | Fix bug : `operator=` templatés sans `return *this;` | P0 | ✅ Done |
| US-014 | Renommer `_details` → `detail` | P2 | ✅ Done |

---

## US-011 — Résoudre le conflit `feature-fill`

**Priorité :** P0 — **Dépend de :** rien

### Story
La branche `origin/feature-fill` ajoute `fill()` mais supprime `at()` (créée avant l'intégration de `feature-at`). Il faut intégrer `fill()` sans perdre `at()`.

### Spécification
- Cherry-pick uniquement le contenu fonctionnel de `22accdb` :
  ```cpp
  void fill(const T& value) noexcept(std::is_nothrow_copy_assignable_v<T>)
  { _data.fill(value); }
  ```
- Ne PAS importer la suppression de `at()` ni le renommage `Coords→Args` ni les modifs de `utils.hpp`
- Ajouter tests `fill.cpp` : fill sur trivial, sur user-defined, sur matrix of matrix
- Supprimer `origin/feature-fill` après merge (commande dans la PR description)

### Critères d'acceptation
- [ ] `at()` toujours présent et fonctionnel
- [ ] `fill()` testé (couverture 100%)
- [ ] Branche remote `feature-fill` supprimée

---

## US-012 — Cleanup CMake & typos

**Priorité :** P0 — **Dépend de :** rien

### Spécification
- `test/CMakeLists.txt` ligne 2 : `Tets` → `Tests`
- Bump `cmake_minimum_required(VERSION 3.0)` → `3.20`
- `set(CMAKE_CXX_STANDARD 17)` reste tant que US-008 pas merged

### Critères d'acceptation
- [ ] Aucun warning CMake
- [ ] Build identique

---

## US-013 — Fix bug : `operator=` templatés sans `return *this;`

**Priorité :** P0 — **Dépend de :** rien

### Bug
`src/include/matrix.hpp` lignes 199-200 et 214-216 :
```cpp
template<class U>
matrix& operator=(matrix<U, Dimensions...> const& other)
{ std::copy(cbegin(other._data), cend(other._data), begin(_data)); }
// MANQUE: return *this;
```
C'est de l'**UB** (function returning non-void sans return). Idem pour la version move.

### Spécification
- Ajouter `return *this;` dans les deux opérateurs templatés
- Test régression `assignment_returns_self.cpp` : `auto& ref = (m1 = m2); ASSERT_EQ(&ref, &m1);`

### Critères d'acceptation
- [ ] Bug corrigé
- [ ] Test ajouté

---

## US-014 — Renommer `_details` → `detail`

**Priorité :** P2 — **Dépend de :** rien

### Spécification
- Convention plus standard (cf. Boost, std)
- Pas de breaking change utilisateur (namespace interne)

### Critères d'acceptation
- [x] Tous les usages renommés
- [x] Build vert
