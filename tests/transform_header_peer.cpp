#include "cxx11_mode.hpp"
// Include the legacy forwarding header first in this translation unit.
#include <linscaler/transform/rational_index_map.hpp>
#include <linscaler/transform/rational_affine_map.hpp>
#include <linscaler/transform/grid_map.hpp>
#include <linscaler/transform/floating_affine_map.hpp>
#include <linscaler/transform/mapper.hpp>
#include "test_support.hpp"

long double transform_header_peer() {
    using linscaler::linsp::linsp_;
    using namespace linscaler::transform;
    const linsp_<int> integers(2, 3, 4);
    const linsp_<double> samples(0.5, 0.5, 20);
    CHECK(coordinate_at(integers, 2) == 8);
    CHECK(fractional_index(integers, 8) == 2);
    CHECK(map_index(integers, 0.5) == 3);
    CHECK(find_index(samples, 1.5) == 2);
    CHECK(map_index(samples, 2) == 1.5);
    return map_index_between(integers, samples, 2);
}

linscaler::transform::index_position transform_position_peer() {
    return linscaler::transform::rational_affine_map(5, 3, -1)(1);
}

linscaler::transform::floating_affine_map<double> transform_affine_peer() {
    return linscaler::transform::compile_grid_map<double>(
        linscaler::linsp::linsp_<int>(2, 3, 4),
        linscaler::linsp::linsp_<double>(0.5, 0.5, 20));
}
