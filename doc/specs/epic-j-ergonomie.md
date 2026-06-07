# EPIC J — Ergonomie & finition

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-039 | Suite de benchmarks (Google Benchmark) | P1 | ✅ Done |
| US-050 | Cookbook Doxygen | P1 | ✅ Done |
| US-051 | `matrix_view` : itérateurs strided + `front`/`back`/`fill` | P1 | ✅ Done |
| US-052 | `matrix_view` : I/O, ctor const, vues composables | P1 | ✅ Done |
| US-053 | Constructeurs additionnels : `std::array`, `std::span`, générateur | P1 | ✅ Done |
| US-054 | `matrix::rows()` / `cols()` + `matmul` vecteur 1D | P1 | ✅ Done |
| US-055 | `CHANGELOG.md` versionné | P1 | ✅ Done |
| US-056 | Messages d'exception détaillés dans `at()` | P1 | ✅ Done |
| US-057 | Centraliser `NOLINTNEXTLINE` dans `matrix.hpp` | P1 | ✅ Done |
| US-058 | Optimiser `matrix(matrix_view<strided>)` | P1 | ✅ Done |
| US-059 | `operator-()` `constexpr` + hash combine 64-bit | P1 | ✅ Done |

---

## US-039 — Suite de benchmarks (Google Benchmark)

**Priorité :** P1 — **Dépend de :** US-026, US-031, US-033 — **Épopée :** J *(déplacée de EPIC I)*

### Spécification
- Nouveau dossier `bench/`
- Dépendance via `FetchContent` : `google/benchmark`
- Benchmarks :
  - Construction (zeros vs default)
  - Element access (operator() vs at)
  - Iteration (range-for vs index)
  - Arithmétique (m+m, m*m hadamard, matmul)
  - Comparaison vs `std::array` brut
- Job CI optionnel `benchmark` (déclenché manuellement via `workflow_dispatch`)
- Résultats archivés en artefact

### Critères d'acceptation
- [ ] `cmake --build build --target bench && ./build/bench/matrix-bench` fonctionne
- [ ] Pas de régression vs `std::array` brut sur opérations équivalentes (overhead < 5 %)

---

## US-050 — Cookbook Doxygen

**Priorité :** P1 — **Dépend de :** US-043 — **Épopée :** J

### Story
En tant qu'utilisateur, je veux trouver des recettes pratiques pour les opérations courantes sans avoir à lire l'API complète.

### Spécification technique
- Nouveau fichier `doc/cookbook.md` déclaré comme page Doxygen (`\page cookbook Cookbook`)
- Recettes minimum :
  - « Iterating over rows and columns »
  - « Filling and transforming a matrix »
  - « Comparing matrices element-wise vs lexicographic »
  - « Working with views: contiguous vs strided »
  - « Interop with `std::ranges`, `std::format`, `std::hash` »
  - « Solving Ax=b with dot, transpose, matmul »
- `doc/Doxyfile.in` : activer `GENERATE_TREEVIEW = YES` et `USE_MATHJAX = YES`
- Lien proéminent vers le Cookbook depuis `mainpage.md` et `README.md`

### Critères d'acceptation
- [ ] `cmake --build build --target doc` inclut la page Cookbook
- [ ] Chaque recette contient un exemple de code compilable (vérifié manuellement)
- [ ] Le Cookbook est accessible en 1 clic depuis la `@mainpage`
- [ ] `GENERATE_TREEVIEW = YES` et `USE_MATHJAX = YES` actifs dans `Doxyfile.in`

---

## US-051 — `matrix_view` : itérateurs strided, `front`/`back`/`fill`

**Priorité :** P1 — **Dépend de :** US-036 — **Bloque :** US-052, US-058 — **Épopée :** J

### Story
En tant qu'utilisateur, je veux itérer sur une `matrix_view<T, strided, N>` avec les algorithmes STL (`std::ranges`, `std::accumulate`) et accéder à `front()`/`back()`.

