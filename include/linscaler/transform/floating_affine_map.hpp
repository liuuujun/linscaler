#ifndef LINSCALER_TRANSFORM_FLOATING_AFFINE_MAP_HPP
#define LINSCALER_TRANSFORM_FLOATING_AFFINE_MAP_HPP

#include <cmath>
#include <stdexcept>
#include <type_traits>

namespace linscaler {
namespace transform {
namespace detail {

template<typename Real>
Real affine_finite_result(Real value) {
    if (!std::isfinite(value))
        throw std::overflow_error("floating affine result is not finite");
    return value;
}

template<typename Real>
Real affine_scale_result(Real value, bool nonzero_expected) {
    affine_finite_result(value);
    if (nonzero_expected && value == Real(0))
        throw std::underflow_error("nonzero affine scale rounded to zero");
    return value;
}

} // namespace detail

/// A floating representation of f(x) = scale*x + offset, independent of any grid.
/// Evaluation uses one std::fma in Real; it neither clips nor quantizes the result.
template<typename Real>
class floating_affine_map {
    static_assert(std::is_floating_point<Real>::value,
                  "floating_affine_map requires a floating-point coefficient type");
public:
    floating_affine_map(Real scale, Real offset) : scale_(scale), offset_(offset) {
        if (!std::isfinite(scale) || !std::isfinite(offset))
            throw std::invalid_argument("affine coefficients must be finite");
    }

    Real scale() const { return scale_; }
    Real offset() const { return offset_; }

    Real evaluate(Real input) const {
        if (!std::isfinite(input))
            throw std::invalid_argument("affine input must be finite");
        return detail::affine_finite_result(std::fma(scale_, input, offset_));
    }

    Real operator()(Real input) const { return evaluate(input); }

private:
    Real scale_;
    Real offset_;
};

/// Apply first, then second. Algebraic composition need not reproduce staged rounding.
template<typename Real>
floating_affine_map<Real> compose(const floating_affine_map<Real>& first,
                                 const floating_affine_map<Real>& second) {
    const Real scale = detail::affine_scale_result(
        Real(second.scale() * first.scale()),
        first.scale() != Real(0) && second.scale() != Real(0));
    const Real offset = detail::affine_finite_result(
        std::fma(second.scale(), first.offset(), second.offset()));
    return floating_affine_map<Real>(scale, offset);
}

/// Return the floating representation of the continuous inverse; no epsilon test.
template<typename Real>
floating_affine_map<Real> inverse(const floating_affine_map<Real>& map) {
    if (map.scale() == Real(0))
        throw std::invalid_argument("constant affine map has no unique inverse");
    const Real scale = detail::affine_scale_result(Real(Real(1) / map.scale()), true);
    const Real offset = detail::affine_finite_result(Real(-map.offset() / map.scale()));
    return floating_affine_map<Real>(scale, offset);
}

} // namespace transform
} // namespace linscaler
#endif
