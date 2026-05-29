# Tableau de bord

## Vue par épopée

| Épopée | Statut | Progression | Détail |
|--------|--------|-------------|--------|
| **A — Infrastructure & CI/CD** | ✅ Terminée | 7/7 | ✅ US-001, US-002, US-003, US-004, US-005, US-006, US-007 |
| **B — Modernisation C++20** | ✅ Terminée | 3/3 | ✅ US-008, US-009, US-010 (fusionné US-019) |
| **C — Dette technique** | ✅ Terminée | 4/4 | ✅ US-011, US-012, US-013, US-014 |
| **D — Conformité STL** | ✅ Terminée | 4/4 | ✅ US-015, US-016, US-017, US-018 |
| **E — Comparaison & I/O** | ✅ Terminée | 7/7 | ✅ US-019, US-020, US-021, US-022, US-023, US-024, US-025 |
| **F — Arithmétique** | ✅ Terminée | 4/4 | ✅ US-026, ✅ US-027, ✅ US-028, ✅ US-029 |
| **G — Algorithmes** | ✅ Terminée | 5/5 | ✅ US-030, ✅ US-031, ✅ US-032, ✅ US-033, ✅ US-034 |
| **H — Vues & reshape** | ✅ Terminée | 4/4 | ✅ US-035, ✅ US-036, ✅ US-037, ✅ US-044 |
| **I — Packaging & préparation v1.0.0** | 🔄 En cours | 8/10 | ✅ US-038, ✅ US-041, ✅ US-043, ✅ US-045, ✅ US-046, ✅ US-047, ✅ US-048, ✅ US-049, ⬜ US-040, US-042 |
| **J — Ergonomie & finition** | ✅ Terminée | 11/11 | ✅ US-039, ✅ US-050 à US-059 |
| **K — Extensions pre-v1** | 🔄 En cours | 7/10 | ✅ US-060, ✅ US-061, ✅ US-062, ✅ US-063, ✅ US-064, ✅ US-065, ✅ US-069, ⬜ US-066, US-067, US-068 |

**Total : 64 / 69 US**

## EPIC A — Infrastructure & CI/CD

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-001 | Pipeline CI multi-plateforme | P0 | ✅ Done |
| US-002 | Couverture de code (gcov + lcov + Codecov) | P0 | ✅ Done |
| US-003 | Sanitizers (ASan + UBSan) | P1 | ✅ Done |
| US-004 | clang-format + vérification CI | P1 | ✅ Done |
| US-005 | clang-tidy + vérification CI | P1 | ✅ Done |
| US-006 | Doc Doxygen publiée sur GitHub Pages | P1 | ✅ Done |
| US-007 | Release automation (semver + GitHub Releases) | P2 | ✅ Done |

## EPIC I — Packaging & préparation v1.0.0

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-038 | Cas particulier dimension 0 | P2 | ✅ Done |
| US-040 | Dossier `examples/` enrichi | P1 | ⬜ À faire |
| US-041 | Gate couverture 100 % | P1 | ✅ Done |
| US-042 | Tag `v1.0.0` | P0 (final) | ⬜ À faire |
| US-043 | Documentation Doxygen complète | P1 | ✅ Done |
| US-045 | Packaging CMake : cible `ysc-matrix`, alias, install, find_package | P0 | ✅ Done |
| US-046 | Correctifs docs + `.gitignore` | P0 | ✅ Done |
| US-047 | README marketing + `mainpage.md` v1 | P0 | ✅ Done |
| US-048 | Job CI consumer test | P0 | ✅ Done |
| US-049 | Amalgamation auto-générée par CI | P0 | ✅ Done |

## EPIC J — Ergonomie & finition

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

## EPIC K — Extensions pre-v1

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-060 | Réductions par axe (`sum<Axis>()`, etc.) | P2 | ✅ Done |
| US-061 | `submatrix` : extraction d'un sous-bloc N-D | P2 | ✅ Done |
| US-062 | `enumerate()` : itérateur de coordonnées | P2 | ✅ Done |
| US-063 | Opérateurs bit-à-bit pour types entiers | P2 | ✅ Done |
| US-064 | Test ASan : détection de vue dangling | P2 | ✅ Done |
| US-065 | Tests de référence linalg (valeurs pré-calculées) | P2 | ✅ Done |
| US-066 | CI Windows : cache vcpkg | P2 | ⬜ À faire |
| US-067 | Hygiène repo : `.editorconfig`, `SECURITY.md`, `CODE_OF_CONDUCT.md`, Dependabot | P2 | ⬜ À faire |
| US-068 | Migration guide : promesse de stabilité SemVer v1.0.0 | P2 | ⬜ À faire |
| US-069 | `generate` avec callable multi-index | P2 | ✅ Done |

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

