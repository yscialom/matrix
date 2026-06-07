# User Stories — ysc::matrix roadmap

## Tableau de bord

### Vue par épopée

| Épopée | Statut | Progression | Détail |
|--------|--------|-------------|--------|
| [**A — Infrastructure & CI/CD**](epic-a-infrastructure.md) | ✅ Terminée | 7/7 | ✅ US-001, US-002, US-003, US-004, US-005, US-006, US-007 |
| [**B — Modernisation C++20**](epic-b-modernisation-cpp20.md) | ✅ Terminée | 3/3 | ✅ US-008, US-009, US-010 (fusionné US-019) |
| [**C — Dette technique**](epic-c-dette-technique.md) | ✅ Terminée | 4/4 | ✅ US-011, US-012, US-013, US-014 |
| [**D — Conformité STL**](epic-d-conformite-stl.md) | ✅ Terminée | 4/4 | ✅ US-015, US-016, US-017, US-018 |
| [**E — Comparaison & I/O**](epic-e-comparaison-io.md) | ✅ Terminée | 7/7 | ✅ US-019, US-020, US-021, US-022, US-023, US-024, US-025 |
| [**F — Arithmétique**](epic-f-arithmetique.md) | ✅ Terminée | 4/4 | ✅ US-026, ✅ US-027, ✅ US-028, ✅ US-029 |
| [**G — Algorithmes**](epic-g-algorithmes.md) | ✅ Terminée | 5/5 | ✅ US-030, ✅ US-031, ✅ US-032, ✅ US-033, ✅ US-034 |
| [**H — Vues & reshape**](epic-h-vues-reshape.md) | ✅ Terminée | 4/4 | ✅ US-035, ✅ US-036, ✅ US-037, ✅ US-044 |
| [**I — Packaging & préparation v1.0.0**](epic-i-packaging.md) | 🔄 En cours | 9/10 | ✅ US-038, ✅ US-040, ✅ US-041, ✅ US-043, ✅ US-045, ✅ US-046, ✅ US-047, ✅ US-048, ✅ US-049, ⬜ US-042 |
| [**J — Ergonomie & finition**](epic-j-ergonomie.md) | ✅ Terminée | 11/11 | ✅ US-039, ✅ US-050 à US-059 |
| [**K — Extensions pre-v1**](epic-k-extensions.md) | ✅ Terminée | 10/10 | ✅ US-060 à US-069 |

**Total : 68 / 69 US**

> Le tableau US par US de chaque épopée est dans le fichier correspondant. Mettre à jour ces tableaux **et** le compteur de progression de l'épopée ci-dessus à chaque US clôturée.

---

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
