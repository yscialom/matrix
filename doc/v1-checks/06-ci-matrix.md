# Axe 6 — CI matrix audit

**Date.** 2026-06-07
**Branche.** `chore/v1-check-06-ci-matrix`
**Référence.** `doc/before-v1-checks.md` §6

## Objectif

Confirmer que la matrice CI couvre effectivement toutes les combinaisons supportées annoncées, qu'aucun job n'est silencieusement skippé ou flaky, et que les conditions d'exécution sont saines.

## Périmètre

- **IN :** `.github/workflows/ci.yml`, `benchmark.yml`, `docs.yml`, `examples.yml`, `release.yml`.
- **OUT :** contenu fonctionnel des tests/lint/doc (couvert par les autres axes).

---

## 1. Matrice réelle vs annoncée

Support annoncé (source : `doc/user-stories.md` § Décisions transverses) :

| OS | Compilateur | Annoncé |
|----|-------------|---------|
| Linux | GCC ≥ 12 | GCC ≥ 12 |
| Linux | Clang ≥ 15 | Clang ≥ 15 |
| macOS | Apple Clang ≥ 14 | Apple Clang ≥ 14 |
| Windows | MSVC ≥ 19.30 (VS 2022) | MSVC ≥ 19.30 |

Matrice effective du job `build` dans `ci.yml` :

| OS | Compilateur | Debug | Release | Annonce couverte |
|----|-------------|:-----:|:-------:|:----------------:|
| ubuntu-24.04 | GCC 12 | ✅ | ✅ | GCC ≥ 12 ✅ |
| ubuntu-24.04 | GCC 13 | ✅ | ✅ | GCC ≥ 12 ✅ |
| ubuntu-24.04 | Clang 15 | ✅ | ✅ | Clang ≥ 15 ✅ |
| ubuntu-24.04 | Clang 17 | ✅ | ✅ | Clang ≥ 15 ✅ |
| macos-14 | Apple Clang (Xcode 15.x) | ✅ | ✅ | Apple Clang ≥ 14 ✅ |
| windows-2022 | MSVC (VS 2022) | ✅ | ✅ | MSVC ≥ 19.30 ✅ |

**Total :** 12 combinaisons build × 2 build types = 12 jobs matriciels. Toutes les cibles annoncées sont couvertes.

Jobs spécialisés complémentaires (non matriciels) :

| Job | OS / Compilateur / Mode | Ce qu'il vérifie |
|-----|-------------------------|-----------------|
| `coverage` | ubuntu-24.04 / GCC-13 / Debug | Couverture lcov → Codecov |
| `sanitizers` | ubuntu-24.04 / Clang-17 / Debug | ASan + UBSan |
| `format-check` | ubuntu-24.04 | clang-format |
| `lint` | ubuntu-24.04 | clang-tidy |
| `consumer-test` | ubuntu-24.04 / GCC-13 | FetchContent |
| `doc-check` | ubuntu-24.04 | Doxygen 0 warning |

**Observation D (documentaire) :** La version Apple Clang n'est pas épinglée via `maxim-lobanov/setup-xcode` — elle dépend de Xcode par défaut sur `macos-14` (actuellement 15.x). C'est la pratique courante pour un projet open-source ; acceptable.

**Observation E (documentaire) :** GCC 14, Clang 16 et Clang 18 sont disponibles sur ubuntu-24.04 mais non testés. La borne inférieure (GCC 12, Clang 15) ET une version ultérieure (GCC 13, Clang 17) sont couvertes — l'annonce "≥" est respectée. L'ajout d'entrées supplémentaires améliorerait la confiance mais n'est pas requis.

---

## 2. `continue-on-error`, `if:` et jobs désactivés

```
grep -rn 'continue-on-error' .github/workflows/   → 0 résultat
grep -rn 'if: false'          .github/workflows/   → 0 résultat
```

- **0 job `continue-on-error: true`.**
- **0 job désactivé silencieusement** (`if: false`, commenté, ou exclu via `matrix.exclude`).
- `fail-fast: false` est positionné sur le job `build` — correct, garantit l'exécution complète de la matrice même en cas d'échec partiel.
- Les `if:` conditionnels (Ubuntu/macOS/Windows) sont des conditions de **step**, pas de job — ils n'écartent aucune combinaison de la matrice.

---

## 3. Déclencheurs par workflow

| Workflow | Déclenché par | Justification |
|----------|---------------|---------------|
| `ci.yml` | push `develop`/`master` + PR vers `develop`/`master` | Validation principale, toutes branches |
| `docs.yml` | push `develop` uniquement | Déploiement Pages seulement à l'intégration (intentionnel) |
| `examples.yml` | push `develop` uniquement | Vitrine, pas bloquant pour les PR (intentionnel, US-040) |
| `benchmark.yml` | `workflow_dispatch` uniquement | Exécution manuelle à la demande |
| `release.yml` | tags `v*.*.*` | Automatisation de release |