### Définition of Done (DoD) — critères transverses

Toute US est considérée **Done** quand, en plus de ses critères d'acceptation propres :

- Build et tests verts (`cmake --build build --target check`)
- Aucun avertissement clang-format (`cmake --build build --target format-check`)
- Aucun avertissement clang-tidy (`cmake --build build --target lint`)
- **Documentation :** toute fonction publique (membre ou libre) ajoutée ou modifiée est documentée avec Doxygen :
  - `@brief` — description en une phrase
  - `@tparam` pour chaque paramètre template (si applicable)
  - `@param` pour chaque paramètre de fonction (si applicable)
  - `@return` si la valeur de retour est non-void
  - Un exemple compilable dans `@code`…`@endcode`
- Toute nouvelle fonction est rattachée à un groupe `@ingroup` existant (ou un nouveau groupe `@defgroup` créé), afin d'être accessible depuis la page principale de la doc Doxygen en ≤ 2 clics

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

EPIC I — Packaging & préparation v1.0.0
  US-038 (zero-dim edge case)
  US-046 (doc fixes + .gitignore)
  US-045 (CMake packaging) → US-047 (README), US-048 (CI consumer), US-049 (amalgamation)
  US-040 (examples) → dépend US-045
  US-041 (100% coverage gate)
  US-043 (Doxygen) → US-050 (cookbook, EPIC J)
  US-047 (README) → US-042 (tag v1.0.0)
  US-049 (amalgamation) → US-042

EPIC J — Ergonomie & finition
  US-039 (benchmarks)
  US-050 (cookbook) → dépend US-043
  US-051 (strided iterators) → US-052, US-058
  US-052 (view I/O + composables)
  US-053 (constructeurs additionnels)
  US-054 (rows/cols + matmul 1D) → dépend US-036, US-033
  US-055 (CHANGELOG)
  US-056 (at() messages)
  US-057 (NOLINT centralisé)
  US-058 (optim ctor strided) → dépend US-051
  US-059 (operator-() + hash 64-bit)

EPIC K — Extensions pre-v1
  US-060 (réductions par axe) → dépend US-031
  US-061 (submatrix) → dépend US-036
  US-062 (enumerate) → dépend US-016
  US-063 (bitwise) → dépend US-026
  US-064 (ASan dangling view) → dépend US-035, US-003
  US-065 (linalg reference) → dépend US-033, US-034
  US-066 (CI Windows cache)
  US-067 (hygiène repo)
  US-068 (migration guide) → dépend US-042
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
- [x] Tous les usages renommés
- [x] Build vert

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

# EPIC I — Packaging & préparation v1.0.0

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

## US-040 — Dossier `examples/` enrichi

**Priorité :** P1 — **Dépend de :** US-045 — **Épopée :** I

### Story
En tant que développeur C++ qui découvre la bibliothèque, je veux trouver des exemples progressifs et compilables couvrant toutes les fonctionnalités majeures.

### Spécification technique
- Dossier `examples/` avec 6 fichiers :
  - `01_basics.cpp` — construction, accès, itération
  - `02_arithmetic.cpp` — element-wise + scalaire + Hadamard + linalg
  - `03_views.cpp` — slice, row, col, reshape, flatten
  - `04_algorithms.cpp` — apply, map, réductions
  - `05_interop_stl.cpp` — std::ranges, std::sort, std::format, unordered_set
  - `06_linear_algebra.cpp` — résolution Ax=b illustrant dot, transpose, matmul
- Option CMake : `YSC_MATRIX_BUILD_EXAMPLES=OFF` (préfixe YSC_MATRIX_)
- Chaque exemple se compile et s'exécute de manière autonome
- Job CI optionnel qui compile les exemples avec `YSC_MATRIX_BUILD_EXAMPLES=ON`

### Critères d'acceptation
- [ ] Tous les 6 exemples compilent et tournent sans erreur
- [ ] Option CMake `YSC_MATRIX_BUILD_EXAMPLES=OFF` par défaut
- [ ] Job CI compile les examples (option ON)

---

## US-041 — Gate couverture 100 %

**Priorité :** P1 — **Dépend de :** US-002, toutes les US fonctionnelles — **Statut :** ✅ Done

### Spécification

La gate couverture est assurée par le check status Codecov `codecov/patch` (target : 100 % des lignes du diff, 100 % du projet) posté automatiquement sur chaque PR. Ce check est visible dans l'interface GitHub et constitue la gate de facto : toute régression couverture rend le check rouge, signalant au mainteneur de ne pas merger.

La couverture actuelle sur `develop` est de **100,0 %** (lignes et patch).

### Critères d'acceptation

