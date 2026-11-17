#ifndef LINSCALER_CORE_LINSP_HPP
#define LINSCALER_CORE_LINSP_HPP
#include "../basic/integer.hpp"
#include "../basic/precision.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace linscaler {
namespace linsp {
namespace detail {
template<typename T>
bool finite_coordinate(T value, std::true_type) { return std::isfinite(value); }
template<typename T>
bool finite_coordinate(T, std::false_type) { return true; }

template<typename T>
T from_integer(std::int64_t value) {
    if (value < static_cast<std::int64_t>((std::numeric_limits<T>::lowest)()) ||
        value > static_cast<std::int64_t>((std::numeric_limits<T>::max)()))
        throw std::overflow_error("value outside coordinate type");
    return static_cast<T>(value);
}
template<typename T>
T from_real(long double value) {
    if (!std::isfinite(value) ||
        value < static_cast<long double>((std::numeric_limits<T>::lowest)()) ||
        value > static_cast<long double>((std::numeric_limits<T>::max)()))
        throw std::overflow_error("non-finite or unrepresentable coordinate");
    return static_cast<T>(value);
}
inline std::uint64_t magnitude(std::int64_t value) {
    return value < 0 ? std::uint64_t(0) - static_cast<std::uint64_t>(value)
                     : static_cast<std::uint64_t>(value);
}
} // namespace detail

/// Finite arithmetic progression. Storage is O(1); coordinates are computed on demand.
/// Integer coordinates use checked int64_t intermediates (see docs/semantics.md).
template<typename T>
class linsp_ {
    static_assert(std::is_arithmetic<T>::value && !std::is_same<T, bool>::value,
                  "a non-boolean arithmetic coordinate type is required");
    static_assert(!std::is_integral<T>::value || std::numeric_limits<T>::digits <= 63,
                  "integral coordinates must fit int64_t; uint64_t is not supported");
public:
    class const_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = T;
        using difference_type = std::ptrdiff_t;
        using pointer = void;
        using reference = T;
        const_iterator() = default;
        T operator*() const {
            if (!owner_) throw std::out_of_range("dereferencing a singular iterator");
            return (*owner_)[static_cast<std::size_t>(index_)];
        }
        T operator[](difference_type n) const { return *(*this + n); }
        const_iterator& operator++() { return *this += 1; }
        const_iterator operator++(int) { auto old = *this; ++*this; return old; }
        const_iterator& operator--() { return *this -= 1; }
        const_iterator operator--(int) { auto old = *this; --*this; return old; }
        const_iterator& operator+=(difference_type n) {
            const auto next = integer::add(index_, n);
            if (!owner_ || next < 0 || static_cast<std::uint64_t>(next) > owner_->count())
                throw std::out_of_range("iterator outside progression");
            index_ = static_cast<difference_type>(next);
            return *this;
        }
        const_iterator& operator-=(difference_type n) {
            if (n == (std::numeric_limits<difference_type>::min)())
                throw std::out_of_range("iterator displacement overflow");
            return *this += -n;
        }
        friend const_iterator operator+(const_iterator it, difference_type n) { return it += n; }
        friend const_iterator operator+(difference_type n, const_iterator it) { return it += n; }
        friend const_iterator operator-(const_iterator it, difference_type n) { return it -= n; }
        friend difference_type operator-(const const_iterator& a, const const_iterator& b) {
            a.require_same_owner(b);
            return a.index_ - b.index_;
        }
        friend bool operator==(const const_iterator& a, const const_iterator& b) {
            return a.owner_ == b.owner_ && a.index_ == b.index_;
        }
        friend bool operator!=(const const_iterator& a, const const_iterator& b) { return !(a == b); }
        friend bool operator<(const const_iterator& a, const const_iterator& b) {
            a.require_same_owner(b);
            return a.index_ < b.index_;
        }
        friend bool operator>(const const_iterator& a, const const_iterator& b) { return b < a; }
        friend bool operator<=(const const_iterator& a, const const_iterator& b) { return !(b < a); }
        friend bool operator>=(const const_iterator& a, const const_iterator& b) { return !(a < b); }
    private:
        friend class linsp_;
        const_iterator(const linsp_* owner, std::ptrdiff_t index) : owner_(owner), index_(index) {}
        void require_same_owner(const const_iterator& other) const {
            if (owner_ != other.owner_) throw std::invalid_argument("unrelated iterators");
        }
        const linsp_* owner_ = nullptr;
        difference_type index_ = 0;
    };
    using iterator = const_iterator;

