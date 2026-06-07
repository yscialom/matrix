# Axe 2 — Header self-containment & includes

**Date.** 2026-06-07
**Branche.** `chore/v1-check-02-self-contained-headers`
**Référence.** `doc/before-v1-checks.md` §2

## Objectif

Garantir que chaque header public compile isolément et inclut tout ce qu'il utilise (IWYU strict côté API publique).

## Périmètre

- **IN** : `src/include/matrix.hpp`, `src/include/matrix_view.hpp`, `src/include/matrix_detail.hpp`.
- **OUT** : tests, exemples (couverts par l'axe 9 côté consommation).

## Méthode

1. Recensement de chaque `std::*` utilisé dans le code (hors blocs `@code` Doxygen).
2. Comparaison contre la liste des `#include` directs du header.
3. Compilation de chaque header isolé via un `.cpp` minimal :
   ```cpp
   #include <matrix.hpp>          // ou matrix_view.hpp, ou matrix_detail.hpp
   int main() { return 0; }
   ```
   avec `-std=c++20 -Wall -Wextra -Wpedantic -Werror`.

## Résultats de l'audit

### `matrix_detail.hpp`

| Symbole std utilisé | Header attendu | Présent avant ? | Action |
|---|---|---|---|
| `std::array` | `<array>` | oui | — |
| `std::conditional_t` | `<type_traits>` | oui | — |
| `std::integral` | `<concepts>` | oui | — |
| `std::ostream` | `<ostream>` | oui | — |
| `std::remove_cvref_t` | `<type_traits>` | oui | — |
| `std::same_as` | `<concepts>` | oui | — |
| `std::size_t` | `<cstddef>` | oui | — |
| `std::tuple` | `<tuple>` | **non** | **ajout `<tuple>`** |

### `matrix_view.hpp`

| Symbole std utilisé | Header attendu | Présent avant ? | Action |
|---|---|---|---|
| `std::apply` | `<tuple>` | oui | — |
| `std::array` | `<array>` | oui | — |
| `std::contiguous_iterator` | `<iterator>` | oui | — |
| `std::fill` | `<algorithm>` | oui | — |
| `std::random_access_iterator(_tag)` | `<iterator>` | oui | — |
| `std::reverse_iterator` | `<iterator>` | oui | — |
| `std::ostream` | `<ostream>` | oui | — |
| `std::out_of_range` | `<stdexcept>` | oui | — |
| `std::string`, `std::to_string` | `<string>` | oui | — |
| `std::ostringstream`, `std::format(ter)` | `<sstream>`, `<format>` (guardés) | oui | — |
| `std::ptrdiff_t` | `<cstddef>` | **non** | **ajout `<cstddef>`** |
| `std::size_t` | `<cstddef>` | **non** | **ajout `<cstddef>`** |
| `std::integral`, `std::same_as` | `<concepts>` | **non** | **ajout `<concepts>`** |
| `std::conditional_t`, `std::is_nothrow_copy_assignable_v`, `std::remove_cvref_t` | `<type_traits>` | **non** | **ajout `<type_traits>`** |

### `matrix.hpp`

| Symbole std utilisé | Header attendu | Présent avant ? | Action |
|---|---|---|---|
| `std::copy`, `std::transform`, `std::min`, `std::max` | `<algorithm>` | oui | — |
| `std::array` | `<array>` | oui | — |
| `operator<=>` (defaulted) | `<compare>` | oui | — |
| `std::same_as`, `std::convertible_to`, `std::default_initializable`, `std::invocable`, `std::totally_ordered`, `std::integral` | `<concepts>` | oui | — |
| `std::hash`, `std::invoke`, `std::multiplies` | `<functional>` | oui | — |
| `std::reverse_iterator`, `std::input_iterator_tag` | `<iterator>` | oui | — |
| `std::accumulate`, `std::iota` | `<numeric>` | oui | — |
| `std::ostream` | `<ostream>` | oui | — |
| `std::ranges::*` | `<ranges>` | oui | — |
| `std::span` | `<span>` | oui | — |
| `std::out_of_range`, `std::length_error` | `<stdexcept>` | oui | — |
| `std::string`, `std::to_string` | `<string>` | oui | — |
| `std::apply` | `<tuple>` | oui | — |
| `std::is_nothrow_*_v`, `std::remove_cvref_t`, `std::conditional_t`, `std::invoke_result_t` | `<type_traits>` | oui | — |
| `std::ostringstream`, `std::format(ter)` | `<sstream>`, `<format>` (guardés) | oui | — |
| `std::ptrdiff_t`, `std::size_t` | `<cstddef>` | **non** | **ajout `<cstddef>`** |
| `std::initializer_list` | `<initializer_list>` | **non** | **ajout `<initializer_list>`** |
| `std::move`, `std::forward`, `std::pair`, `std::swap`, `std::declval`, `std::index_sequence`, `std::make_index_sequence` | `<utility>` | **non** | **ajout `<utility>`** |

### Bug `<stdexcept>` (mémoire `bug_stdexcept_manquant.md`)

Le bug rapporté dans la mémoire est **déjà corrigé** : `<stdexcept>` est présent dans `matrix.hpp` et `matrix_view.hpp` depuis le commit `3c0c4b6` (US-001, pipeline CI multi-plateforme). La mémoire est obsolète et sera retirée.

## Vérification : compilations isolées

Trois `.cpp` minimaux compilés avec `-std=c++20 -Wall -Wextra -Wpedantic -Werror -I src/include`, après application des fixes :

| Compilateur | Optim | `matrix.hpp` | `matrix_view.hpp` | `matrix_detail.hpp` |
|---|---|---|---|---|
| g++-14 (14.3.0, libstdc++) | -O0 | OK | OK | OK |
| g++-14 (14.3.0, libstdc++) | -O2 | OK | OK | OK |
| g++-15 (15.2.0, libstdc++) | -O0 | OK | OK | OK |
| g++-15 (15.2.0, libstdc++) | -O2 | OK | OK | OK |
| clang++-20 (libstdc++) | -O0 | OK | OK | OK |
| clang++-20 (libstdc++) | -O2 | OK | OK | OK |

**Total : 18/18 OK.**

### Limites du sweep local

La matrice demandée par l'axe (`{g++-12, g++-13, clang++-15, clang++-17}` × `{libstdc++, libc++}` × `{-O0, -O2}` = 48 cas) n'est que partiellement reproductible localement :

- g++-12 et g++-13 ne sont pas installés (machine sous Ubuntu fournit g++-14 / g++-15).
- clang++-15 et clang++-17 ne sont pas installés (machine fournit clang++-20).
- libc++ n'est pas disponible sur cette machine.

La CI existante (`ci.yml`) couvre déjà cette matrice côté tests/build complet (Ubuntu × GCC 12/13/14 + Clang 15/17, macOS Apple Clang, Windows MSVC). Le PR vérifiera donc ces combinaisons automatiquement, et le passage CI valide en lui-même la self-containment puisque tout consommateur (tests, examples) `#include <matrix.hpp>` en premier.

## Vérification : build complet et tests

- `cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug` : OK.
- `cmake --build build --target check` : build vert, **1/1 test passé** (`matrix-test`).
- `cmake --build build --target format` (Docker clang-format-18) : diff inchangé, mes ajouts conformes au style.

## Note sur les includes lourds redondants

Tous les `#include` actuels couvrent au moins un symbole effectivement utilisé dans le code (hors blocs `@code` Doxygen). Aucun retrait n'est proposé.

## Conclusion

**Vert après corrections.**

- 6 `#include` directs manquants ajoutés (1 dans `matrix_detail.hpp`, 4 dans `matrix_view.hpp`, 3 dans `matrix.hpp`) ; ils étaient jusqu'ici tirés transitivement par les stdlibs disponibles.
- Aucun symbole `std::*` utilisé hors d'un `@code` ne reste sans `#include <Y>` direct.
- Bug historique `<stdexcept>` : déjà corrigé en amont (US-001) ; mémoire obsolète retirée.
- 18/18 compilations locales OK ; la CI étend la matrice aux compilateurs/stdlib manquants.
