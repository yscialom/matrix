# Tableau de bord

## Vue par épopée

| Épopée | Statut | Progression | Détail |
|--------|--------|-------------|--------|
| **A — Infrastructure & CI/CD** | 🔶 Partielle | 3/7 | ✅ US-001, US-002, US-003 · ⬜ US-004 à US-007 |
| **B — Modernisation C++20** | ✅ Terminée | 3/3 | ✅ US-008, US-009, US-010 (fusionné US-019) |
| **C — Dette technique** | 🔶 Partielle | 2/4 | ✅ US-011, US-013 · ⬜ US-012, US-014 |
| **D — Conformité STL** | ✅ Terminée | 4/4 | ✅ US-015, US-016, US-017, US-018 |
| **E — Comparaison & I/O** | 🔶 Partielle | 1/7 | ✅ US-019 · ⬜ US-020 à US-025 |
| **F — Arithmétique** | ⬜ Non démarrée | 0/4 | ⬜ US-026 à US-029 |
| **G — Algorithmes** | ⬜ Non démarrée | 0/5 | ⬜ US-030 à US-034 |
| **H — Vues & reshape** | ⬜ Non démarrée | 0/3 | ⬜ US-035 à US-037 |
| **I — Finition & release** | ⬜ Non démarrée | 0/5 | ⬜ US-038 à US-042 |

**Total : 12 / 42 US**

## EPIC A — Infrastructure & CI/CD

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-001 | Pipeline CI multi-plateforme | P0 | ✅ Done |
| US-002 | Couverture de code (gcov + lcov + Codecov) | P0 | ✅ Done |
| US-003 | Sanitizers (ASan + UBSan) | P1 | ✅ Done |
| US-004 | clang-format + vérification CI | P1 | 🔓 Disponible |
| US-005 | clang-tidy + vérification CI | P1 | 🔓 Disponible |
| US-006 | Doc Doxygen publiée sur GitHub Pages | P1 | 🔓 Disponible |
| US-007 | Release automation (semver + GitHub Releases) | P2 | 🔓 Disponible |

---

# User Stories — ysc::matrix roadmap

## Objectifs globaux

| Axe | Cible |
|-----|-------|
| Standard | C++20 (concepts, `<=>`, `consteval`, `std::span`) |
| API | STL-compliant + algèbre linéaire + vues |
| Couverture | 100 % lignes & branches |
| Qualité | clang-format, clang-tidy, sanitizers, doc Doxygen publiée |
| CI/CD | Multi-OS, multi-compilateurs, releases automatisées |

## Décisions transverses (s'appliquent à toutes les US)

### Outils & versions cibles
- **Compilateurs supportés :** GCC ≥ 12, Clang ≥ 15, MSVC ≥ 19.30 (VS 2022), Apple Clang ≥ 14
- **CMake :** ≥ 3.20 (FetchContent stable, `target_compile_features(cxx_std_20)`)
- **Tests :** GoogleTest (déjà en place — pas de migration vers Catch2/doctest)
- **Couverture :** gcov + lcov + Codecov.io
- **Doc :** Doxygen → GitHub Pages (branche `gh-pages`)
- **Format :** clang-format avec style basé sur LLVM (4-space indent, 100 cols)
- **Linter :** clang-tidy avec ruleset bugprone-*, modernize-*, performance-*, readability-*

### Conventions de code
- Namespace racine : `ysc`
- Détails internes : `ysc::detail` (renommer `_details` → `detail`, US-014)
- snake_case pour fonctions/variables, PascalCase pour templates types
- Tests dans `test/src/<feature>.cpp` ; un fichier par US fonctionnelle
- Noms de tests : `TEST(MatrixXxx, BehaviorYyy)`

### Layout mémoire & sémantique
- **Row-major** (rightmost dimension contiguë) — CONSERVÉ, ne pas changer
- Stockage : `std::array<T, (Dims*...)>` — CONSERVÉ
- Aucun overhead vs C-array : tout doit rester `constexpr`-friendly et sans branche dynamique sur les chemins chauds
- Politique d'invalidation des itérateurs : jamais invalidés (sauf swap qui suit l'élément)

### Politique d'erreur
- `operator()` : UB hors bornes (perf-critical)
- `at()` : `std::out_of_range`
- Erreurs de type/taille : à la compilation (concepts/static_assert), jamais runtime

### Granularité des PRs
- 1 US = 1 PR ≤ 400 lignes diff (hors tests)
- Chaque PR doit laisser le build vert et les tests verts
- Pas de breaking change sans bump majeur

---

## Épopées et dépendances