- [x] Check `codecov/patch` rouge si couverture diff < 100 % (assuré côté Codecov server-side)
- [x] Couverture projet `develop` à 100,0 % (constaté via Codecov : 479/479 lignes)

---

## US-042 — Tag `v1.0.0`

**Priorité :** P0 (final) — **Dépend de :** toutes US P0 et P1 — **Épopée :** I

### Story
En tant que mainteneur, je veux bumper la version à `v1.0.0` une fois que toutes les US P0 et P1 sont satisfaisantes, en déclenchant manuellement le tag après revue subjective.

### Spécification technique
- Bump version `CMakeLists.txt` : MAJOR=1, MINOR=0, PATCH=0
- CHANGELOG.md généré (US-055) et à jour
- Tag `v1.0.0` créé manuellement par Yankel sur `develop` après revue
- Workflow release (US-007) déclenché → release GitHub publiée avec `matrix-amalgamated.hpp` (US-049)
- Doc à jour, badges verts

### Critères d'acceptation
- [ ] Release `v1.0.0` visible sur GitHub avec asset `matrix-amalgamated.hpp`
- [ ] CHANGELOG.md complet
- [ ] Badges CI, Codecov, Docs tous verts
- [ ] `find_package(ysc-matrix 1.0 CONFIG REQUIRED)` fonctionne (US-045)

---

## US-043 — Documentation Doxygen complète de l'API publique

