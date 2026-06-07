# Axe 4 — Documentation cross-check

**Date.** 2026-06-07
**Branche.** `chore/v1-check-04-docs-crosscheck`
**Référence.** `doc/before-v1-checks.md` §4

## Objectif

Vérifier la cohérence transverse de la documentation textuelle entre les différents points d'entrée vus par un nouvel utilisateur : `README.md`, `doc/mainpage.md`, `doc/cookbook.md`, `doc/migration.md`, et les Named Requirements.

## Périmètre

- **IN** : `README.md`, `doc/mainpage.md`, `doc/cookbook.md`, `doc/migration.md`, sections Named Requirements dans `src/include/*.hpp`.
- **OUT** : Doxygen 0 warning (axe 3), API freeze (axe 1).

## Méthode

1. Extraction et compilation de tous les blocs ` ` `cpp ` de chaque fichier markdown.
2. Vérification des signatures citées vs API courante.
3. Vérification des versions citées (`v0.8.0`, `v1.0.0`, etc.) vs `CMakeLists.txt`.
4. Audit des liens internes et externes.
5. Vérification de la promesse SemVer (`ysc::` vs `ysc::detail::`) sur tous les supports.
6. Croisement Named Requirements vs typedefs réels.

## Résultats de l'audit

### 1. Compilation des snippets

| Fichier | Snippets testés | État | Note |
|---|---|---|---|
| `README.md` | 1 | ✅ Pass | Quick Start complet compilé avec g++-14. |
| `doc/mainpage.md` | 1 | ✅ Pass | Snippet d'introduction compilé. |
| `doc/cookbook.md` | 6 | ✅ Pass | Déjà audité par l'axe 3 ; re-vérifié manuellement. |
| `doc/migration.md` | 0 | — | Uniquement des blocs CMake et snippets partiels (hash). |

**Finding.** Aucun snippet markdown n'était cassé. La cohérence avec l'API v0.8.0/v1.0.0 est maintenue.

### 2. Versions et Signatures

| Source | Version citée | État | Action |
|---|---|---|---|
| `CMakeLists.txt` | 0.8.0 | Référence | — |
| `README.md` (FetchContent) | v1.0.0 | ✅ Anticipé | Cohérent avec la préparation v1.0.0. |
| `README.md` (find_package) | 0.7 | ❌ Divergent | **Corrigé en 1.0** pour cohérence. |
| `doc/migration.md` | v1.0.0 | ✅ Anticipé | Cible de la migration. |
| `CHANGELOG.md` | 0.6.0 | ❌ Obsolète | Voir note ci-dessous. |

**Note sur CHANGELOG.md.** Le fichier est bloqué à la version 0.6.0. Bien que le workflow de release (`release.yml`) utilise `git-cliff` pour générer le changelog lors du tag, il ne commite pas le résultat dans le dépôt. Le changelog "live" sur GitHub est donc à jour, mais le fichier local est obsolète.
*Décision :* Non-correctif dans cette PR (processus de release à revoir globalement dans l'US-042).

### 3. Liens et Navigation

Audit des liens dans `README.md` et `doc/*.md` :
- Liens GitHub (badges, codecov) : OK.
- Liens internes GitHub (`doc/migration.md`, `CONTRIBUTING.md`) : OK.
- Liens Doxygen (`topics.html`, `migration.html`) : OK (seront convertis par Doxygen).

### 4. Promesse SemVer

La frontière `ysc::` (publique) vs `ysc::detail::` (interne) est le pilier de la stabilité v1.0.0. Elle n'apparaissait que dans `migration.md`.

*Action :* Ajout d'une section "Stability & Semantic Versioning" dans `README.md` et `doc/mainpage.md` pour garantir que cette règle est visible dès le premier contact.

### 5. Named Requirements

`ysc::matrix` annonce satisfaire `Container`, `ReversibleContainer` et `ContiguousContainer`.
- Audit typedefs (`test/src/typedefs.cpp`) : 100% conformes ( `value_type`, `iterator`, `reverse_iterator`, etc.).
- Audit méthodes : `begin/end`, `rbegin/rend`, `size`, `empty`, `data`, `swap`, `operator==` sont tous présents.

## Findings & Corrections

| ID | Description | Statut |
|---|---|---|
| L1 | Typo "Dimentions" dans matrix.hpp:96. | **Corrigé** |
| L2 | Typo "aa matrix" dans matrix.hpp:126. | **Corrigé** |
| V1 | find_package(0.7) dans README au lieu de 1.0. | **Corrigé** |
| S1 | Promesse SemVer absente de README et mainpage. | **Ajouté** |
| C1 | CHANGELOG.md obsolète (bloqué à 0.6.0). | *Signalé* |

## Conclusion

**Vert après corrections.**
La documentation est désormais cohérente entre ses différents points d'entrée. La promesse de stabilité est explicitée partout et les snippets de code sont validés.

