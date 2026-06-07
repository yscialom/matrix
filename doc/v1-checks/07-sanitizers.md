# Axe 7 — Sanitizers full sweep

## Objectif

Confirmer que ASan + UBSan couvrent l'intégralité de la suite (tests + examples) et statuer sur MSan / TSan.

## Périmètre

- `cmake/Sanitizers.cmake`
- `.github/workflows/ci.yml` — job `sanitizers`
- `test/src/*.cpp`
- `examples/*.cpp`

---

## 1. Run local complet (ASan + UBSan)

Configuration :

```bash
cmake -S . -B build-san \
  -DENABLE_SANITIZERS=ON \
  -DCMAKE_BUILD_TYPE=Debug \
  -DYSC_MATRIX_BUILD_EXAMPLES=ON
cmake --build build-san --parallel
ASAN_OPTIONS=detect_leaks=1 UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
  ctest --test-dir build-san --output-on-failure
```

Résultat :

```
 1/11 matrix-test ..................... Passed    0.19 sec
 2/11 example-01-basics .............. Passed    0.01 sec
 3/11 example-02-arithmetic .......... Passed    0.01 sec
 4/11 example-03-views ............... Passed    0.01 sec
 5/11 example-04-algorithms .......... Passed    0.01 sec
 6/11 example-05-interop-stl ......... Passed    0.01 sec
 7/11 example-06-linear-algebra ...... Passed    0.01 sec
 8/11 example-07-game-of-life ........ Passed    0.01 sec
 9/11 example-08-kalman-filter-1d .... Passed    0.01 sec
10/11 example-09-sobel-edge-detection  Passed    0.01 sec
11/11 example-10-pagerank ............ Passed    0.01 sec

100% tests passed, 0 tests failed out of 11
```

**Aucune erreur ASan / UBSan.**

---

## 2. Audit des skips `YSC_SANITIZERS_ENABLED`

| Fichier | Ligne | Condition | Test | Justification |
|---------|-------|-----------|------|---------------|
| `test/src/access.cpp` | 25 | `YSC_SANITIZERS_ENABLED \|\| _GLIBCXX_ASSERTIONS` | `access::const_no_check_outofbound` | Accès OOB intentionnel sur `operator()` (UB délibéré — comportement du chemin sans bounds checking) |
| `test/src/access.cpp` | 57 | `YSC_SANITIZERS_ENABLED \|\| _GLIBCXX_ASSERTIONS` | `access::mutable_no_check_outofbound` | Idem, variante mutable |
| `test/src/matrix_view_lifetime.cpp` | 31 | `#ifndef YSC_SANITIZERS_ENABLED` | `MatrixViewLifetime::DanglingViewUseAfterFree` | Inversé : actif **uniquement** avec ASan — `EXPECT_DEATH` vérifie que ASan détecte la heap-use-after-free |

Chaque skip est correctement documenté avec un message de `GTEST_SKIP()` explicite. Aucune correction requise.

---

## 3. Lacune identifiée et correction apportée

**Avant cet axe**, le job CI `sanitizers` ne passait pas `-DYSC_MATRIX_BUILD_EXAMPLES=ON`. Les 10 exemples n'étaient donc pas instrumentés en CI.

De plus, `cmake/Sanitizers.cmake` n'applique les flags qu'à `matrix-test` ; les cibles examples restaient non instrumentées même en ajoutant l'option au configure.

**Corrections apportées dans cette PR :**

1. **`examples/CMakeLists.txt`** — dans `ysc_add_example()`, ajout d'un bloc conditionnel appliquant les flags `-fsanitize=address,undefined -fno-omit-frame-pointer -g` à chaque cible example lorsque `ENABLE_SANITIZERS=ON`.

2. **`.github/workflows/ci.yml`** — job `sanitizers` étendu :
   - Configure : ajout de `-DYSC_MATRIX_BUILD_EXAMPLES=ON`
   - Nouveau step `Run examples` : `ctest --tests-regex '^example-'` avec les mêmes variables `ASAN_OPTIONS` / `UBSAN_OPTIONS`

---

## 4. Décision MSan (Memory Sanitizer)

**Décision : refus.**

Raisons :
- MSan requiert une libc++ entièrement instrumentée (`libc++` compilé avec MSan). L'utilisation de libstdc++ génère des faux positifs importants.
- La mise en place en CI nécessite un environnement Clang + libc++ instrumenté, absent de nos runners ubuntu-24.04 standard.
- `ysc::matrix` est une bibliothèque header-only pure-template sans allocation dynamique propre (le stockage est `std::array`). Le risque de vrai bug MSan est très faible.

Conclusion : la complexité d'infrastructure l'emporte sur le bénéfice attendu. Décision révisable si la bibliothèque évolue vers un stockage dynamique.

---

## 5. Décision TSan (Thread Sanitizer)

**Décision : refus.**

`ysc::matrix` n'expose aucune primitive de synchronisation et ne gère pas de concurrence. TSan ne pourrait détecter des data races que dans du code utilisateur, hors périmètre de la bibliothèque. Aucune valeur ajoutée.

---

## Critères de succès

- [x] Run local complet (tests + examples) sous ASan+UBSan : 0 sanitizer error.
- [x] Chaque skip `YSC_SANITIZERS_ENABLED` documenté avec message explicite.
- [x] Décision MSan : refus documenté.
- [x] Décision TSan : refus documenté.
- [x] Job CI `sanitizers` étendu aux examples.

## Conclusion

**Vert.** Un seul problème détecté (examples non couverts en CI) — corrigé dans cette PR. La suite complète passe sous ASan+UBSan sans erreur.
