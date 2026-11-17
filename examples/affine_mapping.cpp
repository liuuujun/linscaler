#include <linscaler/transform/grid_map.hpp>
#include <linscaler/transform/rational_affine_map.hpp>
#include <cmath>
#include <iostream>

int main() {
    using linscaler::linsp::linsp_;
    using namespace linscaler::transform;

    // Two uniform coordinate grids. A source index of 2 has coordinate 22.
    const linsp_<int> source(10, 6, 5);
    const linsp_<int> target(4, 4, 10);
    const auto map = compile_grid_map<double>(source, target);
    if (map.scale() != 1.5 || map.offset() != 1.5 || map(2) != 4.5) return 1;

    // A subsequent change of index origin; compose applies map before shift.
    const floating_affine_map<double> shift(1.0, -1.0);
    const auto shifted = compose(map, shift);
    if (shifted(2) != 3.5 || std::abs(inverse(map)(4.5) - 2.0) > 1e-12) return 1;

    // These explicitly known integer coefficients also admit an exact representation.
    const auto exact = rational_affine_map(3, 2, 3)(2);
    if (exact.floor != 4 || exact.remainder != 1 || exact.denominator != 2) return 1;
    std::cout << "Source index 2 maps to target index " << map(2)
              << "; exact position is " << exact.floor << " + "
              << exact.remainder << '/' << exact.denominator << '\n';
}