```
EPIC A — Infrastructure & CI/CD
  US-001 → US-002 → US-003
                  → US-004
                  → US-005
                  → US-006
                  → US-007

EPIC B — Modernisation C++20
  US-008 (bump standard) → US-009 (concepts) → US-010 (<=>)

EPIC C — Dette technique & cleanup
  US-011, US-012, US-013, US-014 (indépendants, faisables en parallèle)

EPIC D — Conformité STL
  US-015 (typedefs) → US-016 (iterators) → US-017 (size/data/empty)
                                         → US-018 (front/back/fill)

EPIC E — Comparaison & I/O
  US-019 (==/<=>) → US-020 (hash)
  US-021 (refactor ctor) → US-022 (nested init-list) → US-023 (factories)
  US-024 (ostream) → US-025 (std::format)

EPIC F — Arithmétique
  US-026 (element-wise +/-) → US-027 (element-wise */÷)
  US-028 (scalar) → US-029 (unaire)

EPIC G — Algorithmes
  US-030 (apply) → US-031 (reductions)
  US-032 (transpose) → US-033 (matmul 2D) → US-034 (dot)

EPIC H — Vues & reshape
  US-035 (matrix_view) → US-036 (slice/submatrix) → US-037 (reshape)

EPIC I — Finition & release
  US-038 (zero-dim edge case)
  US-039 (benchmarks)
  US-040 (README + examples)
  US-041 (100% coverage gate)
  US-042 (release v2.0.0)
```

---

# EPIC A — Infrastructure & CI/CD

## US-001 — Pipeline CI multi-plateforme

**Priorité :** P0 — **Dépend de :** rien — **Bloque :** US-002 à US-007

### Story
En tant que mainteneur, je veux qu'un workflow GitHub Actions compile et teste le code sur Linux/macOS/Windows × GCC/Clang/MSVC à chaque push et PR.

### Spécification technique
- Fichier : `.github/workflows/ci.yml`
- Triggers : `push` (toutes branches), `pull_request` (vers `develop` et `master`)
- Matrice :
  - `os: [ubuntu-24.04, macos-14, windows-2022]`
  - `compiler: [gcc-12, gcc-13, clang-15, clang-17, msvc, appleclang]` (filtrer par OS)
  - `build_type: [Debug, Release]`
- Étapes :
  1. `actions/checkout@v4`
  2. Installer compilateur si besoin
  3. `cmake -S . -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE`
  4. `cmake --build build --parallel`
  5. `ctest --test-dir build --output-on-failure`
- `fail-fast: false`
- Cache `~/.cache/ccache` (Linux/macOS) avec `actions/cache@v4`

### Critères d'acceptation
- [ ] Tous les jobs verts sur la PR
- [ ] Badge CI dans `README.md`
- [ ] Temps total < 6 min sur cache chaud

---

## US-002 — Couverture de code (gcov + lcov + Codecov)

**Priorité :** P0 — **Dépend de :** US-001 — **Bloque :** US-041

### Spécification technique
- Nouveau job CI `coverage` (Ubuntu + GCC seul, Debug)
- Flags : `-O0 -g --coverage -fprofile-arcs -ftest-coverage`
- Option CMake : `-DENABLE_COVERAGE=ON` pilotant ces flags
- Étapes :
  1. Build + run tests
  2. `lcov --capture --directory build --output-file coverage.info`
  3. `lcov --remove coverage.info '*/test/*' '*/_deps/*' '/usr/*' --output-file coverage.info`
  4. `codecov-action@v4` avec `coverage.info`
- Badge Codecov dans README

### Critères d'acceptation
- [ ] Rapport visible sur codecov.io
- [ ] Badge dans README
- [ ] Couverture initiale rapportée (≥ existante, pas de seuil bloquant ici — US-041 ajoutera le seuil)

---

## US-003 — Sanitizers (ASan + UBSan)

**Priorité :** P1 — **Dépend de :** US-001

### Spécification
- Job CI `sanitizers` (Ubuntu + Clang 17, Debug)
- Flags : `-fsanitize=address,undefined -fno-omit-frame-pointer`
- Option CMake : `-DENABLE_SANITIZERS=ON`
- Variables d'env : `ASAN_OPTIONS=detect_leaks=1`, `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`
- Pas de TSan (pas de threads), pas de MSan (complexe à provisionner)

### Critères d'acceptation
- [ ] Job vert sur develop
- [ ] Tout test échouant en sanitizer = build rouge

---

## US-004 — clang-format + vérification CI

**Priorité :** P1 — **Dépend de :** US-001

