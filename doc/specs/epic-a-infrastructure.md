# EPIC A — Infrastructure & CI/CD

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-001 | Pipeline CI multi-plateforme | P0 | ✅ Done |
| US-002 | Couverture de code (gcov + lcov + Codecov) | P0 | ✅ Done |
| US-003 | Sanitizers (ASan + UBSan) | P1 | ✅ Done |
| US-004 | clang-format + vérification CI | P1 | ✅ Done |
| US-005 | clang-tidy + vérification CI | P1 | ✅ Done |
| US-006 | Doc Doxygen publiée sur GitHub Pages | P1 | ✅ Done |
| US-007 | Release automation (semver + GitHub Releases) | P2 | ✅ Done |

---

## US-001 — Pipeline CI multi-plateforme

**Priorité :** P0 — **Dépend de :** rien — **Bloque :** US-002 à US-007

### Story
En tant que mainteneur, je veux qu'un workflow GitHub Actions compile et teste le code sur Linux/macOS/Windows × GCC/Clang/MSVC à chaque push et PR.

### Spécification technique
- Fichier : `.github/workflows/ci.yml`
- Triggers : `push` (toutes branches), `pull_request` (vers `develop` et `master`)
- Matrice :
  - `os: [ubuntu-24.04, macos-14, windows-2022]`
  - `compiler: [gcc-12, gcc-13, clang-15, clang-17, msvc, appleclang]` (filtrer par OS)
  - `build_type: [Debug, Release]`
- Étapes :
  1. `actions/checkout@v4`
  2. Installer compilateur si besoin
  3. `cmake -S . -B build -DCMAKE_BUILD_TYPE=$BUILD_TYPE`
  4. `cmake --build build --parallel`
  5. `ctest --test-dir build --output-on-failure`
- `fail-fast: false`
- Cache `~/.cache/ccache` (Linux/macOS) avec `actions/cache@v4`

### Critères d'acceptation
- [ ] Tous les jobs verts sur la PR
- [ ] Badge CI dans `README.md`
- [ ] Temps total < 6 min sur cache chaud

---

## US-002 — Couverture de code (gcov + lcov + Codecov)

**Priorité :** P0 — **Dépend de :** US-001 — **Bloque :** US-041

### Spécification technique
- Nouveau job CI `coverage` (Ubuntu + GCC seul, Debug)
- Flags : `-O0 -g --coverage -fprofile-arcs -ftest-coverage`
- Option CMake : `-DENABLE_COVERAGE=ON` pilotant ces flags
- Étapes :
  1. Build + run tests
  2. `lcov --capture --directory build --output-file coverage.info`
  3. `lcov --remove coverage.info '*/test/*' '*/_deps/*' '/usr/*' --output-file coverage.info`
  4. `codecov-action@v4` avec `coverage.info`
- Badge Codecov dans README

### Critères d'acceptation
- [ ] Rapport visible sur codecov.io
- [ ] Badge dans README
- [ ] Couverture initiale rapportée (≥ existante, pas de seuil bloquant ici — US-041 ajoutera le seuil)

---

## US-003 — Sanitizers (ASan + UBSan)

**Priorité :** P1 — **Dépend de :** US-001

### Spécification
- Job CI `sanitizers` (Ubuntu + Clang 17, Debug)
- Flags : `-fsanitize=address,undefined -fno-omit-frame-pointer`
- Option CMake : `-DENABLE_SANITIZERS=ON`
- Variables d'env : `ASAN_OPTIONS=detect_leaks=1`, `UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1`
- Pas de TSan (pas de threads), pas de MSan (complexe à provisionner)

### Critères d'acceptation
- [ ] Job vert sur develop
- [ ] Tout test échouant en sanitizer = build rouge

---

## US-004 — clang-format + vérification CI

**Priorité :** P1 — **Dépend de :** US-001

### Spécification
- Fichier `.clang-format` à la racine :
  ```yaml
  BasedOnStyle: LLVM
  IndentWidth: 4
  ColumnLimit: 100
  AccessModifierOffset: -4
  PointerAlignment: Left
  AllowShortFunctionsOnASingleLine: InlineOnly
  ```
- Job CI `format-check` : `clang-format --dry-run --Werror $(find src test -name '*.hpp' -o -name '*.cpp')`
- Cible CMake `format` qui applique `clang-format -i`
- Pre-commit hook documenté dans `CONTRIBUTING.md` (création optionnelle)

### Critères d'acceptation
- [ ] Tous les fichiers reformatés conformément
- [ ] Job CI bloque tout PR non formaté

---

## US-005 — clang-tidy + vérification CI

**Priorité :** P1 — **Dépend de :** US-001

### Spécification
- Fichier `.clang-tidy` à la racine :
  ```yaml
  Checks: >
    bugprone-*,
    cppcoreguidelines-*,
    modernize-*,
    performance-*,
    portability-*,
    readability-*,
    -modernize-use-trailing-return-type,
    -readability-magic-numbers,
    -cppcoreguidelines-avoid-magic-numbers
  WarningsAsErrors: '*'
  HeaderFilterRegex: 'src/include/.*\.hpp$'
  ```
- Job CI `lint` : `run-clang-tidy -p build` après cmake configure
- Génération `compile_commands.json` via `CMAKE_EXPORT_COMPILE_COMMANDS=ON`

### Critères d'acceptation
- [ ] Aucun warning clang-tidy sur le code livré
- [ ] CI rouge si nouveau warning introduit

---

## US-006 — Doc Doxygen publiée sur GitHub Pages

**Priorité :** P1 — **Dépend de :** US-001

### Spécification
- Workflow `.github/workflows/docs.yml` déclenché sur `push` vers `develop`
- Étapes : checkout, install Doxygen, `cmake --build build --target doc`, déploiement sur `gh-pages` via `peaceiris/actions-gh-pages@v3`
- Le `doc/publish.sh` existant peut être supprimé ou rendu local-only
- URL publique : `https://yscialom.github.io/matrix/`
- Badge "docs" dans README

### Critères d'acceptation
- [ ] Doc accessible sur l'URL ci-dessus
- [ ] Mise à jour automatique à chaque merge sur `develop`

---

## US-007 — Release automation (semver + GitHub Releases)

**Priorité :** P2 — **Dépend de :** US-001

### Spécification
- Workflow `.github/workflows/release.yml` déclenché sur tag `v*.*.*`
- Étapes : build, test, génération CHANGELOG via `git-cliff` (config `cliff.toml`), création release GitHub avec `softprops/action-gh-release@v1`
- Convention de commits **Conventional Commits** (feat:, fix:, chore:, etc.) — documentée dans `CONTRIBUTING.md`
- Bump des `VERSION_MAJOR/MINOR/PATCH` dans `CMakeLists.txt` à chaque release (manuel via PR, CI vérifie cohérence avec le tag)

### Critères d'acceptation
- [ ] Release v2.0.0 créée à la fin de la roadmap (cf. US-042)
- [ ] CHANGELOG généré
