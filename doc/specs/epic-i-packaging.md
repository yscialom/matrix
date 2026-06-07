# EPIC I — Packaging & préparation v1.0.0

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-038 | Cas particulier dimension 0 | P2 | ✅ Done |
| US-040 | Dossier `examples/` enrichi | P1 | ✅ Done |
| US-041 | Gate couverture 100 % | P1 | ✅ Done |
| US-042 | Tag `v1.0.0` | P0 (final) | ⬜ À faire |
| US-043 | Documentation Doxygen complète | P1 | ✅ Done |
| US-045 | Packaging CMake : cible `ysc-matrix`, alias, install, find_package | P0 | ✅ Done |
| US-046 | Correctifs docs + `.gitignore` | P0 | ✅ Done |
| US-047 | README marketing + `mainpage.md` v1 | P0 | ✅ Done |
| US-048 | Job CI consumer test | P0 | ✅ Done |
| US-049 | Amalgamation auto-générée par CI | P0 | ✅ Done |

> US-044 (`matrix(matrix_view)`) est rattachée à [EPIC H — Vues & reshape](epic-h-vues-reshape.md).

---

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

**Priorité :** P1 — **Dépend de :** US-045, US-053, US-054, US-056, US-060, US-061, US-062, US-063, US-069 — **Épopée :** I

### Story
En tant que développeur C++ qui découvre la bibliothèque, je veux trouver d'une part des exemples progressifs et compilables couvrant **toutes** les fonctionnalités majeures (EPIC A-K), et d'autre part des exemples vitrines mobilisant transversalement ces fonctionnalités sur des cas d'usage réels, afin de juger rapidement si `ysc::matrix` répond à mon besoin.

### Spécification technique

Dossier `examples/` avec **10 fichiers** organisés en deux groupes.

#### Groupe 1 — Exemples topiques progressifs (couverture par feature)

- `01_basics.cpp` — construction, accès, itération.
  - Doit inclure : `operator()` / `at()`, message d'exception détaillé d'`at()` (US-056), construction depuis `std::array` et `std::span` (US-053), `generate` linéaire et multi-index (US-053 + US-069), `rows()` / `cols()` (US-054).
- `02_arithmetic.cpp` — element-wise + scalaire + Hadamard + unaires.
  - Doit inclure : `operator-()` unaire `constexpr` (US-059), opérateurs bit-à-bit `&` / `|` / `^` / `~` / `<<=` / `>>=` sur un `matrix<unsigned, …>` (US-063).
- `03_views.cpp` — `slice`, `row`, `col`, `reshape`, `flatten`, **`submatrix` N-D** (US-061), composition de vues (US-052), const-correctness des vues (`matrix_view<const T, …>` vs `matrix_view<T, …>`).
- `04_algorithms.cpp` — `apply`, `map`, réductions scalaires (`sum`, `min`, `max`, `all`, `any`), **réductions par axe** (`sum<0>()`, `min<1>()`, etc. — US-060), **`enumerate()`** pour itération coordonnées-valeur (US-062).
- `05_interop_stl.cpp` — `std::ranges`, `std::sort` sur `flatten()`, `std::format`, `std::unordered_set<matrix<…>>` (via `std::hash`), interop `std::span`.
- `06_linear_algebra.cpp` — `dot`, `transpose`, `matmul` matrice × matrice **et matrice × vecteur 1D** (US-054), illustré par une résolution Ax=b par les équations normales.

#### Groupe 2 — Exemples vitrines (transversaux, orientés cas d'usage)

Chaque vitrine est un programme autonome qui produit une sortie lisible et démontre la valeur de `ysc::matrix` sur un domaine concret.

- `07_game_of_life.cpp` — Automate de Conway sur une grille 8×8, ~10 générations animées en console.
  - Mobilise : `enumerate()` (US-062), `submatrix<3, 3>` pour le voisinage de Moore (US-061), `map` pour la transition, `sum()` sur sous-blocs, `operator<<` pour l'affichage.
  - Vitrine du combo **`enumerate` + `submatrix`** comme outil de stencil 2D.
- `08_kalman_filter_1d.cpp` — Filtre de Kalman constant-velocity (état position + vitesse) sur 20 mesures bruitées.
  - Mobilise : matrices `constexpr` F / H / Q / R, `matmul` matrice × matrice et matrice × vecteur (US-054), `transpose`, `identity<…>()`, `zeros<…>()`, `dot`, arithmétique scalaire.
  - Vitrine du domaine **robotique / fusion de capteurs** ; montre que toute l'algèbre nécessaire tient dans `<matrix.hpp>`.
