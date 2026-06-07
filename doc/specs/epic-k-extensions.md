# EPIC K — Extensions pre-v1

| US | Titre | Priorité | Statut |
|----|-------|----------|--------|
| US-060 | Réductions par axe (`sum<Axis>()`, etc.) | P2 | ✅ Done |
| US-061 | `submatrix` : extraction d'un sous-bloc N-D | P2 | ✅ Done |
| US-062 | `enumerate()` : itérateur de coordonnées | P2 | ✅ Done |
| US-063 | Opérateurs bit-à-bit pour types entiers | P2 | ✅ Done |
| US-064 | Test ASan : détection de vue dangling | P2 | ✅ Done |
| US-065 | Tests de référence linalg (valeurs pré-calculées) | P2 | ✅ Done |
| US-066 | CI Windows : cache vcpkg | P2 | ✅ Done |
| US-067 | Hygiène repo : `.editorconfig`, `SECURITY.md`, `CODE_OF_CONDUCT.md`, Dependabot | P2 | ✅ Done |
| US-068 | Migration guide : promesse de stabilité SemVer v1.0.0 | P2 | ✅ Done |
| US-069 | `generate` avec callable multi-index | P2 | ✅ Done |

---

## US-060 — Réductions par axe (`sum<Axis>()`, etc.)

**Priorité :** P2 — **Dépend de :** US-031 — **Épopée :** K

### Story
En tant qu'utilisateur, je veux calculer la somme, le min, le max d'une matrice selon un axe donné, en obtenant une matrice de dimension inférieure.

### Spécification technique
- `template<std::size_t Axis> constexpr auto sum() const -> matrix<T, /* dims sans Axis */>`
- Idem pour `min<Axis>()` et `max<Axis>()`
- Implémentation : métaprogrammation pour déduire les dimensions résultantes (supprimer la dimension `Axis` du pack `Dims...`)
- Contrainte : `static_assert(Axis < order)`

### Critères d'acceptation
- [x] `matrix<int,2,3>{{1,2,3},{4,5,6}}.sum<0>()` == `matrix<int,3>{5,7,9}` (somme par colonnes)
- [x] `matrix<int,2,3>{{1,2,3},{4,5,6}}.sum<1>()` == `matrix<int,2>{6,15}` (somme par lignes)
- [x] Erreur de compilation si `Axis >= order` (contrainte `requires(Axis < order)`)
- [x] Tests dans `test/src/reductions_axis.cpp`

---

## US-061 — `submatrix` : extraction d'un sous-bloc N-D

**Priorité :** P2 — **Dépend de :** US-036 — **Épopée :** K

### Story
En tant qu'utilisateur travaillant sur du traitement d'image ou des noyaux de convolution, je veux extraire un sous-bloc d'une matrice N-D.

### Spécification technique
```cpp
template<std::size_t... NewD>
matrix_view<T, strided, NewD...>
submatrix(std::array<std::size_t, order> origin) const;
```
- Contrainte compile-time : `sizeof...(NewD) == order`
- Vérification runtime : `origin[i] + NewD[i] <= Dims[i]` pour chaque `i`, sinon `std::out_of_range`
- Retourne une vue strided (strides calculés sur la matrice source)

### Critères d'acceptation
- [ ] `m.submatrix<2,2>({1,1})` sur une `matrix<int,4,4>` retourne une vue 2×2 correcte
- [ ] Mutation via la vue se reflète dans la matrice source
- [ ] `m.submatrix<3,3>({2,2})` sur une `matrix<int,4,4>` lève `std::out_of_range`
- [ ] Tests dans `test/src/submatrix.cpp`

---

## US-062 — `enumerate()` : itérateur de coordonnées

**Priorité :** P2 — **Dépend de :** US-016 — **Épopée :** K

### Story
En tant qu'utilisateur, je veux itérer sur les éléments d'une matrice avec leurs coordonnées multi-dimensionnelles (façon `np.ndenumerate`).