    linsp_() = default;
    linsp_(T first, T step, std::size_t count) : first_(first), step_(step), count_(count) {
        if (count == 0) throw std::invalid_argument("count must be positive; use default for empty");
        if (count > static_cast<std::size_t>((std::numeric_limits<std::ptrdiff_t>::max)()))
            throw std::length_error("count exceeds iterator difference range");
        if (!detail::finite_coordinate(first, std::is_floating_point<T>()) ||
            !detail::finite_coordinate(step, std::is_floating_point<T>()))
            throw std::invalid_argument("coordinates must be finite");
        (void)value_at(count - 1); // Validate endpoint and all linear intermediates.
    }
    T first() const { return first_; }
    T step() const { return step_; }
    std::size_t count() const { return count_; }
    bool empty() const { return count_ == 0; }
    bool constant() const { return step_ == T(0); }
    T last() const {
        if (empty()) throw std::out_of_range("empty progression has no last element");
        return value_at(count_ - 1);
    }
    T operator[](std::size_t index) const {
        if (index >= count_) throw std::out_of_range("index outside progression");
        return value_at(index);
    }
    bool parallel_to(const linsp_& other) const {
        return precision::is_approx_equal(step_, other.step_);
    }
    bool aligned_with(const linsp_& other) const {
        if (!parallel_to(other)) return false;
        if (constant() || other.constant())
            return constant() && other.constant() &&
                   precision::is_approx_equal(first_, other.first_);
        return aligned_with_impl(other, std::is_integral<T>());
    }
    bool contains(T value, double epsilon = precision::default_epsilon) const {
        precision::validate_epsilon(epsilon);
        if (empty()) return false;
        return contains_impl(value, epsilon, std::is_integral<T>());
    }
    /// Nearest grid index, clamped to the finite progression. Ties choose the higher index.
    /// Empty and constant progressions return zero (legacy convention).
    std::size_t nearest(T value) const {
        if (!detail::finite_coordinate(value, std::is_floating_point<T>()))
            throw std::invalid_argument("query must be finite");
        if (empty() || count_ == 1 || constant()) return 0;
        if ((step_ > 0 && value <= first_) || (step_ < 0 && value >= first_)) return 0;
        if ((step_ > 0 && value >= last()) || (step_ < 0 && value <= last())) return count_ - 1;
        return nearest_impl(value, std::is_integral<T>());
    }
    linsp_ subset(std::size_t start, std::size_t count) const {
        if (start >= count_ || count > count_ - start)
            throw std::out_of_range("subset outside progression");
        if (count == 0) throw std::invalid_argument("subset count must be positive");
        return linsp_((*this)[start], step_, count);
    }
    std::vector<T> to_vector() const { return std::vector<T>(begin(), end()); }
    const_iterator begin() const { return const_iterator(this, 0); }
    const_iterator end() const { return const_iterator(this, static_cast<std::ptrdiff_t>(count_)); }

private:
    // Tag dispatch instantiates only the arithmetic appropriate for T in C++11.
    bool aligned_with_impl(const linsp_& other, std::true_type) const {
        return precision::is_aligned(integer::subtract(first_, other.first_),
                                     static_cast<std::int64_t>(step_));
    }
    bool aligned_with_impl(const linsp_& other, std::false_type) const {
        const auto tolerance = (std::min)(static_cast<long double>(precision::default_epsilon),
                                          std::abs(static_cast<long double>(step_)) / 4);
        return precision::is_aligned(static_cast<long double>(first_) - other.first_,
                                     static_cast<long double>(step_), static_cast<double>(tolerance));
    }
    bool contains_impl(T value, double, std::true_type) const {
        if (value < (std::min)(first_, last()) || value > (std::max)(first_, last())) return false;
        if (constant()) return value == first_;
        return precision::is_aligned(integer::subtract(value, first_),
                                     static_cast<std::int64_t>(step_));
    }
    bool contains_impl(T value, double epsilon, std::false_type) const {
        if (!std::isfinite(value)) return false;
        if (constant()) return precision::is_approx_equal(value, first_, epsilon);
        const auto tolerance = (std::min)(static_cast<long double>(epsilon),
                                          std::abs(static_cast<long double>(step_)) / 4);
        const auto raw = (static_cast<long double>(value) - first_) / step_;
        const auto index = std::round(raw);
        if (!std::isfinite(index) || index < 0 || index >= static_cast<long double>(count_))
            return false;
        const auto expected = static_cast<long double>(first_) + index * step_;
        return std::abs(static_cast<long double>(value) - expected) <= tolerance;
    }
    std::size_t nearest_impl(T value, std::true_type) const {
        const auto distance = detail::magnitude(integer::subtract(value, first_));
        const auto stride = detail::magnitude(step_);
        const auto q = distance / stride;
        const auto r = distance % stride;
        return static_cast<std::size_t>(q + (r >= stride - r ? 1 : 0));
    }
    std::size_t nearest_impl(T value, std::false_type) const {
        const auto raw = (static_cast<long double>(value) - first_) / step_;
        const auto index = std::floor(raw + 0.5L);
        if (index >= static_cast<long double>(count_ - 1)) return count_ - 1;
        return static_cast<std::size_t>(index);
    }
    T value_at(std::size_t index) const {
        return value_at_impl(index, std::is_integral<T>());
    }
    T value_at_impl(std::size_t index, std::true_type) const {
        return detail::from_integer<T>(integer::add(first_,
            integer::multiply(step_, static_cast<std::int64_t>(index))));
    }
    T value_at_impl(std::size_t index, std::false_type) const {
        return detail::from_real<T>(static_cast<long double>(first_) +
                                    static_cast<long double>(index) * step_);
    }
    T first_ = 0;
    T step_ = 0;
    std::size_t count_ = 0;
};

/// Structural, exact equality. Use explicit approximate queries for floating point.
template<typename T>
bool operator==(const linsp_<T>& a, const linsp_<T>& b) {
    return a.count() == b.count() && a.first() == b.first() && a.step() == b.step();
}
template<typename T>
bool operator!=(const linsp_<T>& a, const linsp_<T>& b) { return !(a == b); }
} // namespace linsp
} // namespace linscaler
#endif