### Spécification technique
- Spécialisation `matrix_view<T, strided, N>` (1D) :
  - Ajouter un `strided_iterator` (random-access, pas contiguous) : `operator*`, `operator++`, `operator--`, `operator+(n)`, `operator-(n)`, `operator-(it)`
  - Membres : `begin()`, `end()`, `cbegin()`, `cend()`, `size()`, `front()`, `back()`, `fill(value)`
- Pour vues strided N-D (N > 1) : rendre `front()` et `back()` disponibles (`(*this)(0,...,0)` et `(*this)(last,...,last)`)
- `fill()` sur strided : boucle sur indices, pas d'accès linéaire au buffer

### Critères d'acceptation
- [x] `for (auto& v : col_view)` compile et fonctionne pour `col_view` issu de `m.col(j)`
- [x] `std::accumulate(v.begin(), v.end(), 0)` fonctionne sur une vue 1D strided
- [x] `std::ranges::sort(v)` compile (random-access iterator)
- [x] `v.fill(42)` fonctionne sur une vue strided
- [x] `v.front()` et `v.back()` disponibles sur toute vue strided

---

## US-052 — `matrix_view` : I/O, ctor const, vues composables

**Priorité :** P1 — **Dépend de :** US-051 — **Épopée :** J

### Story
En tant qu'utilisateur, je veux afficher une `matrix_view`, créer une vue read-only depuis une `const matrix&`, et chaîner des appels de slicing sur une vue.

### Spécification technique
- `operator<<` pour `matrix_view<T, contiguous, ...>` (délègue au helper `detail::print_recursive`)
- `std::formatter` pour `matrix_view<T, contiguous, ...>` (avec guard `__cpp_lib_format`)
- Ctor `matrix_view<const T, contiguous, ...>(const matrix<T, ...>&)` — conversion **explicite**, permet de créer une vue read-only depuis une `const matrix&`
- Alias : `template<class T, std::size_t... D> using const_matrix_view = matrix_view<const T, contiguous, D...>`
- `slice()`, `row()`, `col()` ajoutés sur `matrix_view<T, contiguous, ...>` (composabilité)

### Critères d'acceptation
- [ ] `std::cout << m.row(0)` compile et affiche la vue
- [ ] `std::format("{}", m.row(0))` compile (sous guard)
- [ ] `const matrix<int,3,3> cm{...}; const_matrix_view<int,3,3> v{cm};` compile
- [ ] `v.slice(1)` sur une `matrix_view<T, contiguous, ...>` retourne une nouvelle vue
- [ ] `const_matrix_view<int,3>` est un alias valide

---

## US-053 — Constructeurs additionnels : `std::array`, `std::span`, générateur

**Priorité :** P1 — **Dépend de :** US-021 — **Épopée :** J

### Story
En tant qu'utilisateur, je veux construire une matrice depuis un buffer existant (`std::array`, `std::span`) ou une fonction génératrice.

### Spécification technique
- `explicit matrix(std::array<T, linear_size> data)` — copie depuis `std::array`
- `explicit matrix(std::span<const T, linear_size> data)` — copie depuis span (C++20)
- Factory libre :
  ```cpp
  template<class T, std::size_t... D, std::invocable<std::size_t> F>
  constexpr matrix<std::invoke_result_t<F, std::size_t>, D...>
  generate(F f);  // f(linear_index) appelé pour chaque élément
  ```
- Variante multi-index (ordre des arguments = ordre des dimensions) :
  ```cpp
  template<class T, std::size_t... D, class F>
    requires std::invocable<F, decltype((void(D), std::size_t{}))...>
  constexpr matrix<std::invoke_result_t<F, ...>, D...>
  generate(F f);  // f(i0, i1, ...) pour matrice N-D
  ```

### Critères d'acceptation
- [ ] `matrix<int,3> m(std::array<int,3>{1,2,3})` compile et fonctionne
- [ ] `matrix<int,3> m(std::span<const int,3>{buf, 3})` compile
- [ ] `auto m = ysc::generate<int,3,3>([](std::size_t i, std::size_t j){ return int(i+j); })` compile
- [ ] Tests dans `test/src/construct_from_buffer.cpp`

