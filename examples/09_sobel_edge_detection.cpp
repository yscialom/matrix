// 09_sobel_edge_detection.cpp — Sobel edge detection on a 16×16 image.
//
// Showcase: classical image-processing kernels declared at compile
// time and convolved through enumerate()-style traversal +
// submatrix<3,3> for the per-pixel neighbourhood.
//
// Sobel kernels:
//   Gx = [[-1,  0,  1],         Gy = [[-1, -2, -1],
//         [-2,  0,  2],               [ 0,  0,  0],
//         [-1,  0,  1]]               [ 1,  2,  1]]
//
// Gy is simply transpose(Gx) — both come from `ysc::transpose` so we
// declare only one kernel.

#include <matrix.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <exception>
#include <format>
#include <iostream>

namespace {

constexpr std::size_t H = 16;
constexpr std::size_t W = 16;

using image_t = ysc::matrix<int, H, W>;
using mag_t = ysc::matrix<double, H, W>;

constexpr ysc::matrix<int, 3, 3> Gx{-1, 0, 1, //
                                    -2, 0, 2, //
                                    -1, 0, 1};

void print_image(const char* label, const image_t& m) {
    std::cout << label << ":\n";
    for (std::size_t i = 0; i < H; ++i) {
        std::cout << "  ";
        for (std::size_t j = 0; j < W; ++j) {
            std::cout << (m(i, j) != 0 ? '#' : '.');
        }
        std::cout << '\n';
    }
}

void print_magnitudes(const char* label, const mag_t& m) {
    static constexpr const char* ramp = " .-+*#@";
    constexpr std::size_t levels = 7;
    double max_v = 0.0;
    for (const double v : m) {
        max_v = std::max(max_v, v);
    }
    std::cout << label << " (max=" << std::format("{:.1f}", max_v) << "):\n";
    for (std::size_t i = 0; i < H; ++i) {
        std::cout << "  ";
        for (std::size_t j = 0; j < W; ++j) {
            const std::size_t idx =
                (max_v > 0.0) ? static_cast<std::size_t>((m(i, j) / max_v) * (levels - 1)) : 0U;
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            std::cout << ramp[idx];
        }
        std::cout << '\n';
    }
}

void run() {
    // Synthetic image: white square of value 255 in the centre (rows 5..10,
    // cols 5..10) on a black background.
    image_t img{};
    for (std::size_t i = 5; i <= 10; ++i) {
        for (std::size_t j = 5; j <= 10; ++j) {
            img(i, j) = 255;
        }
    }
    print_image("source image", img);

    const auto Gy = ysc::transpose(Gx);

    // Convolve: for every interior pixel, compute Gx · N and Gy · N where
    // N is the 3×3 neighbourhood centred on the pixel. Magnitude is √(gx²+gy²).
    mag_t magnitude{};
    for (std::size_t i = 1; i + 1 < H; ++i) {
        for (std::size_t j = 1; j + 1 < W; ++j) {
            const ysc::matrix<int, 3, 3> n{img.submatrix<3, 3>({i - 1, j - 1})};
            const int gx = (n * Gx).sum(); // element-wise product, then sum
            const int gy = (n * Gy).sum();
            magnitude(i, j) = std::sqrt(static_cast<double>((gx * gx) + (gy * gy)));
        }
    }

    print_magnitudes("\nedge magnitude (Sobel)", magnitude);

    // Threshold via map to produce a binary edge image.
    constexpr double threshold = 100.0;
    const auto edges = magnitude.map([](double v) { return v > threshold ? 1 : 0; });
    image_t edge_img{};
    for (auto [coords, e] : edges.enumerate()) {
        edge_img(coords[0], coords[1]) = (e != 0) ? 255 : 0;
    }
    print_image("\nedges (threshold)", edge_img);
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