**Priorité :** P1 — **Dépend de :** US-006, US-023 (toutes les US d'API déjà mergées) — **Bloque :** US-042 — **Épopée :** I

### Story
En tant qu'utilisateur de la bibliothèque, je veux que chaque fonction publique soit documentée avec Doxygen (description, paramètres, exemple de code) et accessible en ≤ 2 clics depuis la page principale de la doc.

### Spécification technique

**Contenu de chaque documentation :**
- `@brief` — une phrase décrivant le comportement
- `@tparam` pour chaque paramètre template (si applicable)
- `@param[in]`/`@param[out]` pour chaque paramètre de fonction (si applicable)
- `@return` si la valeur de retour est non-void
- `@throws` si une exception peut être levée
- Un exemple compilable dans `@code`…`@endcode`

**Options Doxyfile :**
- `GENERATE_TREEVIEW = YES` (navigation latérale)
- `USE_MATHJAX = YES` (formules LaTeX dans la doc)
- `WARN_AS_ERROR = YES` pour que tout warning devienne une erreur

**Organisation par groupes (`@defgroup` / `@ingroup`) — organisés par cas d'usage :**

| Groupe | Contenu |
|--------|---------|
| `ysc_construction` | Constructeurs, `operator=`, factories : `zeros`, `full`, `ones`, `identity`, `generate` ; `matrix(std::array)`, `matrix(std::span)` |
| `ysc_access` | `operator()`, `at()` |
| `ysc_iterators` | `begin`, `end`, `cbegin`, `cend`, `rbegin`, `rend`, `crbegin`, `crend` et variantes const |
| `ysc_capacity` | `size`, `max_size`, `empty`, `data`, `order`, `dimensions` |
| `ysc_modifiers` | `fill`, `swap` (membre et friend), `front`, `back` |
| `ysc_comparison` | `operator==`, `operator<=>` |
| `ysc_arithmetic` | `operator+`, `-`, `*`, `/`, scalaires, Hadamard, unaires |
| `ysc_algorithms` | `apply`, `map`, `sum`, `min`, `max`, `all`, `any` |
| `ysc_linalg` | `transpose`, `matmul`, `dot` |
| `ysc_views` | `slice`, `row`, `col`, `rows`, `cols`, `reshape`, `flatten` ; `matrix_view` (classe), `contiguous`, `strided`, `all_t`, `all`, `const_matrix_view` ; vues sur `matrix_view` : `row`, `col`, `fill`, `front`, `back` |
| `ysc_io` | `operator<<` (matrix + matrix_view contiguous), `std::formatter` (matrix + matrix_view contiguous) |
| `ysc_hash` | `std::hash<ysc::matrix<T, D...>>` |

**Page principale (`@mainpage`) :**
- Fichier `doc/mainpage.md`
- Courte description de la bibliothèque, mention de `ysc::all` (sentinel) dans le groupe Views & slicing
- Tableau listant les 12 groupes avec liens (`@ref`)
- Exemple "Quick Start" complet
- Lien proéminent vers Cookbook (US-050) et `examples/`
- Section « Non-goals » : broadcasting, SIMD/blocking, dimensions dynamiques, constructeur depuis `initializer_list` runtime

**Job CI :**
- Étendre le job `docs` (US-006) avec `WARN_AS_ERROR = YES` en CI
- Ou ajouter une étape `doxygen-check` dédiée (sans déploiement)

### Critères d'acceptation
- [x] Chaque fonction publique de `matrix.hpp`, `matrix_view.hpp`, `matrix_detail.hpp` possède `@brief`, `@tparam`/`@param`/`@return`/`@throws` selon applicable, et un exemple `@code`…`@endcode`
- [x] Toutes les fonctions sont rattachées à l'un des 12 groupes ci-dessus via `@ingroup`
- [x] La page `@mainpage` liste les 12 groupes ; chaque groupe est accessible en 1 clic depuis la page principale
- [x] `cmake --build build --target doc` produit zéro avertissement Doxygen
- [x] La CI est rouge si un avertissement Doxygen est introduit (`WARN_AS_ERROR = YES`)
- [x] `GENERATE_TREEVIEW = YES` et `USE_MATHJAX = YES` actifs dans `Doxyfile.in`

---

---

## US-045 — Packaging CMake : cible `ysc-matrix`, alias, install, `find_package`

**Priorité :** P0 — **Dépend de :** rien — **Bloque :** US-047, US-048, US-049 — **Épopée :** I

### Story
En tant qu'utilisateur, je veux pouvoir intégrer `ysc::matrix` dans mon projet via `find_package(ysc-matrix CONFIG REQUIRED)` ou `FetchContent`, avec la cible CMake `ysc::matrix` correctement nommée et les features C++20 propagées.

### Spécification technique
- `src/CMakeLists.txt` : renommer cible `matrix` → `ysc-matrix`, ajouter `add_library(ysc::matrix ALIAS ysc-matrix)`, ajouter `target_compile_features(ysc-matrix INTERFACE cxx_std_20)`
- `CMakeLists.txt` racine : `project(ysc-matrix VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} LANGUAGES CXX)`
- Mode consumer-only (auto-désactivation quand la lib est sous-projet `FetchContent`) :
  ```cmake
  if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
      set(YSC_MATRIX_IS_TOP_LEVEL ON)
  endif()
  option(YSC_MATRIX_BUILD_TESTING   "Build tests"         ${YSC_MATRIX_IS_TOP_LEVEL})
  option(YSC_MATRIX_BUILD_DOCUMENTATION "Build Doxygen doc" ${YSC_MATRIX_IS_TOP_LEVEL})
  option(YSC_MATRIX_BUILD_EXAMPLES  "Build examples"      OFF)
  ```
- Générer et installer `ysc-matrixConfig.cmake` + `ysc-matrixConfigVersion.cmake` via `CMakePackageConfigHelpers`
- `install(TARGETS ysc-matrix EXPORT ysc-matrixTargets)` + `install(FILES ...)` pour les 3 headers (`matrix.hpp`, `matrix_view.hpp`, `matrix_detail.hpp`) dans `include/ysc/`
- Documenter dans README : `find_package(ysc-matrix CONFIG REQUIRED)` + `target_link_libraries(my_target PRIVATE ysc::matrix)`

### Critères d'acceptation
- [ ] `target_link_libraries(my_target PRIVATE ysc::matrix)` fonctionne via `FetchContent`
- [ ] `cmake --install build && find_package(ysc-matrix CONFIG REQUIRED)` fonctionne
- [ ] `find_package(ysc-matrix 0.7 CONFIG REQUIRED)` vérifie la contrainte de version
- [ ] Quand consommé via `FetchContent`, les tests/docs de ysc::matrix ne se déclenchent PAS dans le projet hôte
- [ ] README mis à jour avec les deux méthodes d'intégration

---

## US-046 — Correctifs documentaires et `.gitignore`

**Priorité :** P0 — **Dépend de :** rien — **Épopée :** I

### Story
En tant que contributeur, je veux que la documentation de release et le `.gitignore` soient cohérents avec l'état réel du repo.

### Spécification technique
- `CONTRIBUTING.md` : remplacer `./release.sh prepare M.m.p` par `./.github/github-release.sh prepare M.m.p` (et `finalize`)
- `.gitignore` : remplacer `/build` par `/build*/`
- `doc/Doxyfile.in` : supprimer ou corriger `EXAMPLE_PATH = @PROJECT_SOURCE_DIR@/doc/sample` (répertoire inexistant → avertissement Doxygen)

### Critères d'acceptation
- [ ] `./.github/github-release.sh prepare X.Y.Z` est la commande documentée dans `CONTRIBUTING.md`
- [ ] `build-cov/` et autres `build*/` n'apparaissent plus en untracked dans `git status`
- [ ] `cmake --build build --target doc` ne produit aucun avertissement sur `EXAMPLE_PATH`

---

## US-047 — README et `mainpage.md` : positionnement marketing v1

**Priorité :** P0 — **Dépend de :** US-045 (bloquant pour FetchContent/find_package) — **Bloque :** US-042 — **Statut :** ✅ Done — **Épopée :** I

### Story
En tant que développeur C++ qui découvre le projet sur GitHub, je veux comprendre en moins de 30 secondes si `ysc::matrix` est fait pour moi.

### Spécification technique (implémentée)

**Structure du README :**
1. Titre + description marketing ("header-only C++20, zero-overhead, STL-compatible") + badges (CI, codecov, docs)
2. Section « Quick Start » — bloc de code ~15 lignes (construction, `operator()`, `at()`, compile-time metadata, range-for)
3. Section « Features » — tableau des fonctionnalités clés
4. Section « Installation » — `FetchContent` (CMake) + installation manuelle
5. Lien vers API reference (Doxygen) + Cookbook
6. Section « Building and Testing »

**`mainpage.md` :**
- Description marketing équivalente au README
- Section « Quick Start » avec bloc de code
- Section « Features » en tableau
- Section « Installation »
- Section « Cookbook » avec lien proéminent
- Section « API Reference » listant tous les groupes Doxygen par cas d'usage

### Critères d'acceptation
- [x] README présente le projet en tête avec une description marketing claire (header-only C++20, zero-overhead, STL-native)
- [x] README contient un Quick Start fonctionnel (~15 lignes de code)
- [x] README contient un tableau des fonctionnalités
- [x] README contient une section Installation avec FetchContent
- [x] README contient les liens vers la documentation Doxygen et le Cookbook
- [x] `mainpage.md` contient la même description marketing, Quick Start, Features et Installation
- [x] `mainpage.md` liste tous les groupes Doxygen par cas d'usage (API Reference)
- [x] `mainpage.md` contient un lien proéminent vers le Cookbook
- [x] Tous les snippets du README compilent (vérifiés manuellement)
- [x] CI `doc` verte (`WARN_AS_ERROR = YES` depuis US-043)

---

## US-048 — Job CI « consumer test »

**Priorité :** P0 — **Dépend de :** US-045 — **Épopée :** I

### Story
En tant que mainteneur, je veux qu'un job CI vérifie automatiquement qu'un projet externe peut consommer `ysc::matrix` via `FetchContent`.

### Spécification technique
- Nouveau job `consumer-test` dans `.github/workflows/ci.yml`
- Crée un dossier temporaire `consumer_test/` avec un `CMakeLists.txt` minimal
- `FetchContent_Declare` la lib via le SHA du commit courant (`github.sha`)
- Cible `consumer` : programme incluant `<matrix.hpp>`, construisant `matrix<int,2,3>`, accédant à `m(0,0)`
- `cmake configure + build + ctest` sur Ubuntu/GCC uniquement
- Dossier `consumer_test/` ajouté à `.gitignore`

### Critères d'acceptation
- [ ] Job `consumer-test` vert sur `develop`
- [ ] Le job échoue si l'alias `ysc::matrix` est absent ou mal configuré
- [ ] Le job échoue si `cxx_std_20` n'est pas propagé et que le consumer utilise des concepts C++20

---

## US-049 — Amalgamation auto-générée par la CI

**Priorité :** P0 — **Dépend de :** US-007 — **Bloque :** US-042 — **Épopée :** I

### Story
En tant qu'utilisateur qui veut une intégration minimale, je veux pouvoir télécharger un seul fichier header `matrix-amalgamated.hpp` depuis la release GitHub.

### Spécification technique
- Script `utils/amalgamate.py` versionné dans le repo :
  - Concatène `matrix_detail.hpp` + `matrix_view.hpp` + `matrix.hpp` (dans cet ordre)
  - Supprime les `#include` internes (`#include <matrix_detail.hpp>`, `#include <matrix_view.hpp>`)
  - Conserve tous les `#include` système (`<algorithm>`, `<array>`, etc.)
  - Encadre chaque section d'un commentaire balise (`// === BEGIN matrix_detail.hpp ===`)
- Résultat : `matrix-amalgamated.hpp` — drop-in, `#include <matrix-amalgamated.hpp>` suffit
- Le workflow `release.yml` invoque `python3 utils/amalgamate.py -o matrix-amalgamated.hpp` et joint le fichier comme asset de release
- L'ancien `matrix.hpp.gz` (incomplet, ne contient pas les 3 headers) est remplacé dans les assets
- **Aucun** fichier amalgamé checké dans le repo (ajouté à `.gitignore`)

### Critères d'acceptation
- [ ] `python3 utils/amalgamate.py` produit un fichier `matrix-amalgamated.hpp`
- [ ] `g++ -std=c++20 -x c++ matrix-amalgamated.hpp /dev/null` compile sans erreur
- [ ] Job CI `release` joint `matrix-amalgamated.hpp` en asset de release
- [ ] L'ancien `matrix.hpp.gz` incomplet est remplacé ou supprimé des assets

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

---

# EPIC J — Ergonomie & finition

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

---

# EPIC K — Extensions pre-v1

## US-060 — Réductions par axe (`sum<Axis>()`, etc.)

**Priorité :** P2 — **Dépend de :** US-031 — **Épopée :** K

### Story
En tant qu'utilisateur, je veux calculer la somme, le min, le max d'une matrice selon un axe donné, en obtenant une matrice de dimension inférieure.

### Spécification technique
- `template<std::size_t Axis> constexpr auto sum() const -> matrix<T, /* dims sans Axis */>`
- Idem pour `min<Axis>()` et `max<Axis>()`
- Implémentation : métaprogrammation pour déduire les dimensions résultantes (supprimer la dimension `Axis` du pack `Dims...`)
- Contrainte : `static_assert(Axis < order)`

### Critères d'acceptation
- [x] `matrix<int,2,3>{{1,2,3},{4,5,6}}.sum<0>()` == `matrix<int,3>{5,7,9}` (somme par colonnes)
- [x] `matrix<int,2,3>{{1,2,3},{4,5,6}}.sum<1>()` == `matrix<int,2>{6,15}` (somme par lignes)
- [x] Erreur de compilation si `Axis >= order` (contrainte `requires(Axis < order)`)
- [x] Tests dans `test/src/reductions_axis.cpp`

---

## US-061 — `submatrix` : extraction d'un sous-bloc N-D

**Priorité :** P2 — **Dépend de :** US-036 — **Épopée :** K

### Story
En tant qu'utilisateur travaillant sur du traitement d'image ou des noyaux de convolution, je veux extraire un sous-bloc d'une matrice N-D.

### Spécification technique
```cpp
template<std::size_t... NewD>
matrix_view<T, strided, NewD...>
submatrix(std::array<std::size_t, order> origin) const;
```
- Contrainte compile-time : `sizeof...(NewD) == order`
- Vérification runtime : `origin[i] + NewD[i] <= Dims[i]` pour chaque `i`, sinon `std::out_of_range`
- Retourne une vue strided (strides calculés sur la matrice source)

### Critères d'acceptation
- [ ] `m.submatrix<2,2>({1,1})` sur une `matrix<int,4,4>` retourne une vue 2×2 correcte
- [ ] Mutation via la vue se reflète dans la matrice source
- [ ] `m.submatrix<3,3>({2,2})` sur une `matrix<int,4,4>` lève `std::out_of_range`
- [ ] Tests dans `test/src/submatrix.cpp`

---

## US-062 — `enumerate()` : itérateur de coordonnées

**Priorité :** P2 — **Dépend de :** US-016 — **Épopée :** K

### Story
En tant qu'utilisateur, je veux itérer sur les éléments d'une matrice avec leurs coordonnées multi-dimensionnelles (façon `np.ndenumerate`).

### Spécification technique
- `m.enumerate()` : retourne un range de `std::pair<std::array<std::size_t, order>, T&>`
- Parcours row-major (cohérent avec l'itérateur linéaire)
- Implémentation : adaptateur sur l'itérateur linéaire, conversion `linear_index → coordinates` via `detail::index_to_coordinates`

### Critères d'acceptation
- [ ] `for (auto& [coords, val] : m.enumerate())` compile
- [ ] `coords` est un `std::array<std::size_t, order>` correct pour chaque élément
- [ ] Mutation via `val` se reflète dans `m`
- [ ] Tests dans `test/src/enumerate.cpp`

---

## US-063 — Opérateurs bit-à-bit pour types entiers

**Priorité :** P2 — **Dépend de :** US-026 — **Épopée :** K

### Story
En tant qu'utilisateur travaillant sur des masques binaires (image, cryptographie), je veux appliquer des opérateurs bit-à-bit sur des `matrix<unsigned, ...>`.

### Spécification technique
- Opérateurs membres : `operator&=`, `operator|=`, `operator^=`
- Opérateurs scalaires : `operator<<=`, `operator>>=` (shift par un entier)
- Opérateur unaire : `operator~` (NOT bit-à-bit)
- Opérateurs binaires friends : `operator&`, `operator|`, `operator^`
- Contrainte : `requires std::integral<T>` sur chaque opérateur
- Implémentation : `std::transform` comme pour les opérateurs arithmétiques

### Critères d'acceptation
- [ ] `matrix<unsigned,3>{1,2,3} & matrix<unsigned,3>{3,3,3}` compile et retourne `{1,2,3}`
- [ ] `~matrix<unsigned,3>{0,0,0}` retourne matrice de `~0u`
- [ ] Erreur de compilation pour `matrix<double,3>` (non-integral)
- [ ] Tests dans `test/src/arithmetic_bitwise.cpp`

---

## US-064 — Test ASan : détection de vue dangling

**Priorité :** P2 — **Dépend de :** US-035, US-003 — **Épopée :** K

### Story
En tant que mainteneur, je veux qu'un test sous ASan détecte une utilisation de `matrix_view` après destruction de la matrice source, pour valider le comportement attendu (UB → crash sous sanitizer).

### Spécification technique
- Fichier `test/src/matrix_view_lifetime.cpp`, compilé uniquement si `YSC_SANITIZERS_ENABLED`
- Scénario : construire une `matrix_view` sur une matrice locale dans une sous-scope, laisser la matrice être détruite, accéder à la vue → use-after-free détecté par ASan
- Le test est **intentionnellement un crash** sous ASan (use-after-free). Il doit être exclu du run normal et inclus dans un test dédié sous sanitizers
- Utiliser `GTEST_SKIP()` si `!defined(YSC_SANITIZERS_ENABLED)` pour ne pas bloquer le build normal

### Critères d'acceptation
- [ ] Le fichier compile avec `ENABLE_SANITIZERS=ON`
- [ ] Le test est skippé sans `ENABLE_SANITIZERS`
- [ ] Le comportement UB est documenté dans la docstring Doxygen de `matrix_view`

---

## US-065 — Tests de référence linalg (valeurs pré-calculées)

**Priorité :** P2 — **Dépend de :** US-033, US-034 — **Épopée :** K

### Story
En tant que mainteneur, je veux quelques tests de sanité qui comparent les résultats numériques de `matmul` et `dot` à des valeurs de référence.

### Spécification technique
- Nouveau fichier `test/src/linalg_reference.cpp`
- Valeurs de référence hard-codées (calculées avec Numpy ou à la main, pas de dépendance runtime)
- Matrices de test :
  - `matmul` : 2×2, 2×3×3×2, matrice identité × matrice quelconque
  - `dot` : 3 paires de vecteurs 1D
  - `transpose` : 2×3 → vérification symétrie
- Pas de dépendance externe (Numpy, Eigen) dans les tests

### Critères d'acceptation
- [ ] `matmul(A, B) == expected_AB` pour ≥ 3 paires de matrices
- [ ] `dot(a, b) == expected` pour ≥ 3 paires de vecteurs
- [ ] `transpose(transpose(m)) == m` pour ≥ 2 matrices
- [ ] Tous les résultats vérifiés via `static_assert` quand `constexpr`

---

## US-066 — CI Windows : cache vcpkg

**Priorité :** P2 — **Dépend de :** US-001 — **Épopée :** K

### Story
En tant que mainteneur, je veux que la CI Windows ne retélécharge pas `vcpkg`/`gtest` à chaque run pour accélérer les builds.

### Spécification technique
- Ajouter `actions/cache@v4` sur `vcpkg_installed/` (ou le dossier d'install vcpkg) dans le job Windows
- Key : hash du fichier de manifest vcpkg ou de la commande `vcpkg install gtest:x64-windows`
- Restore-keys : fallback sur un cache partiel

### Critères d'acceptation
- [ ] Second run CI Windows avec code identique : step `vcpkg install` skippé (cache hit affiché)
- [ ] Temps CI Windows réduit d'au moins 30 secondes sur cache chaud

---

## US-067 — Hygiène repo : `.editorconfig`, `SECURITY.md`, `CODE_OF_CONDUCT.md`, Dependabot

**Priorité :** P2 — **Dépend de :** rien — **Épopée :** K

### Story
En tant que contributeur externe, je veux que le projet respecte les conventions OSS standards et facilite la contribution.

### Spécification technique
- `.editorconfig` à la racine :
  - `indent_style = space`, `indent_size = 4` pour C++ et CMake
  - `indent_size = 2` pour YAML, JSON, Markdown
  - `end_of_line = lf`, `charset = utf-8`, `trim_trailing_whitespace = true`, `insert_final_newline = true`
  - Cohérent avec `.clang-format`
- `SECURITY.md` : instructions pour reporter des vulnérabilités via GitHub Issue (surface réduite : lib sans I/O, header-only)
- `CODE_OF_CONDUCT.md` : Contributor Covenant v2.1 (texte standard, adapté au nom du projet)
- `.github/dependabot.yml` :
  ```yaml
  version: 2
  updates:
    - package-ecosystem: "github-actions"
      directory: "/"
      schedule:
        interval: "monthly"
  ```

### Critères d'acceptation
- [ ] `.editorconfig` présent et cohérent avec `.clang-format` (même indent_size)
- [ ] `SECURITY.md` présent avec instructions claires
- [ ] `CODE_OF_CONDUCT.md` présent (Contributor Covenant v2.1)
- [ ] `.github/dependabot.yml` présent avec config `github-actions`

---

## US-068 — Migration guide : promesse de stabilité SemVer v1.0.0

**Priorité :** P2 — **Dépend de :** US-042 — **Épopée :** K

### Story
En tant qu'utilisateur adoptant la lib depuis une version v0.x, je veux comprendre les garanties de stabilité et les changements breaking introduits en v1.0.0.

### Spécification technique
- Nouveau fichier `doc/migration.md`
- Contenu :
  - **Promesse SemVer depuis v1.0.0** : API publique = tout hors `ysc::detail::`. `ysc::detail::` peut changer en patch.
  - **Changements breaking depuis v0.x :**
    - Renommage cible CMake `matrix` → `ysc-matrix`, alias `ysc::matrix` (US-045)
    - Changement de valeur de hash (US-059) : les `std::unordered_set<matrix<...>>` sérialisés avant v1.0.0 sont invalidés
  - **Instructions de migration** pour chaque changement (avant/après)
- Lien depuis `README.md` section Installation et depuis `mainpage.md`

### Critères d'acceptation
- [ ] `doc/migration.md` présent et lisible depuis le repo GitHub
- [ ] Tous les changements breaking v0.x → v1.0.0 listés avec instructions
- [ ] La promesse SemVer (API publique vs `ysc::detail::`) est explicitement documentée
- [ ] Lien vers `doc/migration.md` depuis `README.md` et `mainpage.md`

---

## US-069 — `generate` avec callable multi-index

**Priorité :** P2 — **Dépend de :** US-053 — **Épopée :** K

### Story
En tant qu'utilisateur construisant des matrices position-dépendantes (identité par fonction, triangulaire, Vandermonde, gradient XY, masques i*j…), je veux pouvoir passer à `ysc::generate` un callable qui reçoit les coordonnées N-D plutôt qu'un index linéaire. Aujourd'hui je dois dérouler à la main `i = k / cols; j = k % cols;`, ce qui est verbeux et fragile en 3D+.

### Spécification technique

Ajouter une **deuxième surcharge** de `ysc::generate` à côté de l'existante (`src/include/matrix.hpp:1632-1640`) :

```cpp
template <class T, std::size_t... Dims, class F>
    requires std::invocable<F, /* sizeof...(Dims) × std::size_t */>
          && std::convertible_to<
                 std::invoke_result_t<F, /* idem */>, T>
constexpr matrix<T, Dims...> generate(F f);
```

- L'overload existant (`std::invocable<F, std::size_t>`, index linéaire row-major) reste exposé et continue de compiler tel quel — **rétrocompatibilité totale**.
- Implémentation : itérer sur les indices N-D via `detail::index_to_coordinates` (déjà disponible dans `matrix_detail.hpp`, US-044) et invoquer `f` avec `std::apply` sur le tuple de coordonnées.
- Doxygen `@brief @tparam @return @code`, groupe `ysc_factory` (ou équivalent existant).
- Le callable peut être `auto` (la C++ générique) ou typé explicitement `std::size_t`.

**Cas limite à clarifier dans l'implémentation** : si `sizeof...(Dims) == 1`, un callable `[](std::size_t k){ ... }` satisfait à la fois `invocable<size_t>` et la nouvelle contrainte (1 arg). Trancher en faveur de la surcharge linéaire (rétrocompat) — par exemple via `requires(!std::invocable<F, std::size_t>)` sur la nouvelle surcharge, ou par concept plus strict.

### Critères d'acceptation
- [ ] `auto I = ysc::generate<int, 3, 3>([](std::size_t i, std::size_t j){ return i == j ? 1 : 0; });` compile et produit la matrice identité 3×3.
- [ ] `static_assert(ysc::generate<int, 2, 2>([](auto i, auto j){ return int(i + j); })(1, 1) == 2);` passe (la surcharge est `constexpr`).
- [ ] Tenseur 3D : `auto m = ysc::generate<int, 2, 3, 4>([](auto i, auto j, auto k){ return int(i*100 + j*10 + k); });` compile et `m(1, 2, 3) == 123`.
- [ ] La surcharge linéaire historique (`generate<int, N>([](std::size_t k){ return int(k); })`) compile sans modification.
- [ ] Test dédié `test/src/generate_multi_index.cpp` couvrant : 1D (équivalent linéaire vs multi-index), 2D, 3D, type non-trivial, propagation `constexpr`.
- [ ] Doxygen mis à jour avec exemple inline `@code`.
- [ ] CI verte sur toutes plateformes ; clang-format / clang-tidy clean.
