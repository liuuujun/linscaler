#ifndef LINSCALER_TRANSFORM_MAPPER_HPP
#define LINSCALER_TRANSFORM_MAPPER_HPP
#include "../core/linsp.hpp"
#include <cmath>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace linscaler {
namespace transform {
namespace detail {
template<typename T>
T mapped_coordinate(long double value, std::true_type) {
    const auto truncated = std::trunc(value);
    // Power-of-two exclusive upper bound stays exact even when long double == double.
    const auto upper = std::ldexp(1.0L, std::numeric_limits<T>::digits);
    const auto lower = std::is_signed<T>::value ? -upper : 0.0L;
    if (truncated < lower || truncated >= upper)
        throw std::overflow_error("integer coordinate overflow");
    return static_cast<T>(truncated);
}
template<typename T>
T mapped_coordinate(long double value, std::false_type) {
    return linsp::detail::from_real<T>(value);
}
} // namespace detail

/// Continuous coordinate at an index; extrapolation is intentional.
template<typename T>
long double coordinate_at(const linsp::linsp_<T>& grid, long double index) {
    if (grid.empty()) throw std::invalid_argument("cannot map an empty grid");
    if (!std::isfinite(index)) throw std::invalid_argument("index must be finite");
    const auto value = static_cast<long double>(grid.first()) + index * grid.step();
    if (!std::isfinite(value)) throw std::overflow_error("coordinate overflow");
    return value;
}
/// Inverse coordinate map. A constant grid has no unique inverse.
template<typename T>
long double fractional_index(const linsp::linsp_<T>& grid, long double value) {
    if (grid.empty() || grid.constant()) throw std::invalid_argument("inverse requires a nonempty, nonconstant grid");
    if (!std::isfinite(value)) throw std::invalid_argument("coordinate must be finite");
    const auto index = (value - static_cast<long double>(grid.first())) / grid.step();
    if (!std::isfinite(index)) throw std::overflow_error("index overflow");
    return index;
}
/// Preserve physical coordinate, converting an index between grids sharing a coordinate system.
template<typename Source, typename Target>
long double map_index_between(const linsp::linsp_<Source>& source,
                              const linsp::linsp_<Target>& target, long double source_index) {
    return fractional_index(target, coordinate_at(source, source_index));
}

/// Compatibility API: constant grids map to index zero; bounds are not checked.
template<typename T>
double find_index(const linsp::linsp_<T>& source, T value) {
    if (source.empty()) throw std::invalid_argument("cannot map an empty grid");
    if (!linsp::detail::finite_coordinate(value, std::is_floating_point<T>()))
        throw std::invalid_argument("coordinate must be finite");
    if (source.constant()) return 0;
    const auto result = fractional_index(source, static_cast<long double>(value));
    return linsp::detail::from_real<double>(result);
}
/// Compatibility API: integer coordinates truncate the final coordinate toward zero.
template<typename T>
T map_index(const linsp::linsp_<T>& target, double index) {
    const auto value = coordinate_at(target, index);
    return detail::mapped_coordinate<T>(value, std::is_integral<T>());
}
/// Preserve index (not physical position). Source must be a grid member.
/// Target extrapolation is intentional and documented.
template<typename T>
T map_value(const linsp::linsp_<T>& source, const linsp::linsp_<T>& target, T value) {
    if (!source.contains(value)) throw std::out_of_range("value is not a source grid member");
    return map_index(target, find_index(source, value));
}
} // namespace transform
} // namespace linscaler
#endif

