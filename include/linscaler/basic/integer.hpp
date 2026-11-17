#ifndef LINSCALER_BASIC_INTEGER_HPP
#define LINSCALER_BASIC_INTEGER_HPP
#include <cstdint>
#include <limits>
#include <stdexcept>
namespace linscaler {
namespace integer {
using value_type = std::int64_t;
inline value_type add(value_type a, value_type b) {
    constexpr auto lo = (std::numeric_limits<value_type>::min)();
    constexpr auto hi = (std::numeric_limits<value_type>::max)();
    if ((b > 0 && a > hi - b) || (b < 0 && a < lo - b))
        throw std::overflow_error("integer addition overflow");
    return a + b;
}
inline value_type subtract(value_type a, value_type b) {
    constexpr auto lo = (std::numeric_limits<value_type>::min)();
    constexpr auto hi = (std::numeric_limits<value_type>::max)();
    if ((b > 0 && a < lo + b) || (b < 0 && a > hi + b))
        throw std::overflow_error("integer subtraction overflow");
    return a - b;
}
inline value_type multiply(value_type a, value_type b) {
    constexpr auto lo = (std::numeric_limits<value_type>::min)();
    constexpr auto hi = (std::numeric_limits<value_type>::max)();
    if (a == 0 || b == 0) return 0;
    if ((a > 0 && b > 0 && a > hi / b) ||
        (a > 0 && b < 0 && b < lo / a) ||
        (a < 0 && b > 0 && a < lo / b) ||
        (a < 0 && b < 0 && a < hi / b))
        throw std::overflow_error("integer multiplication overflow");
    return a * b;
}
inline void check_division(value_type a, value_type b) {
    if (b == 0) throw std::invalid_argument("zero divisor");
    if (a == (std::numeric_limits<value_type>::min)() && b == -1)
        throw std::overflow_error("integer division overflow");
}
/// Mathematical floor, including negative operands, without negating INT64_MIN.
inline value_type floor_div(value_type a, value_type b) {
    check_division(a, b);
    const auto q = a / b;
    const auto r = a % b;
    return q - ((r != 0 && (r < 0) != (b < 0)) ? 1 : 0);
}
inline value_type ceil_div(value_type a, value_type b) {
    check_division(a, b);
    const auto q = a / b;
    const auto r = a % b;
    return q + ((r != 0 && (r < 0) == (b < 0)) ? 1 : 0);
}
} // namespace integer
} // namespace linscaler
#endif

