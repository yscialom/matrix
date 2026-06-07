# Audit de préparation v1.0.0 — Synthèse

**Date de clôture :** 2026-06-07
**Statut global :** VERT — 11 axes / 11 verts, aucun bloquant identifié pour `v1.0.0`.

Cet audit transverse a été conduit après les 68/69 US fonctionnelles (US-042 — tag — exclue). Chaque axe a fait l'objet d'une session dédiée, d'une branche `chore/v1-check-NN-<slug>` et, le plus souvent, d'une PR de correctifs. Le détail historique de chaque axe est dans l'historique Git (branches `chore/v1-check-NN-*` et PRs liées).

## Récapitulatif par axe

| Axe | Sujet | PR | Verdict | Correctifs livrés |
|----|------|----|--------|--------------------|
| 1 | Public API freeze audit | (intégré) | ✅ Vert | F1, F2 |
| 2 | Header self-containment & includes | (intégré) | ✅ Vert | 6 `#include` directs ajoutés |
| 3 | Doxygen completeness & 0 warning | (intégré) | ✅ Vert | `EXCLUDE_SYMBOLS` + 14 snippets `@code` corrigés |
| 4 | Documentation cross-check | (intégré) | ✅ Vert | typos + version `find_package` + section SemVer |
| 5 | Couverture & types pathologiques | #140 | ✅ Vert | nouveaux tests pathological_types |
| 6 | CI matrix audit | (intégré) | ✅ Vert | mutable `@main` éliminée + 2 fixes workflows |
| 7 | Sanitizers full sweep | #142 | ✅ Vert | examples couverts par ASan/UBSan |
| 8 | Lint cleanliness | #143 | ✅ Vert | 5 warnings + 105 NOLINT justifiés |
| 9 | Packaging E2E | #144 | ✅ Vert | aucun correctif (6/6 scénarios verts) |
| 10 | Benchmarks & overhead claim | #145 | ✅ Vert | benchmarks complétés (fill/swap, small, int) |
| 11 | README adoption overhaul | (en cours) | ✅ Vert | refonte README (Quick Start above-the-fold) |

## Correctifs notables (par axe)

- **Axe 1 — F1.** `matrix_view<contiguous>::operator matrix_view<strided>()` rendu `constexpr`.
- **Axe 1 — F2.** `matrix::enumerate_range` ctor passé en `private` (+ `friend class matrix;`).
- **Axe 2.** 6 `#include` directs ajoutés (`<tuple>`, `<cstddef>`, `<concepts>`, `<type_traits>`, `<initializer_list>`, `<utility>`) ; le bug historique `<stdexcept>` était déjà corrigé.
- **Axe 3.** `EXCLUDE_SYMBOLS = ysc::detail ysc::detail::*` ajouté au `Doxyfile.in` ; 14 snippets `@code` corrigés, dont un vrai bug `auto& [coords, val] : m.enumerate()` (l'itérateur retourne un temporaire, lier en `auto`).
- **Axe 4.** Typos `Dimentions`/`aa matrix` corrigés ; `find_package` version 0.7 → 1.0 dans le README ; section *Stability & Semantic Versioning* ajoutée au README et au `mainpage.md`.
- **Axe 5.** Couverture 100 % confirmée via Codecov (479/479 lignes) ; ajout de `test/src/pathological_types.cpp` couvrant `std::unique_ptr<int>` (move-only), `throwing_copy`, et `no_eq` (`operator==` deleted).
- **Axe 6.** Référence mutable `eclipse-score/apt-install@main` éliminée (7 occurrences, remplacées par `apt-get` direct) ; `docs.yml` : `BUILD_DOCUMENTATION` → `YSC_MATRIX_BUILD_DOCUMENTATION` ; `benchmark.yml` runner épinglé sur `ubuntu-24.04`. Flakiness mesurée : 0 % sur 50 runs `develop`.
- **Axe 7.** Job CI `sanitizers` étendu aux examples (option CMake + flags appliqués via `ysc_add_example`).
- **Axe 8.** 5 warnings `readability-math-missing-parentheses` corrigés (`slice.cpp`, `matrix_from_view.cpp`) ; 105 NOLINT* enrichis d'une justification et d'une règle nommée.
- **Axe 9.** Aucun correctif : `find_package`, `FetchContent`, alias `ysc::matrix`, amalgamation (4 592 lignes, ODR multi-TU OK, `detail::` encapsulé), examples ctest 11/11, borne CMake 3.20 — tout vert.
- **Axe 10.** Claim « zero overhead vs `std::array` » confirmé : assembly de `operator()` strictement identique à `std::array::operator[]` (`endbr64 + movslq + movl + ret`). Ratios mesurés ≤ 1.04 sur les opérations in-place. Ajouts : `bench_fill_swap.cpp` (lacune comblée), variantes `int` et small sizes, comparaisons `std::array`.
- **Axe 11.** README refondu : Quick Start above-the-fold (ligne 14), section *Why `ysc::matrix`?* listant les USPs, *Requirements* dédiée, Installation ramenée à 28 % de la hauteur.

## Décisions documentées

- **MSan refusé.** Faux positifs lourds sans libc++ instrumentée ; complexité d'infrastructure CI ; bibliothèque header-only sans allocation propre — bénéfice attendu très faible.
- **TSan refusé.** Aucune primitive thread exposée ; TSan ne couvrirait que du code utilisateur, hors périmètre.
- **Apple Clang non épinglé.** Dépend du Xcode par défaut de `macos-14` ; conforme à la pratique open-source courante.
- **`CHANGELOG.md` figé à 0.6.0.** Le workflow `release.yml` génère le changelog mais ne le commite pas dans le dépôt. La refonte du processus de release est traitée globalement par US-042.

## Pointeurs

- Branches de travail : `chore/v1-check-01-api-freeze` à `chore/v1-check-11-readme-overhaul`.
- PRs principales avec correctifs livrés : #140, #142, #143, #144, #145.
- Pour reproduire un axe localement, consulter le `## Méthode` de la branche correspondante via `git show chore/v1-check-NN-*:doc/v1-checks/NN-*.md` (rapports détaillés archivés dans l'historique Git, supprimés du tip pour ranger `doc/`).
