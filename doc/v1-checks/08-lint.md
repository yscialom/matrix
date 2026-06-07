# Axe 8 — Lint cleanliness

**Branche :** `chore/v1-check-08-lint`
**Date :** 2026-06-07

---

## Résultat global : ✅ vert

- `clang-format --dry-run --Werror` (version 18.1.8, identique CI) : **0 diff** sur `src/`, `test/`, `examples/`
- `clang-tidy --warnings-as-errors='*'` (via `run-clang-tidy`) : **0 warning** sur les 3 sous-arbres
- 100 % des 105 occurrences `NOLINT*` ont une règle nommée + commentaire de justification
- `format-check` et `lint` CI couvrent bien `examples/`

---

## Versions utilisées

| Outil          | Version locale | Version CI (ubuntu-24.04) |
|----------------|---------------|--------------------------|
| clang-format   | 21.1.8 (via Docker 18.1.8 pour le check) | 18.1.8 (apt default) |
| clang-tidy     | 21.1.8        | apt default (≥ 18)       |

La cible `format` Docker (`utils/clang-format/Dockerfile`, image `alpine:3.21 / clang18-extra-tools=18.1.8-r2`) garantit l'alignement de version avec le CI.

---

## Corrections apportées

### Warnings clang-tidy (2 fichiers, 5 occurrences)

**Règle :** `readability-math-missing-parentheses` (introduite en LLVM 18)

| Fichier | Ligne | Expression corrigée |
|---------|-------|---------------------|
| `test/src/slice.cpp` | 57–60 | `sizeof(int*) + 1 * sizeof(...)` → `sizeof(int*) + (1 * sizeof(...))` |
| `test/src/matrix_from_view.cpp` | 79, 95, 190 | `i * 12 + j * 4 + k + 1` → `(i * 12) + (j * 4) + k + 1` |

### Justifications manquantes (NOLINT sans commentaire explicatif)

Ajout de commentaires de justification concrets sur 105 occurrences `NOLINT*`, dont les suivantes manquaient :

| Fichier | Règle | Justification ajoutée |
|---------|-------|-----------------------|
| `src/include/matrix_view.hpp:150` | `google-explicit-constructor` | Conversion implicite de `matrix&` en vue — design intentionnel (analogue à `string_view`) |
| `src/include/matrix_view.hpp:171` | `google-explicit-constructor` | Même raison pour `const matrix&` |
| `src/include/matrix_view.hpp:216` | `google-explicit-constructor` | Widening implicite `contiguous → strided` — perte d'information nulle |
| `src/include/matrix_view.hpp:850` | `google-explicit-constructor` | Conversion `iterator → const_iterator`, idiome standard |
| `src/include/matrix_detail.hpp:89` | `misc-no-recursion` | Récursion bornée par le nombre fini de dimensions |
| `src/include/matrix_detail.hpp:236` | `bugprone-easily-swappable-parameters` | Paramètres sémantiquement distincts (extents vs indices) |
| `test/src/construct.cpp` | multiple | Justification sur tous les `NOLINT` inline du fichier |
| `test/src/assignment_returns_self.cpp:17` | `performance-move-const-arg` | Teste le move assignment templated sur type trivial |
| `test/src/empty_matrix.cpp:28` | `performance-unnecessary-copy-initialization` | Teste le constructeur de copie sur matrice vide |
| `test/src/matrix_view.cpp:43,426` | `performance-unnecessary-copy-initialization`, `misc-redundant-expression` | Intentionnel (test de partage de pointeur, test de l'ordre spaceship) |
| `test/src/matrix_view_io.cpp:137,144,152,160` | `bugprone-unused-return-value,clang-analyzer-cplusplus.Move` | Retour ignoré intentionnel dans `ASSERT_THROW` |
| `test/src/submatrix.cpp:109,116,123,130,137,141` | `cppcoreguidelines-avoid-capturing-lambda-coroutines` | Lambda capturante, pas une coroutine |
| `test/src/pathological_types.cpp:32,120` | multiple | Justification ajoutée |
| `test/src/matrix_view_lifetime.cpp:40` | `cppcoreguidelines-owning-memory` | Simulation intentionnelle de use-after-free pour test ASan |

---

## Audit `.clang-tidy`

Le fichier `.clang-tidy` contient **8 exclusions globales** :

| Règle exclue | Raison |
|-------------|--------|
| `modernize-use-trailing-return-type` | Style personnel — retour avant est plus lisible ici |
| `readability-magic-numbers` | Constantes littérales omniprésentes dans une lib mathématique |
| `cppcoreguidelines-avoid-magic-numbers` | Alias du précédent |
| `readability-identifier-length` | Identifiants courts (`i`, `T`) légitimes dans des templates |
| `readability-function-cognitive-complexity` | Fonctions template complexes par nature |
| `cppcoreguidelines-pro-bounds-constant-array-index` | Faux positifs dans les boucles `for (size_t i = 0; i < n; ++i)` sur std::array |
| `cppcoreguidelines-pro-bounds-pointer-arithmetic` | Arithmétique de pointeur interne aux itérateurs — voulue et bornée |
| `cppcoreguidelines-pro-bounds-array-to-pointer-decay` | Idem |

Note : Les 5 premières exclusions sont celles d'US-005. Les 3 dernières ont été ajoutées lors de la consolidation des NOLINTNEXTLINE (US-057). Elles sont toutes justifiées.

Observation : les nombreuses `NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-*)` dans les fichiers `src/` et `test/` sont désormais doublement supprimées (règle globalement désactivée dans `.clang-tidy` ET NOLINTNEXTLINE). Ces NOLINTNEXTLINE sont techniquement redondantes mais pas incorrectes — elles documentent les points d'attention au niveau du code. Pas retirées dans cette PR (nettoyage hors scope).

---

## Couverture CI

| Job CI | `src/` | `test/` | `examples/` |
|--------|--------|---------|-------------|
| `format-check` | ✅ | ✅ | ✅ (`find src test examples ...`) |
| `lint` | ✅ | ✅ | ✅ (`-DYSC_MATRIX_BUILD_EXAMPLES=ON`) |

---

## Critères de succès

- [x] `clang-format --dry-run --Werror` : 0 diff sur les 3 sous-arbres
- [x] `clang-tidy --warnings-as-errors='*'` : 0 warning sur les 3 sous-arbres
- [x] 100 % des `NOLINT*` ont une règle nommée + justification
- [x] Version clang-format utilisée en local (Docker 18.1.8) = celle en CI