### Spécification technique
- `m.enumerate()` : retourne un range de `std::pair<std::array<std::size_t, order>, T&>`
- Parcours row-major (cohérent avec l'itérateur linéaire)
- Implémentation : adaptateur sur l'itérateur linéaire, conversion `linear_index → coordinates` via `detail::index_to_coordinates`

### Critères d'acceptation
- [ ] `for (auto& [coords, val] : m.enumerate())` compile
- [ ] `coords` est un `std::array<std::size_t, order>` correct pour chaque élément
- [ ] Mutation via `val` se reflète dans `m`
- [ ] Tests dans `test/src/enumerate.cpp`

---

## US-063 — Opérateurs bit-à-bit pour types entiers

**Priorité :** P2 — **Dépend de :** US-026 — **Épopée :** K

### Story
En tant qu'utilisateur travaillant sur des masques binaires (image, cryptographie), je veux appliquer des opérateurs bit-à-bit sur des `matrix<unsigned, ...>`.

### Spécification technique
- Opérateurs membres : `operator&=`, `operator|=`, `operator^=`
- Opérateurs scalaires : `operator<<=`, `operator>>=` (shift par un entier)
- Opérateur unaire : `operator~` (NOT bit-à-bit)
- Opérateurs binaires friends : `operator&`, `operator|`, `operator^`
- Contrainte : `requires std::integral<T>` sur chaque opérateur
- Implémentation : `std::transform` comme pour les opérateurs arithmétiques

### Critères d'acceptation
- [ ] `matrix<unsigned,3>{1,2,3} & matrix<unsigned,3>{3,3,3}` compile et retourne `{1,2,3}`
- [ ] `~matrix<unsigned,3>{0,0,0}` retourne matrice de `~0u`
- [ ] Erreur de compilation pour `matrix<double,3>` (non-integral)
- [ ] Tests dans `test/src/arithmetic_bitwise.cpp`

---

## US-064 — Test ASan : détection de vue dangling

**Priorité :** P2 — **Dépend de :** US-035, US-003 — **Épopée :** K

### Story
En tant que mainteneur, je veux qu'un test sous ASan détecte une utilisation de `matrix_view` après destruction de la matrice source, pour valider le comportement attendu (UB → crash sous sanitizer).

### Spécification technique
- Fichier `test/src/matrix_view_lifetime.cpp`, compilé uniquement si `YSC_SANITIZERS_ENABLED`
- Scénario : construire une `matrix_view` sur une matrice locale dans une sous-scope, laisser la matrice être détruite, accéder à la vue → use-after-free détecté par ASan
- Le test est **intentionnellement un crash** sous ASan (use-after-free). Il doit être exclu du run normal et inclus dans un test dédié sous sanitizers
- Utiliser `GTEST_SKIP()` si `!defined(YSC_SANITIZERS_ENABLED)` pour ne pas bloquer le build normal

### Critères d'acceptation
- [ ] Le fichier compile avec `ENABLE_SANITIZERS=ON`
- [ ] Le test est skippé sans `ENABLE_SANITIZERS`
- [ ] Le comportement UB est documenté dans la docstring Doxygen de `matrix_view`

---

## US-065 — Tests de référence linalg (valeurs pré-calculées)

**Priorité :** P2 — **Dépend de :** US-033, US-034 — **Épopée :** K

### Story
En tant que mainteneur, je veux quelques tests de sanité qui comparent les résultats numériques de `matmul` et `dot` à des valeurs de référence.

### Spécification technique
- Nouveau fichier `test/src/linalg_reference.cpp`
- Valeurs de référence hard-codées (calculées avec Numpy ou à la main, pas de dépendance runtime)
- Matrices de test :
  - `matmul` : 2×2, 2×3×3×2, matrice identité × matrice quelconque
  - `dot` : 3 paires de vecteurs 1D
  - `transpose` : 2×3 → vérification symétrie
- Pas de dépendance externe (Numpy, Eigen) dans les tests

### Critères d'acceptation
- [ ] `matmul(A, B) == expected_AB` pour ≥ 3 paires de matrices
- [ ] `dot(a, b) == expected` pour ≥ 3 paires de vecteurs
- [ ] `transpose(transpose(m)) == m` pour ≥ 2 matrices
- [ ] Tous les résultats vérifiés via `static_assert` quand `constexpr`

---

## US-066 — CI Windows : cache vcpkg

**Priorité :** P2 — **Dépend de :** US-001 — **Épopée :** K

### Story
En tant que mainteneur, je veux que la CI Windows ne retélécharge pas `vcpkg`/`gtest` à chaque run pour accélérer les builds.

### Spécification technique
- Ajouter `actions/cache@v4` sur `vcpkg_installed/` (ou le dossier d'install vcpkg) dans le job Windows
- Key : hash du fichier de manifest vcpkg ou de la commande `vcpkg install gtest:x64-windows`
- Restore-keys : fallback sur un cache partiel

### Critères d'acceptation
- [ ] Second run CI Windows avec code identique : step `vcpkg install` skippé (cache hit affiché)
- [ ] Temps CI Windows réduit d'au moins 30 secondes sur cache chaud

---

## US-067 — Hygiène repo : `.editorconfig`, `SECURITY.md`, `CODE_OF_CONDUCT.md`, Dependabot

**Priorité :** P2 — **Dépend de :** rien — **Épopée :** K

### Story
En tant que contributeur externe, je veux que le projet respecte les conventions OSS standards et facilite la contribution.

### Spécification technique
- `.editorconfig` à la racine :
  - `indent_style = space`, `indent_size = 4` pour C++ et CMake
  - `indent_size = 2` pour YAML, JSON, Markdown
  - `end_of_line = lf`, `charset = utf-8`, `trim_trailing_whitespace = true`, `insert_final_newline = true`
  - Cohérent avec `.clang-format`
- `SECURITY.md` : instructions pour reporter des vulnérabilités via GitHub Issue (surface réduite : lib sans I/O, header-only)
- `CODE_OF_CONDUCT.md` : Contributor Covenant v2.1 (texte standard, adapté au nom du projet)
- `.github/dependabot.yml` :
  ```yaml
  version: 2
  updates:
    - package-ecosystem: "github-actions"
      directory: "/"
      schedule:
        interval: "monthly"
  ```

### Critères d'acceptation
- [ ] `.editorconfig` présent et cohérent avec `.clang-format` (même indent_size)
- [ ] `SECURITY.md` présent avec instructions claires
- [ ] `CODE_OF_CONDUCT.md` présent (Contributor Covenant v2.1)
- [ ] `.github/dependabot.yml` présent avec config `github-actions`

---

## US-068 — Migration guide : promesse de stabilité SemVer v1.0.0

**Priorité :** P2 — **Dépend de :** US-042 — **Épopée :** K

### Story
En tant qu'utilisateur adoptant la lib depuis une version v0.x, je veux comprendre les garanties de stabilité et les changements breaking introduits en v1.0.0.

### Spécification technique
- Nouveau fichier `doc/migration.md`
- Contenu :
  - **Promesse SemVer depuis v1.0.0** : API publique = tout hors `ysc::detail::`. `ysc::detail::` peut changer en patch.
  - **Changements breaking depuis v0.x :**
    - Renommage cible CMake `matrix` → `ysc-matrix`, alias `ysc::matrix` (US-045)
    - Changement de valeur de hash (US-059) : les `std::unordered_set<matrix<...>>` sérialisés avant v1.0.0 sont invalidés
  - **Instructions de migration** pour chaque changement (avant/après)
- Lien depuis `README.md` section Installation et depuis `mainpage.md`

### Critères d'acceptation
- [ ] `doc/migration.md` présent et lisible depuis le repo GitHub
- [ ] Tous les changements breaking v0.x → v1.0.0 listés avec instructions
- [ ] La promesse SemVer (API publique vs `ysc::detail::`) est explicitement documentée
- [ ] Lien vers `doc/migration.md` depuis `README.md` et `mainpage.md`

---

## US-069 — `generate` avec callable multi-index

**Priorité :** P2 — **Dépend de :** US-053 — **Épopée :** K

### Story
En tant qu'utilisateur construisant des matrices position-dépendantes (identité par fonction, triangulaire, Vandermonde, gradient XY, masques i*j…), je veux pouvoir passer à `ysc::generate` un callable qui reçoit les coordonnées N-D plutôt qu'un index linéaire. Aujourd'hui je dois dérouler à la main `i = k / cols; j = k % cols;`, ce qui est verbeux et fragile en 3D+.

### Spécification technique

Ajouter une **deuxième surcharge** de `ysc::generate` à côté de l'existante (`src/include/matrix.hpp:1632-1640`) :

```cpp
template <class T, std::size_t... Dims, class F>
    requires std::invocable<F, /* sizeof...(Dims) × std::size_t */>
          && std::convertible_to<
                 std::invoke_result_t<F, /* idem */>, T>
constexpr matrix<T, Dims...> generate(F f);
```

- L'overload existant (`std::invocable<F, std::size_t>`, index linéaire row-major) reste exposé et continue de compiler tel quel — **rétrocompatibilité totale**.
- Implémentation : itérer sur les indices N-D via `detail::index_to_coordinates` (déjà disponible dans `matrix_detail.hpp`, US-044) et invoquer `f` avec `std::apply` sur le tuple de coordonnées.
- Doxygen `@brief @tparam @return @code`, groupe `ysc_factory` (ou équivalent existant).
- Le callable peut être `auto` (la C++ générique) ou typé explicitement `std::size_t`.

**Cas limite à clarifier dans l'implémentation** : si `sizeof...(Dims) == 1`, un callable `[](std::size_t k){ ... }` satisfait à la fois `invocable<size_t>` et la nouvelle contrainte (1 arg). Trancher en faveur de la surcharge linéaire (rétrocompat) — par exemple via `requires(!std::invocable<F, std::size_t>)` sur la nouvelle surcharge, ou par concept plus strict.

### Critères d'acceptation
- [ ] `auto I = ysc::generate<int, 3, 3>([](std::size_t i, std::size_t j){ return i == j ? 1 : 0; });` compile et produit la matrice identité 3×3.
- [ ] `static_assert(ysc::generate<int, 2, 2>([](auto i, auto j){ return int(i + j); })(1, 1) == 2);` passe (la surcharge est `constexpr`).
- [ ] Tenseur 3D : `auto m = ysc::generate<int, 2, 3, 4>([](auto i, auto j, auto k){ return int(i*100 + j*10 + k); });` compile et `m(1, 2, 3) == 123`.
- [ ] La surcharge linéaire historique (`generate<int, N>([](std::size_t k){ return int(k); })`) compile sans modification.
- [ ] Test dédié `test/src/generate_multi_index.cpp` couvrant : 1D (équivalent linéaire vs multi-index), 2D, 3D, type non-trivial, propagation `constexpr`.
- [ ] Doxygen mis à jour avec exemple inline `@code`.
- [ ] CI verte sur toutes plateformes ; clang-format / clang-tidy clean.
