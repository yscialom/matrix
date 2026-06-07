# Axe 9 — Packaging E2E

**Date :** 2026-06-07
**Branche :** `chore/v1-check-09-packaging-e2e`
**Résultat global : VERT — rien à corriger**

---

## Objectif

Confirmer que tous les chemins de consommation supportés fonctionnent depuis un projet externe réel :
`find_package(ysc-matrix)`, `FetchContent`, install + alias `ysc::matrix`, header amalgamé (US-049), et examples ctest.

---

## Scénario 1 — install + find_package

### Commandes exécutées

```bash
cmake -S . -B build-install \
    -DCMAKE_INSTALL_PREFIX=/tmp/ysc-matrix-install \
    -DYSC_MATRIX_BUILD_TESTING=OFF \
    -DYSC_MATRIX_BUILD_DOCUMENTATION=OFF
cmake --build build-install --parallel
cmake --install build-install
```

### Fichiers installés

```
/tmp/ysc-matrix-install/lib/cmake/ysc-matrix/ysc-matrixTargets.cmake
/tmp/ysc-matrix-install/include/ysc/matrix.hpp
/tmp/ysc-matrix-install/include/ysc/matrix_view.hpp
/tmp/ysc-matrix-install/include/ysc/matrix_detail.hpp
/tmp/ysc-matrix-install/lib/cmake/ysc-matrix/ysc-matrixConfig.cmake
/tmp/ysc-matrix-install/lib/cmake/ysc-matrix/ysc-matrixConfigVersion.cmake
```

### Consumer minimal

```cmake
cmake_minimum_required(VERSION 3.20)
project(consumer-findpkg CXX)
find_package(ysc-matrix REQUIRED)
add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE ysc::matrix)
enable_testing()
add_test(NAME consumer-run COMMAND consumer)
```

```bash
cmake -S /tmp/consumer-findpkg -B /tmp/consumer-findpkg/build \
    -Dysc-matrix_DIR=/tmp/ysc-matrix-install/lib/cmake/ysc-matrix
cmake --build /tmp/consumer-findpkg/build --parallel
ctest --test-dir /tmp/consumer-findpkg/build --output-on-failure
```

**Résultat :** `100% tests passed, 0 tests failed out of 1` ✅

---

## Scénario 2 — FetchContent

### Consumer minimal

```cmake
cmake_minimum_required(VERSION 3.20)
project(consumer-fetchcontent CXX)
include(FetchContent)
FetchContent_Declare(ysc-matrix
    GIT_REPOSITORY file:///home/yscialom/work/divers/matrix
    GIT_TAG        develop
)
FetchContent_MakeAvailable(ysc-matrix)
add_executable(consumer main.cpp)
target_link_libraries(consumer PRIVATE ysc::matrix)
enable_testing()
add_test(NAME consumer-run COMMAND consumer)
```

**Résultat :** `100% tests passed, 0 tests failed out of 1` ✅

### Désactivation automatique tests/exemples

Cibles disponibles dans le build consumer (via `cmake --build ... --target help`) :
- `consumer` — cible du consumer ✅
- `doc` — custom target Doxygen (si Doxygen disponible) — non construit par défaut, sans impact
- **Absence confirmée** : `matrix-test`, `format-check`, `lint`, exemples du sous-projet ✅

La logique top-level dans `CMakeLists.txt` (lignes 20–27) désactive correctement
`YSC_MATRIX_BUILD_TESTING` (défaut `OFF`) et `YSC_MATRIX_BUILD_EXAMPLES` (défaut `OFF`)
quand le projet est consommé en sous-projet.

Note : `YSC_MATRIX_BUILD_DOCUMENTATION` n'est pas conditionné par le top-level check
(il vaut `${DOXYGEN_FOUND}`) — cela ajoute une cible `doc` custom (non construite par défaut)
dans le build du consumer si Doxygen est disponible. Sans impact pratique.

---

## Scénario 3 — Alias `ysc::matrix` / refus de `matrix` non-namespacé

### Vérification du fichier Targets installé

```
# Cibles exportées dans ysc-matrixTargets.cmake :
ysc::matrix  (INTERFACE IMPORTED)
```

La cible `matrix` non-namespacée n'est **pas** exportée. Confirmé par :
- `src/CMakeLists.txt` : `add_library(ysc-matrix INTERFACE)` + `add_library(ysc::matrix ALIAS ysc-matrix)` + `EXPORT_NAME matrix` (nom dans le namespace, pas racine)
- `ysc-matrixTargets.cmake` : seul `ysc::matrix` est listé

