# Changelog

All notable changes to this project will be documented in this file.

## [0.6.0] - 2026-05-14

### Features
- **US-044:** Constructeur matrix(matrix_view) (owning ← view)
- **US-037:** reshape() et flatten() zero-copy
- **US-036:** slice() générique N-D, matrix_view unifié

### Miscellaneous
- Use eclipse-score/apt-install to cache apt packages in CI

## [0.5.0] - 2026-05-11

### Features
- **US-035:** matrix_view non-owning read/write view
- **US-034:** dot() produit scalaire 1D
- **US-033:** matmul() produit matriciel 2D
- **US-032:** transpose() pour matrices 2D
- **US-031:** Réductions : sum, min, max, all, any
- **US-030:** apply() et map() (algorithmes élément-par-élément)

### Bug Fixes
- Couverture de code — instantiation explicite + CMakeLists manquants
- Epic G review — constexpr apply/map, concept += matmul/dot, doc fixes

### Miscellaneous
- Move some files around
- Overall .gitignore improvements
- Fix Codecov comment config

## [0.4.0] - 2026-05-03

### Features
- **US-028 + US-029:** Arithmétique scalaire et opérateurs unaires
- **US-026 + US-027:** Opérateurs arithmétiques élément-par-élément

## [0.3.0] - 2026-05-03

### Features
- **US-020:** Spécialisation std::hash pour ysc::matrix
- **US-025:** Spécialisation std::formatter pour ysc::matrix

### Miscellaneous
- Désactiver les commentaires Codecov sur les PRs
- Exclure test/ du rapport Codecov

## [0.2.4] - 2026-05-03

### Miscellaneous
- Enrichir les assets de la GitHub Release

## [0.2.3] - 2026-05-03

### Bug Fixes
- Corriger le chemin d'extraction tar de git-cliff

## [0.2.2] - 2026-05-03

### Features
- **US-024:** operator<< (ostream) pour ysc::matrix

### Bug Fixes
- Épingler git-cliff v2.13.1 dans le workflow de release

### Miscellaneous
- Script release.sh + mise à jour CONTRIBUTING.md
- Silence Codecov PR comments when coverage is unchanged

## [0.2.1] - 2026-05-03

### Bug Fixes
- Bump version numbers
- Épingler git-cliff v2.13.1 dans le workflow de release

## [0.2.0] - 2026-05-03

### Features
- **US-014:** Renommer namespace _details en detail
- **US-022:** Constructeur depuis initializer_list imbriquée (2D)
- **US-023:** Factories zeros, ones, full, identity
- **US-021:** Refactor du constructeur variadic
- **US-009:** Remplacer SFINAE par Concepts
- **US-018:** front(), back(), swap() membre
- **US-011:** fill() : intégration propre depuis feature-fill
- **US-019:** operator== et operator<=> (defaulted)
- **US-017:** size(), empty(), data(), max_size()
- **US-016:** Itérateurs (begin/end et co.)
- **US-015:** Typedefs membres STL-compatibles
- **US-008:** Bump C++ standard from C++17 to C++20
- **US-001:** Pipeline CI multi-plateforme
- **US-013:** Fix UB: return *this manquant dans les operator= templatés

### Bug Fixes
- Guard test subdirectory behind BUILD_TESTING to fix docs CI

### Documentation
- Improve README and Doxygen landing page
- Ajouter tableau de bord (vue par épopée + détail Epic A)

### Miscellaneous
- **US-007:** Release automation (semver + GitHub Releases)
- **US-006:** Doc Doxygen publiée sur GitHub Pages
- **US-005:** clang-tidy + vérification CI
- **US-004:** clang-format + vérification CI
- **US-003:** Sanitizers (ASan + UBSan)
- **US-002:** Couverture de code (gcov + lcov + Codecov)
- Add ysc::matrix::at overload set
- Add API documentation
- Matrix constructors, assignment operators and destructor
- Minimal impl with cmake + gtest + dummy test + headeronly library
