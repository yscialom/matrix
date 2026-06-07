# Axe 11 — README adoption overhaul

**Date.** 2026-06-07
**Branche.** `chore/v1-check-11-readme-overhaul`
**Référence.** `doc/before-v1-checks.md` §11
**Résultat global : Refonte appliquée.**

## Objectif

Réécrire le `README.md` (contenu et ordre) pour faciliter l'adoption de la
bibliothèque : faire passer le `Quick Start` et la proposition de valeur
au-dessus de la ligne de flottaison, et compresser la section d'installation
qui occupait l'essentiel du fichier.

## Périmètre

- **IN** : `README.md`.
- **OUT** : `doc/mainpage.md`, `doc/cookbook.md` (intacts).

## Analyse du `README.md` précédent

| Section | Lignes | Position |
|---|---|---|
| Banner + titre + badges | 1–13 | tête |
| Pitch (description) | 11–13 | tête |
| Liens API/Cookbook | 15–16 | tête |
| **Installation** (3 sous-sections) | 20–69 | **avant Quick Start** |
| Stability & SemVer | 73–80 | milieu |
| Quick Start | 84–117 | bas de page |
| Features (tableau) | 121–134 | bas |
| Building/Testing, Upgrading | 138–151 | pied |

**Problèmes constatés.**
1. **Quick Start invisible** sans scroll important — le nouveau lecteur voit
   trois sous-sections CMake avant un seul exemple de code.
2. **Section Installation surdimensionnée** : ~50 lignes sur 151 (≈ 33 %),
   au-dessus du seuil 30 % fixé par l'axe.
3. **Proposition de valeur diluée** dans un paragraphe générique ; les USPs
   (zero-overhead, constexpr end-to-end, dimensions statiques, vues zero-copy)
   ne sont mises en avant nulle part.
4. **Dépendances techniques** (C++20, header-only) noyées dans le pitch ; pas
   de section dédiée listant compilateurs et CMake supportés.

## Refonte appliquée

Nouvelle structure :

1. Banner + titre + badges (inchangé).
2. Pitch d'une phrase (resserré, met C++20 et header-only en gras).
3. **Quick Start** (immédiat, ligne 14).
4. **Why `ysc::matrix`?** — 6 bullets USPs (zero-overhead, constexpr, dimensions
   dans le type, STL-compat, vues zero-copy, header-only).
5. **Documentation** — 3 liens (API ref, Cookbook, Migration).
6. **Installation** — 3 sous-sections condensées (FetchContent, find_package,
   single-header).
7. **Requirements** — nouvelle section listant compilateurs et CMake supportés.
8. **Stability & SemVer** (conservée, identique à l'axe 4).
9. **Building from source** (compressée).
10. **Upgrading from v0.x** (lien Migration).

## Critères de succès

| Critère | Cible | Mesuré | État |
|---|---|---|---|
| Quick Start visible au premier écran | Avant le 1er fold | Démarre ligne 14 / 116 | ✅ |
| Dépendances techniques claires | C++20 + header-only explicites | Pitch + section *Requirements* dédiée | ✅ |
| Installation ≤ 30 % de la hauteur | ≤ 35 lignes / 116 | 33 lignes (57–89) = **28 %** | ✅ |
| Quick Start compile | g++ -std=c++20 -Wall -Wextra -Wpedantic | ✅ Compile et exécute (`/tmp/readme_quickstart.cpp`) | ✅ |
| Liens internes valides | tous résolvent | `CONTRIBUTING.md`, `doc/migration.md`, `doc/logo/ysc-matrix-banner.png` présents | ✅ |

## Cohérence avec les axes déjà clos

- **Axe 4 (docs cross-check, 2026-06-07).** Préserve les deux corrections
  posées par l'axe 4 sur le `README.md` :
  - `find_package(ysc-matrix 1.0 CONFIG REQUIRED)` conservé (la version 0.7
    incorrecte avait été corrigée par l'axe 4).
  - Section *Stability & Semantic Versioning* conservée intégralement.
- **Axe 6 (CI matrix, 2026-06-07).** La section *Requirements* reprend
  exactement la matrice annoncée et validée par l'axe 6 : GCC ≥ 12, Clang ≥ 15,
  MSVC ≥ 19.30 (VS 2022), Apple Clang ≥ 14, CMake ≥ 3.20.
- **Axe 9 (packaging E2E, 2026-06-07).** Les trois scénarios d'installation
  cités sont exactement ceux validés par l'axe 9 : FetchContent (tests/examples
  désactivés automatiquement par la détection top-level), find_package + alias
  `ysc::matrix`, single-header (publié dans `release.yml`).
- **Axe 10 (benchmarks, 2026-06-07).** Le claim *zero overhead vs std::array*
  cité dans le bullet « Zero overhead » est précisément celui que l'axe 10 a
  confirmé mesurément (assembly identique pour `operator()`, ratios mesurés
  ≤ 1.05 sur tous les chemins chauds).

## Conclusion

Refonte appliquée. Quick Start above-the-fold, USPs synthétisées dans une
section dédiée, Installation ramenée à 28 % de la hauteur, dépendances
techniques explicitées. Aucun élément de la documentation n'a été désynchronisé
des axes précédemment validés (4, 6, 9, 10) ; le snippet Quick Start a été
re-validé par compilation locale.