Le fait que `docs.yml` et `examples.yml` ne tournent pas sur les PR est **intentionnel et documenté** (US-040).

---

## 4. Versions d'actions et reproductibilité

| Action | Version | Géré Dependabot | Statut |
|--------|---------|:---------------:|--------|
| `actions/checkout` | `@v6` | ✅ | OK |
| `actions/cache` | `@v5` | ✅ | OK |
| `actions/upload-artifact` | `@v7` | ✅ | OK |
| `codecov/codecov-action` | `@v6` | ✅ | OK |
| `peaceiris/actions-gh-pages` | `@v4` | ✅ | OK |
| `softprops/action-gh-release` | `@v3` | ✅ | OK |
| `eclipse-score/apt-install` | `@main` (**référence mutable**) | ❌ | **Corrigé** — voir §5.A |

**Note :** Dependabot (`.github/dependabot.yml`) surveille les actions GitHub mais ne peut pas mettre à jour les références vers des branches (`@main`). C'est pourquoi cette entrée avait échappé aux mises à jour automatiques.

---

## 5. Anomalies détectées et corrections apportées

### A — `eclipse-score/apt-install@main` (référence mutable)

**Problème.** 7 occurrences dans `ci.yml` (6 jobs) + `examples.yml` (1 job) utilisaient `eclipse-score/apt-install@main`, une référence vers une branche. Une telle référence est mutable : un commit ultérieur sur la branche `main` de l'action est silencieusement adopté, sans possibilité de mise à jour Dependabot. Risque : régresssion ou compromis supply-chain non détecté.

**Correction.** Remplacement par `run: sudo apt-get update -q && sudo apt-get install -y <packages>` dans les 7 occurrences — pattern déjà utilisé dans `doc-check` (ci.yml) et `release.yml`. Cette approche élimine la dépendance tierce, est cohérente avec le reste du workflow, et ne modifie pas les packages installés.

**Fichiers modifiés :** `.github/workflows/ci.yml` (6 occurrences), `.github/workflows/examples.yml` (1 occurrence).

---

### B — `docs.yml` : option CMake inexistante

**Problème.** Le step Configure de `docs.yml` utilisait `-DBUILD_DOCUMENTATION=ON`, une option qui n'existe pas dans `CMakeLists.txt`. L'option correcte est `YSC_MATRIX_BUILD_DOCUMENTATION` (ligne 31 de `CMakeLists.txt`). Le déploiement fonctionnait par accident : Doxygen étant installé avant `cmake`, `find_package(Doxygen)` réussit et l'option `YSC_MATRIX_BUILD_DOCUMENTATION` valait `${DOXYGEN_FOUND} = TRUE` par défaut. Sans Doxygen installé en amont, ou si le défaut changeait, le bug deviendrait silencieux.

**Correction.** `-DBUILD_DOCUMENTATION=ON` → `-DYSC_MATRIX_BUILD_DOCUMENTATION=ON` dans `.github/workflows/docs.yml`.

**Fichier modifié :** `.github/workflows/docs.yml`.

---

### C — `benchmark.yml` : runner flottant `ubuntu-latest`

**Problème.** `benchmark.yml` utilisait `ubuntu-latest` (alias flottant) là où tous les autres workflows épinglent `ubuntu-24.04`. Si GitHub met à jour `ubuntu-latest`, les benchmarks changent silencieusement de machine, invalidant les comparaisons historiques.

**Correction.** `ubuntu-latest` → `ubuntu-24.04` dans `.github/workflows/benchmark.yml`.

**Fichier modifié :** `.github/workflows/benchmark.yml`.

---

## 6. Historique de flakiness (50 derniers runs `ci.yml`)

Période couverte : 2026-05-29 → 2026-06-07.

| Résultat | Nombre | Détail |
|----------|:------:|--------|
| `success` | 49 | — |
| `failure` | 1 | PR `fix/doxygen-collision` (2026-06-07T09:37) |
| **Total** | **50** | |

L'unique échec sur `fix/doxygen-collision` est un échec **déterministe** du gate `format-check` (violation de format dans un commit corrigé immédiatement — run suivant vert à 09:38). Il ne s'agit pas de flakiness mais du gate fonctionnant correctement.

**Taux de flakiness effectif : 0 %** (< seuil de 5 %).

---

## Critères de succès

| Critère | Résultat |
|---------|----------|
| Tableau matrice réelle vs annoncée produit ; aucune lacune | ✅ |
| 0 job `continue-on-error` injustifié | ✅ |
| Aucun job désactivé silencieusement | ✅ |
| Flakiness < 5 % sur les 50 derniers runs develop | ✅ (0 %) |

## Verdict

**Vert — aucune lacune de couverture, aucune flakiness.** Trois anomalies techniques (A, B, C) corrigées dans cette PR : suppression de la référence mutable `@main`, correction de l'option CMake dans `docs.yml`, et épinglage du runner dans `benchmark.yml`. Deux observations documentaires (D, E) n'appellent pas de correctif.
