# Axe 1 — Public API freeze audit

**Date.** 2026-06-07
**Branche.** `chore/v1-check-01-api-freeze`
**Référence.** `doc/before-v1-checks.md` §1

## Objectif

Geler intentionnellement la surface publique exposée à `v1.0.0` : naming, signatures, attributs (`[[nodiscard]]`, `noexcept`, `constexpr`, `explicit`), et surface visible. Après `v1.0.0`, tout symbole exposé dans `ysc::` (hors `ysc::detail::`) devient une promesse SemVer (cf. `doc/migration.md`).

## Périmètre

- **IN** : `src/include/matrix.hpp`, `src/include/matrix_view.hpp`, `src/include/matrix_detail.hpp` ; toute déclaration au niveau `namespace ysc` ; spécialisations `std::hash` et `std::formatter`.
- **OUT** : tests (`test/src/`), exemples (`examples/`), internals `ysc::detail::`.

## Méthode

1. Lister exhaustivement tous les symboles publics par fichier.
2. Pour chaque symbole : vérifier `[[nodiscard]]`, `constexpr`, `noexcept`, `explicit`, naming.
3. Vérifier qu'aucun symbole exposé n'aurait dû rester `detail::`.
4. Vérifier que `ysc::detail::` ne fuit pas dans la doc Doxygen (couvert par l'axe 3 ; non audité ici).

---

## 1. Inventaire — `src/include/matrix_detail.hpp`

### Tags publics

| Symbole | Type | Référence |
|---|---|---|
| `struct contiguous {}` | tag-type | matrix_detail.hpp:26 |
| `struct strided {}` | tag-type | matrix_detail.hpp:28 |
| `struct all_t {}` | tag-type | matrix_detail.hpp:36 |
| `inline constexpr all_t all` | variable | matrix_detail.hpp:48 |

### Concept public

| Symbole | Signature | Référence |
|---|---|---|
| `integral_coordinates<Coords...>` | `(std::integral<std::remove_cvref_t<Coords>> && ...)` | matrix_detail.hpp:64 |

### Alias public

| Symbole | Signature | Référence |
|---|---|---|
| `const_matrix_view<T, Dims...>` | `template using = matrix_view<const T, contiguous, Dims...>` | matrix_detail.hpp:303 |

### Forward declarations publiques

| Symbole | Référence |
|---|---|
| `template <class T, std::size_t... Dims> class matrix;` | matrix_detail.hpp:52 |
| `template <class T, class Storage, std::size_t... Dims> class matrix_view;` | matrix_detail.hpp:54 |

---

## 2. Inventaire — `src/include/matrix_view.hpp`

### `matrix_view<T, contiguous, Dimensions...>`

Statiques et typedefs :

| Symbole | Détails | Référence |
|---|---|---|
| `static constexpr std::size_t order` | = `sizeof...(Dimensions)` | matrix_view.hpp:91 |
| `static constexpr std::array dimensions` | = `{Dimensions...}` | matrix_view.hpp:93 |
| `value_type`, `size_type`, `difference_type`, `reference`, `const_reference`, `pointer`, `const_pointer`, `iterator`, `const_iterator`, `reverse_iterator`, `const_reverse_iterator` | aliases STL-compatibles | matrix_view.hpp:107–127 |

Constructeurs / destructeur :

| Signature | `[[nodiscard]]` | `noexcept` | `constexpr` | `explicit` | Référence |
|---|---|---|---|---|---|
| `matrix_view() = delete` | — | — | — | — | matrix_view.hpp:131 |
| `matrix_view(matrix<T, Dims...>& m)` (implicit, NOLINTNEXTLINE) | — | ✓ | ✓ | — | matrix_view.hpp:148 |
| `template<U> matrix_view(const matrix<U, Dims...>& m) requires std::same_as<T, const U>` (implicit) | — | ✓ | ✓ | — | matrix_view.hpp:169 |
| `explicit matrix_view(T* ptr)` | — | ✓ | ✓ | ✓ | matrix_view.hpp:185 |
| `matrix_view(const matrix_view&) noexcept = default` | — | ✓ | ✓ | — | matrix_view.hpp:188 |
| `matrix_view& operator=(const matrix_view&) noexcept = default` | — | ✓ | ✓ | — | matrix_view.hpp:190 |
| `matrix_view(matrix_view&&) noexcept = default` | — | ✓ | ✓ | — | matrix_view.hpp:192 |
| `matrix_view& operator=(matrix_view&&) noexcept = default` | — | ✓ | ✓ | — | matrix_view.hpp:194 |
| `~matrix_view() = default` | — | — | — | — | matrix_view.hpp:196 |
| `constexpr operator matrix_view<T, strided, Dims...>() const noexcept` (implicit, NOLINTNEXTLINE) | — | ✓ | **✓ (corrigé F1)** | — | matrix_view.hpp:214 / matrix_view.hpp:1306 |

Itérateurs (12 méthodes) :

| Signature (toutes `constexpr noexcept`) | `[[nodiscard]]` | Référence |
|---|---|---|
| `iterator begin() noexcept` | — | matrix_view.hpp:223 |
| `const_iterator begin() const noexcept` | ✓ | matrix_view.hpp:230 |
| `const_iterator cbegin() const noexcept` | ✓ | matrix_view.hpp:237 |
| `iterator end() noexcept` | — | matrix_view.hpp:244 |
| `const_iterator end() const noexcept` | ✓ | matrix_view.hpp:251 |
| `const_iterator cend() const noexcept` | ✓ | matrix_view.hpp:258 |
| `reverse_iterator rbegin() noexcept` | — | matrix_view.hpp:265 |
| `const_reverse_iterator rbegin() const noexcept` | ✓ | matrix_view.hpp:272 |
| `const_reverse_iterator crbegin() const noexcept` | ✓ | matrix_view.hpp:281 |
| `reverse_iterator rend() noexcept` | — | matrix_view.hpp:290 |
| `const_reverse_iterator rend() const noexcept` | ✓ | matrix_view.hpp:297 |
| `const_reverse_iterator crend() const noexcept` | ✓ | matrix_view.hpp:306 |

Capacity, data, élément :

| Signature | `[[nodiscard]]` | `noexcept` | `constexpr` | Référence |
|---|---|---|---|---|
| `static constexpr size_type size() noexcept` | ✓ | ✓ | ✓ | matrix_view.hpp:324 |
| `static constexpr size_type max_size() noexcept` | ✓ | ✓ | ✓ | matrix_view.hpp:334 |
| `static constexpr bool empty() noexcept` | ✓ | ✓ | ✓ | matrix_view.hpp:344 |
| `pointer data() noexcept` | ✓ | ✓ | ✓ | matrix_view.hpp:354 |
| `const_pointer data() const noexcept` | ✓ | ✓ | ✓ | matrix_view.hpp:364 |
| `reference front() noexcept` | — | ✓ | ✓ | matrix_view.hpp:375 |
| `const_reference front() const noexcept` | ✓ | ✓ | ✓ | matrix_view.hpp:384 |
| `reference back() noexcept` | — | ✓ | ✓ | matrix_view.hpp:394 |
| `const_reference back() const noexcept` | ✓ | ✓ | ✓ | matrix_view.hpp:404 |
| `const T& operator()(Coords...) const` (`integral_coordinates`) | — | ✓ | ✓ | matrix_view.hpp:424 |
| `T& operator()(Coords...)` (`integral_coordinates`) | — | ✓ | ✓ | matrix_view.hpp:449 |
| `const T& at(Coords...) const` (`integral_coordinates`) | ✓ | — (throws) | — | matrix_view.hpp:477 |
| `T& at(Coords...)` (`integral_coordinates`) | ✓ | — (throws) | — | matrix_view.hpp:517 |

Modifiers / vues :

| Signature | `[[nodiscard]]` | `noexcept` | `constexpr` | Référence |
|---|---|---|---|---|
| `void fill(const T& value) noexcept(...)` | — | (conditional) | ✓ | matrix_view.hpp:551 |
| `auto slice(Specs...) const` | ✓ | — (may throw) | ✓ | matrix_view.hpp:577 |
| `auto row(std::size_t i) const` (2D only) | ✓ | — (may throw) | ✓ | matrix_view.hpp:627 |
| `auto col(std::size_t j) const` (2D only) | ✓ | — (may throw) | ✓ | matrix_view.hpp:652 |

### `matrix_view<T, strided, Dimensions...>`

Statiques, typedefs : identiques en intention (`order`, `dimensions`, `value_type`, etc., matrix_view.hpp:698–723).

Itérateur imbriqué publique `struct iterator` (matrix_view.hpp:735–823) avec tous les opérateurs random-access (`++`, `--`, `+=`, `-=`, `+`, `-`, `[]`, `==`, `<=>`), tous `noexcept`. Idem `struct const_iterator` (matrix_view.hpp:829–923).

Constructeurs / accesseurs / fill (alignés sur l'analogue `contiguous`, sauf que seuls `iterator`/`const_iterator` 1-D sont fournis sur l'order-1 — itération sur un strided multi-D passe par les coordonnées). `noexcept(...)` et `constexpr` cohérents avec la version `contiguous`. Voir matrix_view.hpp:927–1292 pour le détail.

### Fonctions libres / spécialisations standard

| Symbole | Signature | Référence |
|---|---|---|
| `std::ostream& operator<<(std::ostream&, const matrix_view<T, contiguous, Dims...>&)` | I/O texte ; non-`constexpr` ; non-`noexcept` | matrix_view.hpp:1342 |
| `std::formatter<ysc::matrix_view<T, contiguous, Dims...>>` | spécialisation `std::formatter` (guard C++20 `<format>`) | matrix_view.hpp:1374 |

---

## 3. Inventaire — `src/include/matrix.hpp`

### Concept et tags publics

| Symbole | Référence |
|---|---|
| `template <class T, class U> concept matrix_convertible_from = std::convertible_to<U, T>` | matrix.hpp:46 |
| `constexpr struct matrix_zero_t {} zero` | matrix.hpp:52–53 |

### Classe `matrix<T, Dimensions...>`

Statiques :

| Symbole | Référence |
|---|---|
| `static constexpr std::size_t order` | matrix.hpp:136 |
| `static constexpr std::array dimensions` | matrix.hpp:148 |

Typedefs : `value_type`, `size_type`, `difference_type`, `reference`, `const_reference`, `pointer`, `const_pointer`, `iterator`, `const_iterator`, `reverse_iterator`, `const_reverse_iterator` (matrix.hpp:156–176).

Itérateurs (12 méthodes, toutes `[[nodiscard]] constexpr noexcept` quand applicable) : matrix.hpp:191–283. Aligné sur `matrix_view<contiguous>`.

Capacity / accès direct : `size()`, `max_size()`, `empty()`, `data()`, `data() const`, `front()`, `front() const`, `back()`, `back() const` — tous `constexpr noexcept` ; lectures `[[nodiscard]]`. Voir matrix.hpp:298–360 et 1625–1663.

Constructeurs / assignations / destructeur :

| Signature | `noexcept` | `constexpr` | `explicit` | Référence |
|---|---|---|---|---|
| `matrix() = default` | (implicit) | — | — | matrix.hpp:400 |
| `constexpr matrix(matrix_zero_t)` | — | ✓ | — | matrix.hpp:414 |
| `template<Args...> matrix(Args&&...)` (aggregate, requires sizeof...==linear_size & convertible & >0) | — | ✓ | `explicit(sizeof...(Args)==1)` | matrix.hpp:437 |
| `matrix(std::initializer_list<std::initializer_list<T>> init)` (2D only) | — | — | — | matrix.hpp:460 |
| `matrix(matrix const&) = default` | — | — | — | matrix.hpp:487 |
| `template<U> matrix(const matrix<U, Dims...>&)` | — | — | — | matrix.hpp:506 |
| `matrix(matrix&&) = default` | — | — | — | matrix.hpp:525 |
| `template<U> matrix(matrix<U, Dims...>&&)` | — | — | — | matrix.hpp:548 |
| `explicit matrix(const matrix_view<T, contiguous, Dims...>&)` | — | — | ✓ | matrix.hpp:569 |
| `explicit matrix(const matrix_view<T, strided, Dims...>&)` | — | — | ✓ | matrix.hpp:592 |
| `explicit matrix(std::array<T, linear_size> data)` | (spec'd) | — | ✓ | matrix.hpp:616 |
| `explicit matrix(std::span<const T, linear_size> data)` | — | — | ✓ | matrix.hpp:632 |
| `matrix& operator=(matrix const&) = default` / move = default | — | — | — | matrix.hpp:648, 682 |
| `template<U> matrix& operator=(const matrix<U,...>&)` / move | — | — | — | matrix.hpp:665, 702 |
| `~matrix() = default` | — | — | — | matrix.hpp:708 |
| `friend void swap(matrix& lhs, matrix& rhs) noexcept(...)` | (conditional) | — | — | matrix.hpp:378 |

Comparaisons : `friend operator==`, `friend operator<=>` defaulted (matrix.hpp:726, 741).

Arithmétique :

- Élément-par-élément compound : `+=`, `-=`, `*=`, `/=` avec `requires { a op= b; }` (matrix.hpp:761, 782, 844, 865).
- Élément-par-élément binaire (friends, `[[nodiscard]]`) : `+`, `-`, `*`, `/` (matrix.hpp:803, 822, 889, 908).
- Scalaire compound : `*=`, `/=`, `+=`, `-=` (templates `Scalar` avec `requires`) (matrix.hpp:931, 955, 979, 1003).
- Scalaire binaire (friends, `[[nodiscard]]`) : `*`, `*` (commutative), `/`, `+`, `-` (matrix.hpp:1027, 1050, 1072, 1094, 1117).
- Unaires : `matrix operator+() const` ; `constexpr matrix operator-() const noexcept(...)` (matrix.hpp:1135, 1148).

Bit-à-bit :

- Compound : `operator&=/|=/^=` (templates `Other`, accès via `cbegin()` car types différents) ; `operator<<=/>>=` (scalaires) (matrix.hpp:1171, 1194, 1217, 1239, 1261).
- Unaire : `matrix operator~() const` requires(...) (matrix.hpp:1280).

Algorithmes :

| Signature | `[[nodiscard]]` | `constexpr` | Référence |
|---|---|---|---|
| `template<F> void apply(F f) requires(...)` | — | ✓ | matrix.hpp:1311 |
| `template<F> auto map(F f) const requires(...)` | ✓ | ✓ | matrix.hpp:1337 |
| `U sum() const requires(...)` | ✓ | ✓ | matrix.hpp:1359 |
| `U min() const requires(...)` | ✓ | ✓ | matrix.hpp:1380 |
| `U max() const requires(...)` | ✓ | ✓ | matrix.hpp:1400 |
| `bool all() const requires(...)` | ✓ | ✓ | matrix.hpp:1419 |
| `bool any() const requires(...)` | ✓ | ✓ | matrix.hpp:1438 |
| `template<size_t Axis> auto sum() const requires(Axis<order && dim[Axis]>0)` | ✓ | ✓ | matrix.hpp:1460 |
| `template<size_t Axis> auto min() const requires(...)` | ✓ | ✓ | matrix.hpp:1504 |
| `template<size_t Axis> auto max() const requires(...)` | ✓ | ✓ | matrix.hpp:1549 |

Modifiers : `void fill(const T&) noexcept(...)` (matrix.hpp:1592), `void swap(matrix&) noexcept(...)` (matrix.hpp:1608).

Accès indexé : `operator()(Coords...) const` / `operator()(Coords...)` (constexpr, no bounds check) ; `at(Coords...) const` / `at(Coords...)` (throws). matrix.hpp:1682–1775.

Vues / restructuration :

| Signature (toutes `[[nodiscard]] constexpr`) | Référence |
|---|---|
| `auto slice(Specs...) &` / `const&` | matrix.hpp:1805, 1860 |
| `auto row(size_t i) &` / `const&` (2D) | matrix.hpp:1912, 1938 |
| `auto col(size_t j) &` / `const&` (2D) | matrix.hpp:1965, 1991 |
| `auto rows() &` / `const&` (2D) | matrix.hpp:2016, 2039 |
| `auto cols() &` / `const&` (2D) | matrix.hpp:2062, 2085 |
| `matrix_view<T, contiguous, NewD...> reshape() &` (noexcept) | matrix.hpp:2108 |
| `matrix_view<const T, contiguous, NewD...> reshape() const&` (noexcept) | matrix.hpp:2127 |
| `matrix_view<T, contiguous, linear_size> flatten() &` (noexcept) | matrix.hpp:2147 |
| `matrix_view<const T, contiguous, linear_size> flatten() const&` (noexcept) | matrix.hpp:2162 |
| `enumerate_range<T> enumerate() noexcept` | matrix.hpp:2256 |
| `enumerate_range<const T> enumerate() const noexcept` | matrix.hpp:2277 |
| `matrix_view<T, strided, NewD...> submatrix(std::array<size_t, order>) &` | matrix.hpp:2306 |
| `matrix_view<const T, strided, NewD...> submatrix(std::array<size_t, order>) const&` | matrix.hpp:2349 |

Classe imbriquée publique `enumerate_range<ElemT>` (matrix.hpp:2179) : `value_type`, classe `iterator` imbriquée (input-iterator), `begin()`/`end()`. Constructeur **privé** depuis le fix F2 (cf. §5).

### Fonctions libres (`namespace ysc`)

| Symbole | Référence |
|---|---|
| `constexpr matrix<T, D...> zeros() noexcept` | matrix.hpp:2386 |
| `constexpr matrix<T, D...> full(const T& v)` | matrix.hpp:2403 |
| `constexpr matrix<T, D...> ones()` | matrix.hpp:2423 |
| `constexpr matrix<T, N, N> identity()` | matrix.hpp:2441 |
| `constexpr matrix<T, Dims...> generate(F f)` (linear-index overload) | matrix.hpp:2475 |
| `constexpr matrix<T, Dims...> generate(F f) requires detail::coord_generator<...>` (multi-index overload) | matrix.hpp:2520 |
| `constexpr matrix<T, C, R> transpose(const matrix<T, R, C>&)` | matrix.hpp:2555 |
| `constexpr auto matmul(const matrix<Ta, M, N>&, const matrix<Tb, N, P>&)` | matrix.hpp:2589 |
| `constexpr auto matmul(const matrix<Ta, M, N>&, const matrix<Tb, N>&)` (matrix×vector) | matrix.hpp:2659 |
| `constexpr auto dot(const matrix<Ta, N>&, const matrix<Tb, N>&)` | matrix.hpp:2626 |
| `constexpr auto operator&(const matrix<Ta, D...>&, const matrix<Tb, D...>&)` | matrix.hpp:2691 |
| `constexpr auto operator|(const matrix<Ta, D...>&, const matrix<Tb, D...>&)` | matrix.hpp:2721 |
| `constexpr auto operator^(const matrix<Ta, D...>&, const matrix<Tb, D...>&)` | matrix.hpp:2751 |
| `std::ostream& operator<<(std::ostream&, const matrix<T, D...>&)` | matrix.hpp:2790 |

### Spécialisations standard

| Symbole | Référence |
|---|---|
| `std::hash<ysc::matrix<T, D...>>` (`operator() noexcept`) | matrix.hpp:2819 |
| `std::formatter<ysc::matrix<T, D...>, CharT>` (guard `<format>` + Clang ≥ 17) | matrix.hpp:2855 |

---

## 4. Inventaire — `ysc::detail::` (non exporté)

Référence pour traçabilité, garantissant qu'aucun de ces symboles ne fuit sous `ysc::` :

- `ostream_streamable` (concept), `print_recursive` (fonction) — matrix_detail.hpp:71–95.
- `coordinates_to_index`, `index_to_coordinates` — matrix_detail.hpp:98–129.
- Helpers slicing : `is_all_v`, `n_kept_v`, `size_seq`, `prepend_val`, `pad_right_with_all`, `pad_right_with_all_t`, `is_prefix_slice_helper`, `is_prefix_slice_v`, `filter_kept_dims`, `make_matrix_view`, `make_matrix_view_t`, `slice_helper` — matrix_detail.hpp:130–230.
- Helpers réductions axiales : `drop_dim_impl`, `drop_dim_t`, `make_matrix_seq`, `make_matrix_t` — matrix_detail.hpp:196–215.
- `coord_generator<F, T, Dims...>` (concept) — matrix.hpp:2454 (dans `namespace detail` ré-ouvert au sein de matrix.hpp).

Aucun de ces symboles n'apparaît dans la surface publique. **0 fuite détectée.**

---

## 5. Analyse de cohérence et findings

### A — `[[nodiscard]]`

0 fonction non-`void` à valeur de retour pure-fonctionnelle sans `[[nodiscard]]`. Les exceptions justifiées (pattern C++ standard) :

- Toutes les `operator=` (copy, move, templates U).
- Les `operator()` mutants (retournent une référence mutable, intentionnellement utilisable comme cible).
- Tous les compound assignment (`+=`, `-=`, `*=`, `/=`, `&=`, `|=`, `^=`, `<<=`, `>>=`) qui retournent `*this`.
- Constructeurs (n'ont pas de type de retour).
- Conversions implicites (`matrix_view(matrix&)`, `operator matrix_view<strided>()`).
- `operator<<` (stream operator standard).

### B — `noexcept`

Cohérence parfaite :

- Toutes les `at()` (matrix, matrix_view×2) sont **non-`noexcept`** car elles `throw std::out_of_range`. ✓
- Tous les `operator()` (sans bounds check) sont `noexcept`. ✓
- Les opérations dépendantes du type T sont `noexcept(...)` conditionnel (ex: `fill`, `swap`, `operator-` unaire). ✓
- L'opérateur de conversion `contiguous → strided` est `noexcept` (calcule des `std::size_t`, aucune lancée d'exception). ✓

### C — `constexpr`

**F1 corrigé.** L'opérateur de conversion `matrix_view<T, contiguous, …>::operator matrix_view<T, strided, …>() const noexcept` n'était pas `constexpr` (déclaration matrix_view.hpp:214, définition matrix_view.hpp:1306). Son corps n'utilise que des constructions `constexpr`-friendly (boucles `for` sur `std::size_t`, `std::array` agrégat, multiplications). L'ajout de `constexpr` **élargit** le contrat (non-breaking) et permet de constituer un `matrix_view<strided>` à partir d'un `matrix_view<contiguous>` en contexte `constexpr` (notamment dans des tests `static_assert`).

Hors F1, la cohérence `constexpr` est respectée : seules les fonctions qui ne peuvent pas l'être (I/O via `std::ostream`, lancées d'exceptions dans `at()`, `slice()` avec checks dynamiques, constructeur 2D `initializer_list`) ne sont pas `constexpr`. Le reste — y compris `transpose`, `matmul`, `dot`, `generate`, les factories `zeros`/`ones`/`identity`/`full`, et les itérateurs — l'est.

### D — `explicit`

Conversions implicites volontaires (documentées et analogues à `std::string_view`) :

- `matrix_view(matrix<T, Dims...>&)` (NOLINTNEXTLINE, matrix_view.hpp:148).
- `matrix_view(const matrix<U, Dims...>&) requires same_as<T, const U>` (NOLINTNEXTLINE, matrix_view.hpp:169).
- `operator matrix_view<T, strided, Dims...>()` (NOLINTNEXTLINE, matrix_view.hpp:214).

Constructeurs à 1 argument explicites (conformes à la guideline) : tous les autres single-arg dans `matrix` (depuis `matrix_view`, `std::array`, `std::span`, `matrix_zero_t`). Le constructeur variadic templaté est `explicit(sizeof...(Args)==1)` pour éviter la conversion implicite depuis un scalaire seul tout en autorisant la liste agrégat. ✓

### E — Naming

Conventions respectées partout :

- Fonctions et variables : `snake_case` (`transpose`, `matmul`, `flatten`, `dimensions`, `value_type`, …).
- Paramètres template : `PascalCase` ou identifiants descriptifs (`T`, `U`, `Dims`, `Dimensions`, `Coords`, `Storage`, `ElemT`, `Other`, `Scalar`, `F`, `Args`).
- Tags : `snake_case` (`contiguous`, `strided`, `all_t`, `matrix_zero_t`).

### F — Fuites de `detail::`

`grep` exhaustif sur les 3 headers : aucun symbole de `ysc::detail::` n'est exporté sous `ysc::`. Les références à `detail::` dans les déclarations publiques sont toutes des dépendances internes (clauses `requires`, utilisations dans le corps de fonctions) — pas des entrées d'API.

### G — Point d'attention `enumerate_range` (F2 corrigé)

**F2 corrigé.** La classe imbriquée publique `matrix::enumerate_range<ElemT>` (matrix.hpp:2179) exposait un constructeur public `enumerate_range(ElemT* ptr, std::size_t n)` prenant un raw pointer. Un utilisateur ne devrait jamais le construire directement — `matrix::enumerate()` est la seule fabrique légitime. Vérification effectuée : `grep -r enumerate_range test/ examples/` retourne 0 match (l'utilisation passe toujours par `matrix::enumerate()` puis range-for). Le constructeur a été déplacé en `private:` et `friend class matrix;` a été ajouté afin que `matrix::enumerate()` puisse continuer à le fabriquer. `begin()`, `end()`, `value_type`, la classe `iterator` imbriquée restent publics (indispensables pour le range-for).

---

## 6. Findings & corrections

| ID | Description | Statut | Risque pour les consommateurs |
|---|---|---|---|
| F1 | Opérateur de conversion `matrix_view<contiguous>::operator matrix_view<strided>()` non-`constexpr`. | **Corrigé dans cette PR** (matrix_view.hpp:214 et :1306). | Non-breaking : ajout de `constexpr` élargit le contrat. |
| F2 | Constructeur de la classe imbriquée publique `enumerate_range<ElemT>(ElemT*, size_t)` exposé publiquement. | **Corrigé dans cette PR** (matrix.hpp:2179–2235). | Non-breaking pratique : 0 référence externe identifiée (tests + exemples). En théorie, breaking pour un consommateur qui aurait construit `enumerate_range` directement avec un raw pointer (cas non-supporté par la spec). |

## 7. Critères de succès (cf. `doc/before-v1-checks.md` §1)

- [x] Tableau récapitulatif (symbole, signature, attributs) produit.
- [x] 0 symbole signalé comme « probablement devrait être `detail::` ».
- [x] 0 fonction non-`void` sans `[[nodiscard]]` non justifié.
- [x] 0 incohérence `noexcept`/`constexpr` entre fonctions analogues (F1 corrigé).

## 8. Verdict

**Vert.** L'API publique de `ysc::matrix` est saine et prête pour le freeze v1.0.0 après application de F1 et F2 dans cette PR. Aucun symbole `ysc::detail::` ne fuit dans la surface publique ; la cohérence `noexcept`/`constexpr`/`[[nodiscard]]` est solide ; les conversions implicites volontaires sont documentées et alignées sur l'analogue `std::string_view` ; le naming respecte les conventions du projet.

Aucune US supplémentaire n'est requise pour clore l'axe 1.
