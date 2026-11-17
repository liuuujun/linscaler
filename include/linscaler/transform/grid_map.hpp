#ifndef LINSCALER_TRANSFORM_GRID_MAP_HPP
#define LINSCALER_TRANSFORM_GRID_MAP_HPP

#include "../core/linsp.hpp"
#include "floating_affine_map.hpp"
#include <cmath>
#include <limits>
#include <stdexcept>

namespace linscaler {
namespace transform {
namespace detail {

template<typename Real>
Real affine_narrow_coefficient(long double value) {
    if (!std::isfinite(value) ||
        value < static_cast<long double>((std::numeric_limits<Real>::lowest)()) ||
        value > static_cast<long double>((std::numeric_limits<Real>::max)()))
        throw std::overflow_error("affine coefficient outside result type range");
    return affine_finite_result(static_cast<Real>(value));
}

} // namespace detail

/// Compile a coordinate-preserving source-index -> target-index affine map.
/// Grids must share a coordinate system; no unit inference or count-based clipping.
/// Long double intermediates are bounded and may lose integer/floating precision.
/// This is a distinct numerical path from map_index_between, not a bitwise replacement.
template<typename Real = long double, typename Source, typename Target>
floating_affine_map<Real> compile_grid_map(const linsp::linsp_<Source>& source,
                                          const linsp::linsp_<Target>& target) {
    if (source.empty() || target.empty())
        throw std::invalid_argument("grid map requires nonempty grids");
    if (target.constant())
        throw std::invalid_argument("target grid has no unique inverse");

    const long double source_step = static_cast<long double>(source.step());
    const long double target_step = static_cast<long double>(target.step());
    const long double scale = detail::affine_scale_result(
        source_step / target_step, !source.constant());
    const long double difference = detail::affine_finite_result(
        static_cast<long double>(source.first()) - static_cast<long double>(target.first()));
    const long double offset = detail::affine_finite_result(difference / target_step);

    const Real result_scale = detail::affine_scale_result(
        detail::affine_narrow_coefficient<Real>(scale), !source.constant());
    const Real result_offset = detail::affine_narrow_coefficient<Real>(offset);
    return floating_affine_map<Real>(result_scale, result_offset);
}

} // namespace transform
} // namespace linscaler
#endif
