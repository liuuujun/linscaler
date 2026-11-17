#ifndef LINSCALER_BASIC_PRECISION_HPP
#define LINSCALER_BASIC_PRECISION_HPP
#include <cmath>
#include <stdexcept>
#include <type_traits>
namespace linscaler {
namespace precision {
// A value constant with internal linkage, also usable in C++11 translation units.
constexpr double default_epsilon = 1e-6;
inline void validate_epsilon(double epsilon) {
    if (!std::isfinite(epsilon) || epsilon < 0)
        throw std::invalid_argument("epsilon must be finite and nonnegative");
}
namespace detail {
template<typename T>
inline bool approx_equal(T a, T b, double epsilon, std::true_type) {
    if (!std::isfinite(a) || !std::isfinite(b)) return false;
    return std::abs(static_cast<long double>(a) - b) <= epsilon;
}
template<typename T>
inline bool approx_equal(T a, T b, double, std::false_type) {
    return a == b;
}
template<typename T>
inline bool negative_one(T value, std::true_type) { return value == -1; }
template<typename T>
inline bool negative_one(T, std::false_type) { return false; }

template<typename T>
inline bool aligned(T diff, T step, double epsilon, std::true_type) {
    if (!std::isfinite(diff) || !std::isfinite(step)) return false;
    if (step == 0) return std::abs(diff) <= epsilon;
    return std::abs(std::remainder(static_cast<long double>(diff),
                                   static_cast<long double>(step))) <= epsilon;
}
template<typename T>
inline bool aligned(T diff, T step, double, std::false_type) {
    if (step == 0) return diff == 0;
    // INT_MIN % -1 is undefined; unsigned maximum is not a negative step.
    if (negative_one(step, std::is_signed<T>())) return true;
    return diff % step == 0;
}
} // namespace detail

template<typename T>
inline bool is_approx_equal(T a, T b, double epsilon = default_epsilon) {
    static_assert(std::is_arithmetic<T>::value, "arithmetic values required");
    validate_epsilon(epsilon);
    return detail::approx_equal(a, b, epsilon, std::is_floating_point<T>());
}
/// Absolute residual tolerance; small nonzero steps remain nonzero.
template<typename T>
inline bool is_aligned(T diff, T step, double epsilon = default_epsilon) {
    static_assert(std::is_arithmetic<T>::value && !std::is_same<T, bool>::value,
                  "non-boolean arithmetic values required");
    validate_epsilon(epsilon);
    return detail::aligned(diff, step, epsilon, std::is_floating_point<T>());
}
} // namespace precision
} // namespace linscaler
#endif

