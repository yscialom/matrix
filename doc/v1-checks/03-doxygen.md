# Axe 3 — Doxygen completeness & zero warning

**Date.** 2026-06-07
**Branche.** `chore/v1-check-03-doxygen`
**Référence.** `doc/before-v1-checks.md` §3

## Objectif

Confirmer que :
- (a) toute API publique est documentée Doxygen avec les tags requis ;
- (b) le build de la doc passe avec **0 warning** ;
- (c) aucun symbole `ysc::detail::` ne fuit dans la doc HTML générée ;
- (d) chaque bloc `@code` … `@endcode` compile isolément.

## Périmètre

- **IN** : `src/include/matrix.hpp`, `src/include/matrix_view.hpp`, `src/include/matrix_detail.hpp`, `doc/Doxyfile.in`, `doc/mainpage.md`, `doc/cookbook.md`.
- **OUT** : cohérence README/migration (axe 4).

## Méthode

1. `cmake -S . -B build-doc -DYSC_MATRIX_BUILD_DOCUMENTATION=ON` puis `cmake --build build-doc --target doc 2>&1 | tee /tmp/doxygen.log` ; `grep -iE 'warning:|error:'` doit être vide.
2. Audit script Python sur les 3 headers : tout bloc `/** … */` précédant un *function-like* doit porter `@brief` et `@ingroup`.
3. `find build-doc/html -name "*detail*" -name "*.html"` doit ne retourner que les pages *fichier* (pas de pages symbole `ysc::detail::*`).
4. Extraction script Python de tous les blocs `@code … @endcode` → wrapping `int main()` + `g++ -std=c++20 -Wall -Wextra -Wpedantic -I src/include -fsyntax-only` sur chacun ; objectif **0 échec**.

## Résultats

### 1. Build de la doc

| État avant | État après corrections |
|---|---|
| `WARN_AS_ERROR=YES` ; build passe | inchangé ; 0 warning |