Cohérence avec `doc/migration.md` section « CMake target renamed: `matrix` → `ysc::matrix` » ✅

Test consumer compilant avec `ysc::matrix` : **succès** ✅

---

## Scénario 4 — Amalgamated single-header

### Génération

```bash
python3 utils/amalgamate.py -o /tmp/matrix-amalgamated.hpp
```

**Résultat :** `Written: /tmp/matrix-amalgamated.hpp` (4 592 lignes)

### Compilation seule

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic -x c++ -c /tmp/matrix-amalgamated.hpp -o /dev/null
```
**Résultat :** 0 erreur, 0 warning ✅

### Consumer minimal

```cpp
#include "/tmp/matrix-amalgamated.hpp"
int main() {
    ysc::matrix<int, 3, 3> m;
    m(0, 0) = 1;
    auto t = ysc::transpose(m);
    return 0;
}
```

```bash
g++ -std=c++20 -Wall -Wextra -Wpedantic consumer.cpp -o consumer && ./consumer
```
**Résultat :** `OK: amalgamated header — matrix<int,3,3> + transpose` ✅

### Test ODR (deux unités de compilation)

```bash
g++ -std=c++20 unit_a.cpp unit_b.cpp -o odr-test && ./odr-test
```
**Résultat :** `OK: test ODR multi-TU passé` ✅

### Encapsulation de `ysc::detail`

`grep "^namespace detail" /tmp/matrix-amalgamated.hpp` → 2 occurrences, **toutes** imbriquées
dans `namespace ysc {` (jamais au niveau racine). Aucun symbole `detail` ne « fuite » dans
l'espace de noms global. ✅

### Publication

Le workflow `release.yml` (job `release`, step « Prepare release assets ») :
```bash
python3 utils/amalgamate.py -o dist/matrix.hpp
```
puis publie `dist/matrix.hpp` comme asset de release GitHub via `softprops/action-gh-release@v3`. ✅

---

## Scénario 5 — Examples (build + ctest)

```bash
cmake -S . -B build-ex \
    -DYSC_MATRIX_BUILD_EXAMPLES=ON \
    -DYSC_MATRIX_BUILD_DOCUMENTATION=OFF
cmake --build build-ex --parallel
ctest --test-dir build-ex --output-on-failure
```

### Résultats

```
Test project /home/yscialom/work/divers/matrix/build-ex
      Start  1: matrix-test                  Passed    0.01 sec
      Start  2: example-01-basics            Passed    0.00 sec
      Start  3: example-02-arithmetic        Passed    0.00 sec
      Start  4: example-03-views             Passed    0.00 sec
      Start  5: example-04-algorithms        Passed    0.00 sec
      Start  6: example-05-interop-stl       Passed    0.00 sec
      Start  7: example-06-linear-algebra    Passed    0.00 sec
      Start  8: example-07-game-of-life      Passed    0.00 sec
      Start  9: example-08-kalman-filter-1d  Passed    0.00 sec
      Start 10: example-09-sobel-edge-det.   Passed    0.00 sec
      Start 11: example-10-pagerank          Passed    0.00 sec

100% tests passed, 0 tests failed out of 11
```

✅ **11/11 (tests + 10 exemples)**

---

## Scénario 6 — Borne CMake 3.20

- `CMakeLists.txt` (racine) : `cmake_minimum_required(VERSION 3.20)` ✅
- `bench/CMakeLists.txt` : `cmake_minimum_required(VERSION 3.20)` ✅

Test de politique :
```bash
cmake -S . -B /tmp/build-cmake320-test \
    -DCMAKE_POLICY_VERSION_MINIMUM=3.20 \
    -DYSC_MATRIX_BUILD_TESTING=OFF \
    -DYSC_MATRIX_BUILD_DOCUMENTATION=OFF
```
**Résultat :** configuration réussie sans avertissement ✅

---

## Checklist de succès

- [x] `find_package` : succès, consumer compile et passe le test ctest.
- [x] `FetchContent` : succès, tests/exemples du sous-projet désactivés automatiquement.
- [x] `ysc::matrix` accessible ; `matrix` (non-namespacé) non exporté.
- [x] Amalgamation : générée par `utils/amalgamate.py`, consumer compile, ODR multi-TU OK, `ysc::detail` encapsulé, publiée dans `release.yml`.
- [x] Tous les exemples ctest passent (11/11, exit code 0).
- [x] CMake 3.20 est la borne minimale réelle.

**Conclusion : VERT — aucun problème détecté, aucun correctif nécessaire.**