### Spécification
- Fichier `.clang-format` à la racine :
  ```yaml
  BasedOnStyle: LLVM
  IndentWidth: 4
  ColumnLimit: 100
  AccessModifierOffset: -4
  PointerAlignment: Left
  AllowShortFunctionsOnASingleLine: InlineOnly
  ```
- Job CI `format-check` : `clang-format --dry-run --Werror $(find src test -name '*.hpp' -o -name '*.cpp')`
- Cible CMake `format` qui applique `clang-format -i`
- Pre-commit hook documenté dans `CONTRIBUTING.md` (création optionnelle)

### Critères d'acceptation
- [ ] Tous les fichiers reformatés conformément
- [ ] Job CI bloque tout PR non formaté

---

## US-005 — clang-tidy + vérification CI

**Priorité :** P1 — **Dépend de :** US-001

### Spécification
- Fichier `.clang-tidy` à la racine :
  ```yaml
  Checks: >
    bugprone-*,
    cppcoreguidelines-*,
    modernize-*,
    performance-*,
    portability-*,
    readability-*,
    -modernize-use-trailing-return-type,
    -readability-magic-numbers,
    -cppcoreguidelines-avoid-magic-numbers
  WarningsAsErrors: '*'
  HeaderFilterRegex: 'src/include/.*\.hpp$'
  ```
- Job CI `lint` : `run-clang-tidy -p build` après cmake configure
- Génération `compile_commands.json` via `CMAKE_EXPORT_COMPILE_COMMANDS=ON`

### Critères d'acceptation
- [ ] Aucun warning clang-tidy sur le code livré
- [ ] CI rouge si nouveau warning introduit

---

## US-006 — Doc Doxygen publiée sur GitHub Pages

**Priorité :** P1 — **Dépend de :** US-001

### Spécification
- Workflow `.github/workflows/docs.yml` déclenché sur `push` vers `develop`
- Étapes : checkout, install Doxygen, `cmake --build build --target doc`, déploiement sur `gh-pages` via `peaceiris/actions-gh-pages@v3`
- Le `doc/publish.sh` existant peut être supprimé ou rendu local-only
- URL publique : `https://yscialom.github.io/matrix/`
- Badge "docs" dans README

### Critères d'acceptation
- [ ] Doc accessible sur l'URL ci-dessus
- [ ] Mise à jour automatique à chaque merge sur `develop`

---

## US-007 — Release automation (semver + GitHub Releases)

**Priorité :** P2 — **Dépend de :** US-001

### Spécification
- Workflow `.github/workflows/release.yml` déclenché sur tag `v*.*.*`
- Étapes : build, test, génération CHANGELOG via `git-cliff` (config `cliff.toml`), création release GitHub avec `softprops/action-gh-release@v1`
- Convention de commits **Conventional Commits** (feat:, fix:, chore:, etc.) — documentée dans `CONTRIBUTING.md`
- Bump des `VERSION_MAJOR/MINOR/PATCH` dans `CMakeLists.txt` à chaque release (manuel via PR, CI vérifie cohérence avec le tag)

### Critères d'acceptation
- [ ] Release v2.0.0 créée à la fin de la roadmap (cf. US-042)
- [ ] CHANGELOG généré

---

# EPIC B — Modernisation C++20

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

---

# EPIC C — Dette technique

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
- Ajouter tests `fill.cpp` : fill sur trivial, sur user-defined, sur matrix de matrix
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
- [ ] Tous les usages renommés
- [ ] Build vert

---

# EPIC D — Conformité STL

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

---

# EPIC E — Comparaison & I/O

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
- [ ] `std::unordered_set<matrix<int,3>>` compile et fonctionne
- [ ] Hash égal pour matrices égales, distinct pour matrices différentes (statistique)

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
- [ ] Init 2D fonctionne
- [ ] Mauvaise taille → `std::length_error`
- [ ] Test `nested_init.cpp`

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
- [ ] Tous testés
- [ ] `static_assert(zeros<int,2,3>()(0,0) == 0)`
- [ ] `identity<int,3>()(i,j) == (i==j ? 1 : 0)`

---

## US-024 — `operator<<` (ostream)

**Priorité :** P2 — **Dépend de :** US-016

### Spécification
- Format : `[1, 2, 3]` pour 1D, `[[1, 2], [3, 4]]` pour 2D, récursif pour N-D
- Implémentation via récursion sur `order` ; helper `detail::print_recursive`
- Utilise `<<` de `T` (donc T doit être streamable, mais SFINAE/concept friendly : pas de hard error si T pas streamable, simplement opérateur indisponible)

