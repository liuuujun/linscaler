#include "cxx11_mode.hpp"
#include <linscaler/transform/floating_affine_map.hpp>
#include <linscaler/transform/grid_map.hpp>
#include <linscaler/transform/mapper.hpp>
#include <linscaler/transform/rational_affine_map.hpp>
#include <linscaler/transform/rational_index_map.hpp>
#include "test_support.hpp"

long double transform_header_peer();
linscaler::transform::index_position transform_position_peer();
linscaler::transform::floating_affine_map<double> transform_affine_peer();

int main() {
    using linscaler::linsp::linsp_;
    using namespace linscaler::transform;
    const linsp_<int> integers(2, 3, 4);
    const linsp_<double> samples(0.5, 0.5, 20);
    CHECK(coordinate_at(integers, 2) == 8);
    CHECK(fractional_index(integers, 8) == 2);
    CHECK(map_index(integers, 0.5) == 3);
    CHECK(find_index(samples, 1.5) == 2);
    CHECK(map_index(samples, 2) == 1.5);
    CHECK(map_index_between(integers, samples, 2) == 15);
    CHECK(transform_header_peer() == 15);
    const auto position = rational_index_map(5, 3, -1)(1);
    const auto peer = transform_position_peer();
    CHECK(position.floor == 1 && position.remainder == 1 && position.denominator == 3);
    CHECK(peer.floor == position.floor && peer.remainder == position.remainder &&
          peer.denominator == position.denominator);
    const auto compiled = compile_grid_map<double>(integers, samples);
    CHECK(compiled.scale() == 6 && compiled.offset() == 3);
    CHECK(compiled(2) == 15);
    const auto affine_peer = transform_affine_peer();
    CHECK(affine_peer.scale() == compiled.scale() && affine_peer.offset() == compiled.offset());
    CHECK(compose(compiled, inverse(compiled))(2) == 2);
}
