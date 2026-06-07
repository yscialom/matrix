# EPIC B — Modernisation C++20

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-008 | Bump C++17 → C++20 | P0 | ✅ Done |
| US-009 | Remplacer SFINAE par Concepts | P1 | ✅ Done |
| US-010 | Spaceship operator `<=>` (fusionné avec US-019) | P1 | ✅ Done |

---

## US-008 — Bump C++17 → C++20

**Priorité :** P0 — **Dépend de :** US-001 — **Bloque :** US-009, US-010

### Spécification
- `CMakeLists.txt` racine : `set(CMAKE_CXX_STANDARD 20)` + `set(CMAKE_CXX_STANDARD_REQUIRED ON)` + `set(CMAKE_CXX_EXTENSIONS OFF)`
- Bump `cmake_minimum_required(VERSION 3.20)`
- Vérifier que la matrice CI tolère C++20 (mettre à jour US-001 si nécessaire — Apple Clang 14 minimum)
- Aucun changement de code à ce stade — juste l'upgrade du standard

### Critères d'acceptation
- [ ] CI verte sur tous les compilateurs cibles
- [ ] `__cplusplus >= 202002L` documenté dans README

---

## US-009 — Remplacer SFINAE par Concepts

**Priorité :** P1 — **Dépend de :** US-008

### Spécification
- Introduire dans `matrix.hpp` :
  ```cpp
  template<class T, class U>
  concept matrix_convertible_from = std::convertible_to<U, T>;

  template<class... Coords>
  concept integral_coordinates = (std::integral<std::remove_cvref_t<Coords>> && ...);
  ```
- Contraindre les templates :
  - `operator()` et `at()` : `requires integral_coordinates<Coords...>`
  - Constructeurs/assignations templatés : `requires matrix_convertible_from<T, U>`
- Cela donne des messages d'erreur lisibles au lieu de SFINAE

### Critères d'acceptation
- [ ] Compilation `matrix<int,3> m; m(1.5);` produit un message lisible (pas un mur de templates)
- [ ] Tests existants restent verts
- [ ] Nouveau test `concepts.cpp` : `static_assert` sur les concepts

---

## US-010 — Spaceship operator `<=>`

**Priorité :** P1 — **Dépend de :** US-008, US-019 (regroupé)

### Note
Cette US est fusionnée avec US-019 (comparaison) pour éviter de dupliquer.
