#ifndef LINSCALER_CORE_LINSP_OPS_HPP
#define LINSCALER_CORE_LINSP_OPS_HPP
#include "linsp.hpp"
#include <numeric>
#include <optional>
#include <utility>

namespace linscaler {
namespace linsp {
namespace detail {
template<typename T>
linsp_<T> ascending_set(const linsp_<T>& sequence) {
    if (sequence.empty()) return {};
    if (sequence.count() == 1 || sequence.constant())
        return linsp_<T>(sequence.first(), T(0), 1);
    if (sequence.step() >= 0) return sequence;
    if constexpr (std::is_integral<T>::value)
        return linsp_<T>(sequence.last(), from_integer<T>(integer::subtract(0, sequence.step())), sequence.count());
    else
        return linsp_<T>(sequence.last(), -sequence.step(), sequence.count());
}

// Inputs are positive. All coefficient arithmetic is checked.
inline std::pair<std::int64_t, std::int64_t> gcd_coefficient(std::int64_t a, std::int64_t b) {
    std::int64_t x = 1, next_x = 0;
    while (b != 0) {
        const auto q = a / b;
        const auto r = a % b;
        a = b; b = r;
        const auto replacement = integer::subtract(x, integer::multiply(q, next_x));
        x = next_x; next_x = replacement;
    }
    return {a, x};
}
template<typename T>
linsp_<T> integral_intersection(const linsp_<T>& a, const linsp_<T>& b) {
    const auto low = (std::max)(a.first(), b.first());
    const auto high = (std::min)(a.last(), b.last());
    if (low > high) return {};
    if (a.count() == 1) return b.contains(a.first()) ? a : linsp_<T>{};
    if (b.count() == 1) return a.contains(b.first()) ? b : linsp_<T>{};
    const std::int64_t sa = a.step(), sb = b.step();
    const auto coefficient = gcd_coefficient(sa, sb);
    const auto g = coefficient.first;
    const auto delta = integer::subtract(b.first(), a.first());
    if (delta % g != 0) return {};
    const auto modulus = sb / g;
    const auto product = integer::multiply(delta / g, coefficient.second);
    auto k = product % modulus;
    if (k < 0) k += modulus;
    auto first = integer::add(a.first(), integer::multiply(sa, k));
    if (first > high) return {};
    const auto stride = integer::multiply(sa / g, sb);
    if (first < low) {
        const auto jump = integer::ceil_div(integer::subtract(low, first), stride);
        first = integer::add(first, integer::multiply(jump, stride));
    }
    if (first > high) return {};
    const auto count = integer::subtract(high, first) / stride + 1;
    return linsp_<T>(from_integer<T>(first),
                    count == 1 ? T(0) : from_integer<T>(stride),
                    static_cast<std::size_t>(count));
}

/// Constant-memory builder used for approximate floating-point set operations.
template<typename T>
class progression_builder {
public:
    explicit progression_builder(long double epsilon) : epsilon_(epsilon) {}
    bool append(T value) {
        if (count_ == 0) { first_ = previous_ = value; count_ = 1; return true; }
        // A nonzero floating step can still round several samples to one value.
        if (std::abs(static_cast<long double>(value) - previous_) <= epsilon_) return true;
        if (count_ == 1) {
            step_ = from_real<T>(static_cast<long double>(value) - first_);
        } else {
            const auto expected = static_cast<long double>(first_) +
                                  static_cast<long double>(count_) * step_;
            if (std::abs(static_cast<long double>(value) - expected) > epsilon_) return false;
        }
        if (count_ == static_cast<std::size_t>((std::numeric_limits<std::ptrdiff_t>::max)()))
            throw std::length_error("set size exceeds iterator range");
        ++count_;
        previous_ = value;
        return true;
    }
    linsp_<T> result() const {
        return count_ == 0 ? linsp_<T>{} : linsp_<T>(first_, step_, count_);
    }
private:
    T first_ = 0, step_ = 0, previous_ = 0;
    std::size_t count_ = 0;
    long double epsilon_;
};

template<typename T>
std::optional<linsp_<T>> floating_set_operation(const linsp_<T>& a, const linsp_<T>& b, bool intersection) {
    long double epsilon = precision::default_epsilon;
    if (a.count() > 1) epsilon = (std::min)(epsilon, static_cast<long double>(a.step()) / 4);
    if (b.count() > 1) epsilon = (std::min)(epsilon, static_cast<long double>(b.step()) / 4);
    progression_builder<T> result(epsilon);
    std::size_t i = 0, j = 0;
    while (i < a.count() || j < b.count()) {
        if (i == a.count()) {
            if (intersection) break;
            if (!result.append(b[j++])) return std::nullopt;
        } else if (j == b.count()) {
            if (intersection) break;
            if (!result.append(a[i++])) return std::nullopt;
        } else {
            const auto av = a[i], bv = b[j];
            if (std::abs(static_cast<long double>(av) - bv) <= epsilon) {
                // Symmetric representative, independent of argument order.
                if (!result.append((std::min)(av, bv))) return std::nullopt;
                ++i; ++j;
            } else if (av < bv) {
                if (!intersection && !result.append(av)) return std::nullopt;
                ++i;
            } else {
                if (!intersection && !result.append(bv)) return std::nullopt;
                ++j;
            }
        }
    }
    return result.result();
}
} // namespace detail

/// Engaged empty result means empty intersection; nullopt means not representable.
/// Set results are ascending and contain no duplicates.
template<typename T>
std::optional<linsp_<T>> try_intersect(const linsp_<T>& a, const linsp_<T>& b) {
    if (a.empty() || b.empty()) return linsp_<T>{};
    const auto an = detail::ascending_set(a), bn = detail::ascending_set(b);
    if constexpr (std::is_integral<T>::value)
        return detail::integral_intersection(an, bn);
    else
        return detail::floating_set_operation(an, bn, true);
}
template<typename T>
linsp_<T> intersect(const linsp_<T>& a, const linsp_<T>& b) {
    auto result = try_intersect(a, b);
    if (!result) throw std::domain_error("intersection is not a single progression");
    return *result;
}

/// Never fills gaps. nullopt distinguishes an unrepresentable union from an empty union.
template<typename T>
std::optional<linsp_<T>> try_unite(const linsp_<T>& a, const linsp_<T>& b) {
    const auto an = detail::ascending_set(a), bn = detail::ascending_set(b);
    if constexpr (std::is_integral<T>::value) {
        if (an.empty()) return bn;
        if (bn.empty()) return an;
        const auto low = (std::min)(an.first(), bn.first());
        const auto high = (std::max)(an.last(), bn.last());
        const auto delta = integer::subtract((std::max)(an.first(), bn.first()), low);
        const auto stride = std::gcd(std::gcd(static_cast<std::int64_t>(an.step()),
                                              static_cast<std::int64_t>(bn.step())), delta);
        if (stride == 0) return linsp_<T>(low, T(0), 1);
        const auto span = integer::subtract(high, low);
        const auto expected = integer::add(span / stride, 1);
        const auto overlap = detail::integral_intersection(an, bn);
        const auto actual = integer::add(static_cast<std::int64_t>(an.count()),
            integer::subtract(static_cast<std::int64_t>(bn.count()),
                              static_cast<std::int64_t>(overlap.count())));
        if (actual != expected) return std::nullopt;
        return linsp_<T>(low, detail::from_integer<T>(stride), static_cast<std::size_t>(expected));
    } else {
        return detail::floating_set_operation(an, bn, false);
    }
}
template<typename T>
linsp_<T> unite(const linsp_<T>& a, const linsp_<T>& b) {
    auto result = try_unite(a, b);
    if (!result) throw std::domain_error("union is not a single progression; use try_unite");
    return *result;
}
} // namespace linsp
} // namespace linscaler
#endif

