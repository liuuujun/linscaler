#ifndef LINSCALER_TRANSFORM_RATIONAL_AFFINE_MAP_HPP
#define LINSCALER_TRANSFORM_RATIONAL_AFFINE_MAP_HPP
#include "../basic/integer.hpp"
#include <cstdint>
#include <stdexcept>

namespace linscaler {
namespace transform {
/// Exact decomposition of a mapped index: floor + remainder / denominator.
struct index_position {
    std::int64_t floor;
    std::int64_t remainder;
    std::int64_t denominator;
    std::int64_t ceil() const { return integer::add(floor, remainder != 0 ? 1 : 0); }
    /// Ties choose the higher integer, including for negative coordinates.
    std::int64_t nearest() const {
        return integer::add(floor, remainder >= denominator - remainder ? 1 : 0);
    }
    long double value() const {
        return static_cast<long double>(floor) +
               static_cast<long double>(remainder) / denominator;
    }
};

/// Maps integer inputs by (numerator * index + offset) / denominator.
/// Every intermediate is checked int64_t arithmetic; denominator must be positive.
/// Coefficients are stored as supplied, without automatic reduction.
class rational_affine_map {
public:
    rational_affine_map(std::int64_t numerator, std::int64_t denominator, std::int64_t offset = 0)
        : numerator_(numerator), denominator_(denominator), offset_(offset) {
        if (denominator <= 0) throw std::invalid_argument("denominator must be positive");
    }
    index_position operator()(std::int64_t index) const {
        const auto value = integer::add(integer::multiply(numerator_, index), offset_);
        auto remainder = value % denominator_;
        if (remainder < 0) remainder += denominator_;
        return {integer::floor_div(value, denominator_), remainder, denominator_};
    }
    /// Convert local indices to global indices without resetting the sampling phase.
    rational_affine_map shifted_input(std::int64_t global_begin) const {
        return rational_affine_map(numerator_, denominator_,
            integer::add(integer::multiply(numerator_, global_begin), offset_));
    }
    std::int64_t numerator() const { return numerator_; }
    std::int64_t denominator() const { return denominator_; }
    std::int64_t offset() const { return offset_; }
private:
    std::int64_t numerator_, denominator_, offset_;
};
} // namespace transform
} // namespace linscaler
#endif
