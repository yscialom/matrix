// 07_game_of_life.cpp — Conway's Game of Life on an 8×8 grid.
//
// Showcase: enumerate() + submatrix<3,3>() form a natural 2D stencil
// pattern: iterate every (coords, cell) pair, look at its Moore
// neighbourhood as a 3×3 sub-block, decide the cell's next state.
//
// Boundary cells (the outer ring) are held at 0; the live area is the
// interior 6×6 — sufficient for a glider to evolve a few generations.

#include <matrix.hpp>

#include <exception>
#include <iostream>

namespace {

using grid_t = ysc::matrix<int, 8, 8>;

void print(const grid_t& g, int generation) {
    std::cout << "── generation " << generation << " ──\n";
    for (std::size_t i = 0; i < 8; ++i) {
        std::cout << "  ";
        for (std::size_t j = 0; j < 8; ++j) {
            std::cout << (g(i, j) != 0 ? '#' : '.');
        }
        std::cout << '\n';
    }
}

// g is taken by value: submatrix() on a mutable grid yields a
// matrix_view<int, strided, …> that can be materialized into a matrix<int>;
// on a const grid it would return matrix_view<const int, …> with no
// matching matrix(const …) constructor.
grid_t step(grid_t g) {
    grid_t next{};
    for (auto [coords, cell] : g.enumerate()) {
        const std::size_t i = coords[0];
        const std::size_t j = coords[1];
        if (i == 0 || i == 7 || j == 0 || j == 7) {
            continue; // boundary stays dead
        }

        // 3×3 Moore neighbourhood as a sub-view; materialize to a matrix so
        // we can take .sum() on the block.
        const ysc::matrix<int, 3, 3> neighbourhood{g.submatrix<3, 3>({i - 1, j - 1})};

        // 8-neighbours sum = sum of the 3×3 block minus the cell itself.
        const int live_neighbours = neighbourhood.sum() - cell;

        // Standard Conway rules.
        next(i, j) = ((cell != 0) && (live_neighbours == 2 || live_neighbours == 3)) ||
                             ((cell == 0) && live_neighbours == 3)
                         ? 1
                         : 0;
    }
    return next;
}

void run() {
    // A glider near the top-left corner of the interior region.
    // clang-format off
    grid_t g{0, 0, 0, 0, 0, 0, 0, 0,
             0, 0, 1, 0, 0, 0, 0, 0,
             0, 0, 0, 1, 0, 0, 0, 0,
             0, 1, 1, 1, 0, 0, 0, 0,
             0, 0, 0, 0, 0, 0, 0, 0,
             0, 0, 0, 0, 0, 0, 0, 0,
             0, 0, 0, 0, 0, 0, 0, 0,
             0, 0, 0, 0, 0, 0, 0, 0};
    // clang-format on

    constexpr int generations = 10;
    for (int gen = 0; gen <= generations; ++gen) {
        print(g, gen);
        if (gen < generations) {
            g = step(g);
        }
    }

    std::cout << "\nfinal grid (operator<<):\n  " << g << '\n';
    std::cout << "alive cells     = " << g.sum() << '\n';
}

} // namespace

int main() {
    try {
        run();
    } catch (const std::exception& e) {
        std::cerr << "unexpected exception: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