---

## US-054 — `matrix::rows()` / `cols()` + `matmul` vecteur 1D

**Priorité :** P1 — **Dépend de :** US-036, US-033, US-051 — **Épopée :** J

### Story
En tant qu'utilisateur, je veux itérer sur les lignes et colonnes d'une matrice 2D comme sur un range, et multiplier une matrice 2D par un vecteur 1D.

### Spécification technique
- `m.rows()` (contrainte `order == 2`) : retourne un `std::ranges::view` de `matrix_view<T, contiguous, C>` (une vue par ligne)
- `m.cols()` (contrainte `order == 2`) : retourne un `std::ranges::view` de `matrix_view<T, strided, R>` (une vue par colonne)
- Surcharge `matmul` pour vecteur 1D :
  ```cpp
  template<class T, std::size_t M, std::size_t N>
  constexpr matrix<T, M> matmul(const matrix<T, M, N>& mat, const matrix<T, N>& vec);
  ```

### Critères d'acceptation
- [ ] `for (auto row_view : m.rows())` compile sur une `matrix<T, R, C>` (2D)
- [ ] `for (auto col_view : m.cols())` compile sur une `matrix<T, R, C>` (2D)
- [ ] `m.rows()` et `m.cols()` refusés à la compilation pour `order ≠ 2`
- [ ] `matmul(matrix<int,2,3>{...}, matrix<int,3>{...})` retourne `matrix<int,2>`
- [ ] Tests dans `test/src/rows_cols.cpp` et tests additionnels dans `test/src/matmul.cpp`

---

## US-055 — `CHANGELOG.md` versionné

**Priorité :** P1 — **Dépend de :** US-007 — **Épopée :** J

### Story
En tant qu'utilisateur arrivant sur le repo GitHub, je veux voir l'historique des changements directement dans le dépôt sans aller sur la page Releases.

### Spécification technique
- `CHANGELOG.md` initialisé à la racine avec toutes les releases existantes (générées via `git-cliff --config cliff.toml`)
- Format : sections `## [v0.x.0] - YYYY-MM-DD` avec sous-sections feat / fix / chore
- Le workflow `release.yml` met à jour `CHANGELOG.md` après chaque release et commit sur `develop` :
  ```yaml
  - name: Update CHANGELOG
    run: |
      git-cliff --config cliff.toml --output CHANGELOG.md
      git add CHANGELOG.md
      git commit -m "chore(release): update CHANGELOG for ${{ github.ref_name }}"
      git push origin develop
  ```