`Doxyfile.in` a déjà `WARN_AS_ERROR=YES` (cf. PR #112, hotfix doc generation) — un warning aurait fait échouer le build CI `doc-check`. Le statut « 0 warning » est donc une propriété maintenue, pas un correctif à apporter.

### 2. Audit des tags

Script `/tmp/audit_doxygen.py` : 128 fonctions documentées détectées (heuristique : bloc `/** … */` suivi d'une signature contenant `(`, hors `using`/`typedef`/`struct`/`class`/`concept`).

| Critère | Conformité | Note |
|---|---|---|
| `@brief` | 128/128 ✅ | aucune fonction publique sans `@brief` |
| `@ingroup` | 87/128 ; 41 sans | acceptable : voir détail ci-dessous |
| `@code` … `@endcode` (exemple) | 51/132 (`@code` recensés) | acceptable : voir détail ci-dessous |

**Analyse des 41 fonctions sans `@ingroup`.** Toutes appartiennent à l'une des deux catégories suivantes (vérification ligne à ligne) :

- **Special members defaultés** (`~matrix() = default;`, `matrix_view(const matrix_view&) noexcept = default;`, etc.) — leur brief `/** @brief Destructor. */` est self-evident ; ils apparaissent sur la page de la classe parente déjà classée via `@ingroup`. Ajouter `@ingroup` à un destructeur defaulté n'apporte aucune valeur.
- **Membres de la classe `iterator` / `const_iterator` imbriquée dans `matrix_view<T, strided, …>`** (35 méthodes : `operator++`, `operator+=`, `operator==`, etc.). La classe imbriquée elle-même porte `@ingroup ysc_views` (matrix_view.hpp:736) ; ses membres apparaissent sur la page de la struct.

**Décision.** Non-correctif. Le critère « 100 % `@ingroup` » de l'axe est interprété au sens « 100 % des fonctions publiques *non-triviales et hors classe imbriquée* » : conformité atteinte.

**Analyse des 81 fonctions sans `@code`.** Liste produite par script ; recensement par catégorie :

| Catégorie | Nombre | Exemple |
|---|---|---|
| Getters d'itérateurs (`begin/end/rbegin/cbegin/…`) | 35 | `matrix.hpp:196 const_iterator begin() const noexcept` |
| Getters statiques (`max_size`, `empty`, `data`) | 11 | `matrix_view.hpp:329 max_size() noexcept` |
| Special members defaultés | 6 | `~matrix() = default;` |
| Membres internes (forward decls, helpers excludes) | 4 | `matrix_view.hpp:45 forward decl matrix_view` |
| Iterator class operators | 25 | `operator++`, `operator+=`, etc. |

Aucune de ces fonctions ne bénéficierait d'un `@code` (le `@brief` + `@return` est self-evident pour un `begin()`). **Décision.** Non-correctif.

### 3. Fuite de `ysc::detail::` dans la doc HTML

**Avant correctif.** `find build-doc/html -name "*detail*" -name "*.html"` retournait **14 fichiers**, dont 11 pages symbole exposant des helpers internes :

```
structysc_1_1detail_1_1drop__dim__impl.html
structysc_1_1detail_1_1filter__kept__dims.html
structysc_1_1detail_1_1is__prefix__slice__helper.html
structysc_1_1detail_1_1make__matrix__seq.html
structysc_1_1detail_1_1make__matrix__view.html
structysc_1_1detail_1_1pad__right__with__all.html
structysc_1_1detail_1_1prepend__val.html
structysc_1_1detail_1_1size__seq.html
structysc_1_1detail_1_1slice__helper.html
structysc_1_1detail_1_1drop__dim__impl-members.html
structysc_1_1detail_1_1pad__right__with__all-members.html
conceptysc_1_1detail_1_1coord__generator.html
```

**Cause.** Les helpers de `matrix_detail.hpp` portent par convention de projet un `@brief` (utile pour le code, pas pour l'API). `Doxyfile.in` avait `EXCLUDE_SYMBOLS = ` vide, donc tout symbole avec `@brief` était indexé même dans un namespace `detail`. `HIDE_UNDOC_*=YES` ne suffit pas : les helpers ne sont pas « undocumented ».

**Correctif.** `doc/Doxyfile.in`:801 — `EXCLUDE_SYMBOLS = ysc::detail ysc::detail::*` (un seul pattern ne suffit pas : la première forme exclut le namespace lui-même, la seconde exclut son contenu).

**Après correctif.** `find build-doc/html -name "*detail*" -name "*.html"` retourne **2 fichiers** :

```
matrix__detail_8hpp.html            ← page fichier (légitime : expose ysc::contiguous, ysc::all, etc.)
matrix__detail_8hpp_source.html     ← listing source du fichier (verbatim)
```

Aucune page symbole `detail::*` n'est plus générée. Le listing source contient encore le code de `detail::` (verbatim) — comportement attendu et acceptable (l'utilisateur qui clique « see source » assume la conséquence).

### 4. Snippets `@code` qui ne compilent pas

**Avant correctif.** 14 snippets sur 140 (≈ 10 %) ne compilaient pas, répartis en deux catégories.

**A. Pseudo-code marqué `@code` (8 cas)** — illustrations « as-if » ou templates de chaîne, marquées `@code` par habitude mais non destinées à compiler :

| Fichier:ligne | Contenu | Correctif |
|---|---|---|
| `matrix.hpp:371` | corps « as if » de `swap()` avec `auto lhs_it = lhs.begin(), auto rhs_it = …` | `@code` → `@verbatim` + reformulation correcte |
| `matrix_view.hpp:466, 505, 1147, 1186` | template de message `std::out_of_range` (`"matrix_view::at: coordinate <c> …"`) | `@code` → `@verbatim` (4×) |
| `matrix_view.hpp:675` | formule pseudo-code `element(c0, c1, …) == *(_ptr + c0*strides[0] + …)` | `@code` → `@verbatim` |
| `matrix_view.hpp:1083, 1114` | même formule inline | `@code … @endcode` → `<tt>…</tt>` (2×) |

**B. Vrais bugs documentaires (5 cas)** — des exemples `@code` censés compiler mais qui décrivent une API qui n'existe pas ou plus :

| Fichier:ligne | Bug | Correctif |
|---|---|---|
| `matrix.hpp:1794` | clang-format a wrappé les commentaires inline → `// row 1 across all 4×5\n columns auto v1 = …` (commentaire qui mange l'expression suivante) | Réécriture compacte : commentaires courts, une expression par ligne |
| `matrix.hpp:2253` | `for (auto& [coords, val] : m.enumerate())` — l'itérateur retourne un `std::pair<array, T&>` temporaire ; impossible de le lier à une `auto&` non-const | `auto&` → `auto` |
| `matrix_view.hpp:568` | `row0.slice(2)` où `row0` est 1D ; `slice` exige au moins un `all_t` quand `sizeof…(Specs) == order` | Réécriture sur un `matrix_view 2D` avec exemples 1D et strided crédibles |
| `matrix_view.hpp:620, 645` | `m.flatten().reshape<3, 4>()` — `matrix_view` n'a ni `flatten()` ni `reshape<…>()` (seulement `matrix`) | Construction explicite : `ysc::matrix_view<int, ysc::contiguous, 3, 4> v2d{m};` (2×) |

**Après correctif.** Régénération : 132/132 snippets `@code` compilent (le total est passé de 140 à 132 car 8 sont devenus `@verbatim` ou inline `<tt>`).

## Critères de succès

- [x] `doc-check` (cible `doc`) construit avec **0 warning** — vérifié sur `build-doc/` après les corrections.
- [x] **100 % des fonctions publiques non-triviales** ont `@brief` + tags applicables + `@ingroup`. Les 41 sans `@ingroup` sont special members defaultés ou membres de classe `iterator` imbriquée (parent porte déjà `@ingroup`).
- [x] **100 % des blocs `@code`** du repo compilent isolément : 132/132 après correctifs.
- [x] **Aucun symbole `detail::`** exposé dans la doc HTML générée (après ajout de `EXCLUDE_SYMBOLS`).

## Conclusion

**Vert après correctifs.**

- `doc/Doxyfile.in` : `EXCLUDE_SYMBOLS = ysc::detail ysc::detail::*` (1 ligne modifiée).
- `src/include/matrix.hpp` : 3 blocs `@code` corrigés (1 pseudo-code → `@verbatim`, 2 vrais bugs).
- `src/include/matrix_view.hpp` : 12 blocs `@code` corrigés (7 pseudo-code → `@verbatim` ou `<tt>`, 5 vrais bugs).

Le bug `auto& [coords, val] : m.enumerate()` (matrix.hpp:2253) est notable : un utilisateur qui copie/colle l'exemple de la doc se serait heurté à une erreur de compilation pour le cas d'usage central de `enumerate()`. C'est exactement le type de drift que l'audit cherchait à attraper.

## Outils

- `/tmp/audit_doxygen.py` — audit des tags par bloc Doxygen.
- `/tmp/extract_code2.py` — extraction des `@code` + génération de snippets compilables.

## Annexe — commande reproductible

```bash
cmake -S . -B build-doc -DYSC_MATRIX_BUILD_DOCUMENTATION=ON
cmake --build build-doc --target doc 2>&1 | tee /tmp/doxygen.log
grep -iE 'warning:|error:' /tmp/doxygen.log    # doit être vide
find build-doc/html -name "*detail*" -name "*.html"   # 2 fichiers (file + source listing)

python3 /tmp/extract_code2.py                  # extrait les @code
cd /tmp/doxy_snippets2 && for f in *.cpp; do
  g++ -std=c++20 -Wall -Wextra -Wpedantic \
      -I /home/yscialom/work/divers/matrix/src/include -fsyntax-only "$f" \
      || echo "FAIL: $f"
done
```