- `09_sobel_edge_detection.cpp` — Détecteur de contours Sobel sur une petite image synthétique 16×16.
  - Mobilise : kernels `constexpr` Gx et Gy = `transpose(Gx)`, `submatrix<3, 3>` pour le voisinage (US-061), produits élément-par-élément, `sum()` pour la convolution, `map` pour la magnitude, `std::format` pour l'affichage ASCII-art.
  - Vitrine du domaine **traitement d'image** ; montre que les kernels classiques se déclarent à la compilation.
- `10_pagerank.cpp` — Power iteration sur un petit graphe orienté à 6 nœuds.
  - Mobilise : `generate` multi-index pour construire la matrice de transition (US-069), `matmul` matrice × vecteur (US-054), `transpose`, `sum` + `apply` pour la normalisation L1, `dot` pour le test de convergence, `std::format` pour l'affichage du ranking final.
  - Vitrine du domaine **algorithmes de graphe / algèbre linéaire numérique**.

#### Contraintes communes à tous les exemples

- Chaque fichier est un programme autonome compilable indépendamment, avec sa propre `main()` produisant une sortie textuelle utile.
- Compilation sans warning sous `-Wall -Wextra -Wpedantic` (les exemples respectent la DoD transverse comme tout code du repo : clang-format et clang-tidy verts).
- Pas de dépendance externe (ni Eigen, ni stb_image, etc.) — uniquement la STL et `ysc::matrix`.
- Header inclus via `#include <matrix.hpp>` (forme « consumer » comme dans la doc et le job consumer-test US-048), pas chemin relatif.
- Numérotation à 2 chiffres (`01_` à `10_`) pour préserver le tri lexicographique.

#### Build & options CMake

- Option CMake `YSC_MATRIX_BUILD_EXAMPLES=OFF` par défaut (préfixe `YSC_MATRIX_` aligné sur l'usage actuel).
- Quand `ON`, chaque `.cpp` produit un exécutable `example-NN-name` (cible CMake distincte).
- Les exemples se compilent contre la cible in-tree `ysc::matrix` (pas via `find_package`) — le job consumer-test (US-048) reste seul responsable de valider l'intégration via packaging.

#### Job CI

- Nouveau job `examples` (dans `.github/workflows/ci.yml` ou workflow séparé `examples.yml`), déclenché **uniquement sur `push: branches: [develop]`** — pas sur `pull_request`.
  - Rationale : les exemples bougent rarement, le budget CI PR est déjà saturé par les 30+ jobs existants ; une régression éventuelle est détectée au merge et corrigée en hotfix court.
- Le job compile les exemples avec `YSC_MATRIX_BUILD_EXAMPLES=ON` sur **un seul** runner Ubuntu/GCC-13/Release (pas de matrice complète — un seul compilateur suffit pour cette gate de non-régression).
- Chaque exemple est enregistré comme test `ctest` qui vérifie uniquement que l'exécutable termine avec code 0 (pas de comparaison de sortie — les vitrices contiennent du flottant et de l'aléatoire ensemencé).

### Critères d'acceptation
- [ ] Dossier `examples/` contient les 10 fichiers (`01`-`06` topiques + `07`-`10` vitrines) listés ci-dessus.
- [ ] Chaque exemple compile et tourne sans erreur (code de retour 0) sur la config CI de référence (Ubuntu/GCC-13/Release).
- [ ] Aucun warning sous `-Wall -Wextra -Wpedantic`, aucun warning clang-format, aucun warning clang-tidy (DoD transverse).
- [ ] Option CMake `YSC_MATRIX_BUILD_EXAMPLES=OFF` par défaut ; `-DYSC_MATRIX_BUILD_EXAMPLES=ON` active la construction des 10 cibles.
- [ ] Job CI `examples` déclenché uniquement sur `push: branches: [develop]`, vert sur develop.
- [ ] Les 10 fichiers couvrent collectivement, au minimum, tous les groupes Doxygen : `ysc_construction`, `ysc_iterators`, `ysc_capacity`, `ysc_modifiers`, `ysc_enumerate`, `ysc_access`, `ysc_comparison`, `ysc_arithmetic`, `ysc_algorithms`, `ysc_linalg`, `ysc_io`, `ysc_hash`, `ysc_views`.

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