### Critères d'acceptation
- [ ] `std::cout << matrix<int,2,2>{1,2,3,4}` → `[[1, 2], [3, 4]]`
- [ ] Test `ostream.cpp`

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
- [ ] `std::format("{}", matrix<int,2>{1,2})` retourne `"[1, 2]"`
- [ ] Si compilateur sans `<format>` complet (Apple Clang 14), guard `#if __cpp_lib_format >= 201907L`

---

# EPIC F — Arithmétique

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

---

# EPIC G — Algorithmes

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
- [ ] Tous testés
- [ ] `static_assert` sur exemples constexpr-able

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

---

# EPIC H — Vues & reshape

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

## US-036 — Slicing / submatrix

**Priorité :** P2 — **Dépend de :** US-035

### Spécification
- Pour matrices 2D uniquement (extension N-D = autre US)
- Méthode `m.row(i)` retourne `matrix_view<T, C>`
- Méthode `m.col(j)` retourne `matrix_view<T, R>` — **mais col n'est pas contiguë** → nécessite stride. Décision : `col()` retourne `strided_matrix_view` (nouvelle classe avec stride). Autre option plus simple : `col()` copie dans `matrix<T, R>` (no view).
  - **Choix retenu :** `col()` retourne **copie** `matrix<T, R>` (simplicité ; perf acceptable). `row()` retourne `matrix_view` (zero-copy).

### Critères d'acceptation
- [ ] `m.row(0)` est un view
- [ ] `m.col(0)` est une copie
- [ ] Modif via `m.row(0)(j)` reflétée dans `m`
- [ ] Documentation explicite sur la différence

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
- [ ] `static_assert((NewD * ...) == linear_size)` à la compilation
- [ ] Mutation via reshape reflétée

---

# EPIC I — Finition & release

## US-038 — Cas particulier dimension 0

**Priorité :** P2 — **Dépend de :** US-008

### Spécification
- `matrix<int, 0>` ou `matrix<int, 2, 0, 3>` doit compiler
- `linear_size == 0` ; `_data` est `std::array<T, 0>`
- `front()`, `back()` deviennent UB → marquer `[[deprecated]]` quand `empty() == true` ? **Décision :** non, conforme à `std::array`. Doc claire.
- `fill()` no-op
- Itérateurs : `begin() == end()`

### Critères d'acceptation
- [ ] Compile sans warning
- [ ] Tests dédiés `empty_matrix.cpp`

---

## US-039 — Suite de benchmarks (Google Benchmark)

**Priorité :** P2 — **Dépend de :** US-026, US-031, US-033

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

## US-040 — Réécriture du README + dossier examples/

**Priorité :** P1 — **Dépend de :** US-018, US-023, US-026, US-033

### Spécification
- README sections : Introduction, Installation, Quick Start, Features, Examples, API overview, Building from source, Contributing
- Badges : CI, Codecov, Doc, License, C++ standard
- Dossier `examples/` avec :
  - `01_basics.cpp` — construction, accès
  - `02_arithmetic.cpp` — opérations
  - `03_linalg.cpp` — transpose, matmul
  - `04_views.cpp` — vues et slicing
- Cible CMake `examples` (option `BUILD_EXAMPLES=ON`, par défaut `OFF`)

### Critères d'acceptation
- [ ] Tous les exemples compilent et tournent
- [ ] Job CI compile les examples (option ON)

---

## US-041 — Gate couverture 100 %

**Priorité :** P1 — **Dépend de :** US-002, toutes les US fonctionnelles

### Spécification
- Modifier le job `coverage` pour échouer si `lines < 100%` ou `branches < 95%`
- Seuil branches < 100 % toléré (certaines branches inatteignables : exceptions, assert)
- Outil : `lcov --summary` + `awk` extraction + comparaison
- Justification d'exclusions documentée dans `.codecov.yml`

### Critères d'acceptation
- [ ] CI rouge si couverture < 100 %
- [ ] Toutes les lignes prod-code couvertes par tests

---

## US-042 — Release v2.0.0

**Priorité :** P0 (final) — **Dépend de :** toutes les US précédentes

### Spécification
- Bump version `CMakeLists.txt` : MAJOR=2, MINOR=0, PATCH=0
- CHANGELOG.md généré
- Tag `v2.0.0` créé sur `develop`
- Workflow release (US-007) déclenché → release GitHub publiée
- Annonce dans README

### Critères d'acceptation
- [ ] Release v2.0.0 visible sur GitHub
- [ ] Doc à jour, badges verts
- [ ] CHANGELOG complet