### Critères d'acceptation
- [x] `CHANGELOG.md` présent à la racine du repo, lisible via GitHub
- [x] Contient les releases v0.2.0 à v0.6.0 (premières releases disponibles dans l'historique git)
- [x] Workflow `release.yml` met à jour et commit `CHANGELOG.md` à chaque nouvelle release

---

## US-056 — Messages d'exception détaillés dans `at()`

**Priorité :** P1 — **Dépend de :** rien — **Épopée :** J

### Story
En tant que développeur qui débogue, je veux que `matrix::at()` et `matrix_view::at()` indiquent la coordonnée fautive et la taille de la dimension dans le message d'exception.

### Spécification technique
- `matrix::at()` (`matrix.hpp`, deux surcharges) : message format `"matrix::at: coordinate N is out of bounds for dimension K (size=S)"`
- `matrix_view::at()` (`matrix_view.hpp`, deux surcharges) : même format `"matrix_view::at: coordinate N is out of bounds for dimension K (size=S)"`
- Construction du message : `std::to_string` (ou `std::format` avec guard)
- Impact performance nul (uniquement sur le chemin exception)

### Critères d'acceptation
- [ ] `try { m.at(99); } catch (const std::out_of_range& e) { ... }` — `e.what()` contient `"99"` et la taille de la dimension
- [ ] Format du message documenté dans `@throws` Doxygen
- [ ] Tests dans `test/src/access.cpp`

---

## US-057 — Centraliser les `NOLINTNEXTLINE` dans `matrix.hpp`

**Priorité :** P1 — **Dépend de :** rien — **Épopée :** J

### Story
En tant que mainteneur, je veux réduire le bruit visuel des ~19 suppressions clang-tidy dans `matrix.hpp`.

### Spécification technique

Deux options (choisir celle qui minimise le diff) :

**Option A :** désactiver `cppcoreguidelines-pro-bounds-pointer-arithmetic` et `cppcoreguidelines-pro-bounds-constant-array-index` dans `.clang-tidy` sous `src/include/` avec justification commentée.

**Option B :** centraliser les calculs d'index dans des helpers `detail::` (déjà partiellement fait avec `coordinates_to_index`, `index_to_coordinates`) et mettre un seul `NOLINTNEXTLINE` par helper.

Résultat attendu : `grep -c NOLINT src/include/matrix.hpp` ≤ 5 (au lieu de ~19).

### Critères d'acceptation
- [ ] `grep -c NOLINT src/include/matrix.hpp` retourne ≤ 5
- [ ] Aucune suppression orpheline (toutes justifiées par un commentaire)
- [ ] CI clang-tidy reste verte

---

## US-058 — Optimiser `matrix(matrix_view<strided>)`

**Priorité :** P1 — **Dépend de :** US-044, US-051 — **Épopée :** J

### Story
En tant que mainteneur, je veux que le constructeur `matrix(matrix_view<strided>)` itère en O(N) plutôt que de recalculer les coordonnées via `index_to_coordinates` pour chaque élément.

### Spécification technique
- Remplacer la boucle `for i in [0, linear_size) → index_to_coordinates(i) → std::apply → operator()` par `std::copy(v.begin(), v.end(), _data.begin())` en utilisant les itérateurs strided de US-051
- Le constructeur `contiguous` reste inchangé (déjà optimal via `std::copy`)

### Critères d'acceptation
- [ ] Le ctor strided utilise les itérateurs (pas de `index_to_coordinates` en boucle)
- [ ] Tests existants `test/src/matrix_from_view.cpp` restent verts
- [ ] `constexpr` préservé si les itérateurs strided sont `constexpr`

---

## US-059 — `operator-()` `constexpr` + hash combine 64-bit

**Priorité :** P1 — **Dépend de :** rien — **Épopée :** J

### Story
En tant qu'utilisateur, je veux que l'opérateur unaire `-` soit `constexpr` et que la fonction de hash ait une bonne distribution sur les plateformes 64-bit.

### Spécification technique

**`operator-()` unaire :**
```cpp
[[nodiscard]] constexpr matrix operator-() const
    noexcept(noexcept(-std::declval<T const&>())) {
    return map([](const T& v) { return -v; });
}
```
(Actuellement sans `constexpr` ni `noexcept` conditionnel — `matrix.hpp:858-864`)

**Hash combine 64-bit :**
Remplacer dans `std::hash<ysc::matrix<T,D...>>::operator()` :
```cpp
// Avant (seed 32-bit, distribution sub-optimale sur 64-bit) :
h ^= hasher(v) + 0x9e3779b9 + (h << 6) + (h >> 2);
// Après (constante de Knuth 64-bit, Boost 1.81+) :
h ^= hasher(v) + 0x9E3779B97F4A7C15ULL + (h << 12) + (h >> 4);
```

### Critères d'acceptation
- [ ] `static_assert((-matrix<int,2>{1,-1})(0) == -1)` passe (constexpr)
- [ ] `static_assert((-matrix<int,2>{1,-1})(1) == 1)` passe
- [ ] Tests existants `test/src/hash.cpp` restent verts
- [ ] `noexcept` de `operator-()` se propage correctement (testé avec un type dont `operator-` est noexcept)
