#include "../cxx11_mode.hpp"
#include <linscaler/transform/floating_affine_map.hpp>
#include <linscaler/transform/grid_map.hpp>
#include <linscaler/transform/mapper.hpp>
#include <linscaler/transform/rational_affine_map.hpp>
#include <linscaler/transform/rational_index_map.hpp>

int main() {
    using linscaler::linsp::linsp_;
    using namespace linscaler::transform;
    const linsp_<int> source(0, 2, 10);
    const linsp_<double> target(0.5, 0.5, 40);
    const auto index = map_index_between(source, target, 3);
    const auto position = rational_index_map(5, 3, -1)(1);
    const auto exact = rational_affine_map(5, 3, -1).shifted_input(1)(0);
    const auto compiled = compile_grid_map<double>(source, target);
    const auto translated = compose(compiled, floating_affine_map<double>(1, 2));
    return index == 11 && map_index(target, static_cast<double>(index)) == 6 &&
           find_index(source, 6) == 3 && map_value(source, source, 6) == 6 &&
           position.floor == 1 && position.remainder == 1 &&
           position.denominator == 3 && exact.floor == 1 && exact.remainder == 1 &&
           compiled.scale() == 4 && compiled.offset() == -1 && compiled(3) == 11 &&
           inverse(compiled)(11) == 3 && translated(3) == 13 ? 0 : 1;
}
