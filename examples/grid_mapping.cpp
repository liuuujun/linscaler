#include <linscaler/transform/mapper.hpp>
#include <linscaler/transform/grid_map.hpp>
#include <linscaler/transform/rational_affine_map.hpp>
#include <cmath>
#include <iostream>
int main() {
    // Sample timestamps in seconds; map a 44.1 kHz index into a 48 kHz grid.
    const linscaler::linsp::linsp_<double> source(0, 1.0 / 44100, 44100);
    const linscaler::linsp::linsp_<double> target(0, 1.0 / 48000, 48000);
    const long double mapped = linscaler::transform::map_index_between(source, target, 441);
    if (!std::isfinite(mapped) || std::abs(mapped - 480.0L) > 1e-10L) return 1;
    const auto compiled = linscaler::transform::compile_grid_map(source, target);
    if (std::abs(compiled(441) - 480.0L) > 1e-10L) return 1;
    // This example's displayed value fits double. The reference Windows runtime
    // misformats long double through streams; the calculation above stays extended precision.
    std::cout << "Source index 441 maps to target index "
              << static_cast<double>(mapped) << '\n';
    // Exact ratio for integer sample indices. No interpolation of signal values is performed.
    const linscaler::transform::rational_affine_map exact(160, 147);
    return exact(441).floor == 480 && exact(441).remainder == 0 ? 0 : 1;
}

