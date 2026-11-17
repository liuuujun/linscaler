#include "test_support.hpp"
#include <linscaler/transform/floating_affine_map.hpp>
#include <linscaler/transform/grid_map.hpp>
#include <linscaler/transform/mapper.hpp>
#include <linscaler/transform/rational_affine_map.hpp>
#include <linscaler/transform/rational_index_map.hpp>
#include <cmath>
#include <cstdint>
#include <limits>
#include <type_traits>

using linscaler::linsp::linsp_;
using namespace linscaler::transform;

// Select optional floating capabilities without a constant runtime condition
// (MSVC C4127), while retaining the strict C++11 test entry point.
template<typename Real>
void check_floating_subnormals(std::false_type) {}

template<typename Real>
void check_floating_subnormals(std::true_type) {
    typedef floating_affine_map<Real> map_type;
    const Real subnormal = std::numeric_limits<Real>::denorm_min();
    const map_type identity(Real(1), Real(0));
    CHECK(map_type(subnormal, Real(0))(Real(1)) == subnormal);
    CHECK(compose(map_type(subnormal, Real(0)), identity).scale() == subnormal);
    CHECK_THROWS(std::overflow_error, inverse(map_type(subnormal, Real(0))));
    CHECK_THROWS(std::underflow_error, compose(map_type(subnormal, Real(0)),
                                              map_type(Real(0.25), Real(0))));
}

template<typename Real>
void check_floating_type() {
    typedef floating_affine_map<Real> map_type;
    static_assert(std::is_same<decltype(map_type(Real(1), Real(0)).evaluate(Real(0))),
                               Real>::value, "Evaluation preserves the selected floating type");

    const map_type identity(Real(1), Real(0));
    const map_type forward(Real(1.5), Real(1.5));
    const Real inputs[] = {Real(-2), Real(-0.5), Real(0), Real(1), Real(2), Real(9)};
    const Real expected[] = {Real(-1.5), Real(0.75), Real(1.5), Real(3), Real(4.5), Real(15)};
    CHECK(forward.scale() == Real(1.5));
    CHECK(forward.offset() == Real(1.5));
    for (std::size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); ++i) {
        CHECK(forward.evaluate(inputs[i]) == expected[i]);
        CHECK(forward(inputs[i]) == expected[i]);
        CHECK(identity(inputs[i]) == inputs[i]);
    }
    CHECK(map_type(Real(-2), Real(3))(Real(4)) == Real(-5));
    const map_type constant(Real(0), Real(7));
    CHECK(constant(Real(-1000)) == Real(7));
    CHECK(constant(Real(1000)) == Real(7));

    // Known, noncommuting transformations establish the public compose order.
    const map_type first(Real(2), Real(3));
    const map_type second(Real(-4), Real(5));
    const auto combined = compose(first, second);
    CHECK(combined.scale() == Real(-8));
    CHECK(combined.offset() == Real(-7));
    CHECK(combined(Real(2)) == Real(-23));
    CHECK(compose(second, first).offset() == Real(13));
    CHECK(compose(first, constant)(Real(2)) == Real(7));
    CHECK(compose(constant, first)(Real(2)) == Real(17));
    CHECK(compose(identity, forward)(Real(2)) == Real(4.5));
    CHECK(compose(forward, identity)(Real(2)) == Real(4.5));

    // Hand-solved inverse, not merely inverse/compose checking each other.
    const auto reversed = inverse(map_type(Real(2), Real(-6)));
    CHECK(reversed.scale() == Real(0.5));
    CHECK(reversed.offset() == Real(3));
    CHECK(reversed(Real(-8)) == Real(-1));
    CHECK(reversed(Real(0)) == Real(3));
    CHECK(reversed(Real(10)) == Real(8));
    const auto negative_inverse = inverse(map_type(Real(-4), Real(8)));
    CHECK(negative_inverse.scale() == Real(-0.25));
    CHECK(negative_inverse.offset() == Real(2));
    CHECK_THROWS(std::invalid_argument, inverse(constant));
    CHECK_THROWS(std::invalid_argument, inverse(map_type(-Real(0), Real(0))));

    // Small integer arithmetic supplies an independent exact dyadic oracle.
    // No tested mapping or inverse is used to calculate expected values.
    for (int a = -3; a <= 3; ++a)
        for (int b = -3; b <= 3; ++b) {
            const map_type mapping(Real(a) / Real(4), Real(b) / Real(4));
            for (int k = -8; k <= 8; ++k) {
                const int numerator = a * k + 2 * b;
                CHECK(mapping(Real(k) / Real(2)) == Real(numerator) / Real(8));
            }
            for (int c = -2; c <= 2; ++c)
                for (int d = -2; d <= 2; ++d) {
                    const auto chain = compose(map_type(Real(a) / Real(2), Real(b) / Real(2)),
                                               map_type(Real(c) / Real(2), Real(d) / Real(2)));
                    for (int k = -4; k <= 4; ++k) {
                        const int numerator = a * c * k + 2 * c * b + 4 * d;
                        CHECK(chain(Real(k) / Real(2)) == Real(numerator) / Real(8));
                    }
                }
        }

    const Real infinity = std::numeric_limits<Real>::infinity();
    const Real nan = std::numeric_limits<Real>::quiet_NaN();
    CHECK_THROWS(std::invalid_argument, map_type(infinity, Real(0)));
    CHECK_THROWS(std::invalid_argument, map_type(Real(1), -infinity));
    CHECK_THROWS(std::invalid_argument, map_type(nan, Real(0)));
    CHECK_THROWS(std::invalid_argument, map_type(Real(1), nan));
    CHECK_THROWS(std::invalid_argument, forward.evaluate(infinity));
    CHECK_THROWS(std::invalid_argument, forward(-infinity));
    CHECK_THROWS(std::invalid_argument, constant(nan));
    CHECK_THROWS(std::invalid_argument, constant(infinity));

    const Real largest = (std::numeric_limits<Real>::max)();
    const Real smallest_normal = (std::numeric_limits<Real>::min)();
    CHECK_THROWS(std::overflow_error, map_type(largest, Real(0))(Real(2)));
    CHECK_THROWS(std::overflow_error, compose(map_type(largest, Real(0)),
                                             map_type(Real(2), Real(0))));
    CHECK_THROWS(std::overflow_error, compose(map_type(Real(1), largest),
                                             map_type(Real(1), largest)));
    CHECK_THROWS(std::overflow_error, inverse(map_type(Real(0.5), largest)));
    CHECK_THROWS(std::underflow_error, compose(map_type(smallest_normal, Real(0)),
                                              map_type(smallest_normal, Real(0))));

    // FMA has a finite result although a separately rounded product overflows.
    CHECK(map_type(largest, -largest)(Real(2)) == largest);
    const auto rescued = compose(map_type(Real(1), largest), map_type(Real(2), -largest));
    CHECK(rescued.scale() == Real(2));
    CHECK(rescued.offset() == largest);

    // A representable product residual distinguishes fused from two-step evaluation.
    const Real epsilon = std::numeric_limits<Real>::epsilon();
    const Real residual = -(epsilon * epsilon);
    CHECK(map_type(Real(1) + epsilon, Real(-1))(Real(1) - epsilon) == residual);
    const auto cancellation = compose(map_type(Real(1), Real(1) - epsilon),
                                      map_type(Real(1) + epsilon, Real(-1)));
    CHECK(cancellation.offset() == residual);

    // An intentional constant and ordinary result/offset underflow remain valid.
    CHECK(map_type(smallest_normal, Real(0))(smallest_normal) == Real(0));
    const auto underflowed_offset = compose(map_type(Real(0), smallest_normal),
                                           map_type(smallest_normal, Real(0)));
    CHECK(underflowed_offset.scale() == Real(0));
    CHECK(underflowed_offset.offset() == Real(0));
    const auto inverse_offset = inverse(map_type(largest, smallest_normal));
    CHECK(inverse_offset.offset() == Real(0));

    const Real tiny = std::ldexp(Real(1), -30);
    CHECK(inverse(map_type(tiny, Real(0))).scale() == std::ldexp(Real(1), 30));
    check_floating_subnormals<Real>(std::integral_constant<bool,
        std::numeric_limits<Real>::has_denorm == std::denorm_present>());
}

void check_float_grid_subnormals(std::false_type) {}

void check_float_grid_subnormals(std::true_type) {
    const double subnormal = static_cast<double>(std::numeric_limits<float>::denorm_min());
    const auto retained = compile_grid_map<float>(linsp_<double>(0, subnormal, 1),
                                                  linsp_<double>(0, 1, 1));
    CHECK(retained.scale() == std::numeric_limits<float>::denorm_min());
    CHECK_THROWS(std::underflow_error,
                 compile_grid_map<float>(linsp_<double>(0, subnormal / 4, 1),
                                         linsp_<double>(0, 1, 1)));
    const auto lost_offset = compile_grid_map<float>(linsp_<double>(subnormal / 4, 1, 1),
                                                     linsp_<double>(0, 1, 1));
    CHECK(lost_offset.scale() == 1 && lost_offset.offset() == 0);
}

void check_grid_compilation() {
    const linsp_<int> source(10, 6, 5);
    const linsp_<double> target(4, 4, 10);
    static_assert(std::is_same<decltype(compile_grid_map(source, target)),
                               floating_affine_map<long double>>::value,
                  "The default coefficient type is long double");
    const auto mapping = compile_grid_map(source, target);
    const auto as_float = compile_grid_map<float>(source, target);
    const auto as_double = compile_grid_map<double>(source, target);
    CHECK(mapping.scale() == 1.5L && mapping.offset() == 1.5L);
    const long double expected[] = {1.5L, 3, 4.5L, 6, 7.5L};
    for (int i = 0; i < 5; ++i) {
        CHECK(mapping(static_cast<long double>(i)) == expected[i]);
        CHECK(as_float(static_cast<float>(i)) == static_cast<float>(expected[i]));
        CHECK(as_double(static_cast<double>(i)) == static_cast<double>(expected[i]));
    }
    CHECK(mapping(-2) == -1.5L);
    CHECK(mapping(99) == 150);
    CHECK(mapping(0.5L) == 2.25L);

    const auto reverse = compile_grid_map(linsp_<int>(4, 2, 4), linsp_<double>(10, -4, 3));
    const long double inputs[] = {-8, -0.5L, 0, 1, 3, 12};
    const long double reversed[] = {5.5L, 1.75L, 1.5L, 1, 0, -4.5L};
    for (std::size_t i = 0; i < sizeof(inputs) / sizeof(inputs[0]); ++i)
        CHECK(reverse(inputs[i]) == reversed[i]);
    const auto unsigned_source = compile_grid_map(linsp_<unsigned>(10, 2, 3),
                                                  linsp_<int>(20, 4, 3));
    CHECK(unsigned_source.scale() == 0.5L && unsigned_source.offset() == -2.5L);
    const auto singleton = compile_grid_map(linsp_<int>(10, 6, 1), linsp_<int>(4, 4, 1));
    CHECK(singleton.scale() == 1.5L && singleton(2) == 4.5L);
    const auto constant = compile_grid_map(linsp_<int>(3, 0, 4), linsp_<double>(1, 2, 3));
    CHECK(constant.scale() == 0 && constant.offset() == 1);
    CHECK(constant(-100) == 1 && constant(100) == 1);
    CHECK_THROWS(std::invalid_argument, compile_grid_map(linsp_<int>{}, target));
    CHECK_THROWS(std::invalid_argument, compile_grid_map(source, linsp_<double>{}));
    CHECK_THROWS(std::invalid_argument, compile_grid_map(source, linsp_<int>(1, 0, 3)));
    CHECK_THROWS(std::invalid_argument,
                 compile_grid_map(linsp_<int>(1, 0, 3), linsp_<int>(1, 0, 3)));

    // Fixed-point grid parameters permit independent integer reference answers.
    for (int source_origin = -4; source_origin <= 4; ++source_origin)
        for (int target_origin = -4; target_origin <= 4; ++target_origin)
            for (int source_stride = -3; source_stride <= 3; ++source_stride) {
                const linsp_<int> first(source_origin, source_stride, 3);
                const linsp_<int> second(target_origin, 4, 2);
                const auto compiled = compile_grid_map<double>(first, second);
                for (int i = -5; i <= 5; ++i) {
                    const int numerator = source_origin + source_stride * i - target_origin;
                    CHECK(compiled(static_cast<double>(i)) == static_cast<double>(numerator) / 4.0);
                }
            }

    // Coefficient compilation preserves the step ratio even when evaluating two
    // adjacent coordinates loses their difference. This intentionally differs
    // from the old coordinate-then-inverse evaluation path.
    const long double large_origin = std::ldexp(1.0L, std::numeric_limits<long double>::digits);
    const linsp_<long double> large_grid(large_origin, 1, 2);
    const auto compiled_identity = compile_grid_map(large_grid, large_grid);
    CHECK(compiled_identity.scale() == 1 && compiled_identity.offset() == 0);
    CHECK(compiled_identity(1) == 1);
    CHECK(map_index_between(large_grid, large_grid, 1) == 0);
    const auto large_offset = compile_grid_map(large_grid, linsp_<long double>(0, 1, 2));
    CHECK(large_offset.scale() == 1 && large_offset.offset() == large_origin);

    const double larger_than_float = static_cast<double>((std::numeric_limits<float>::max)()) * 2.0;
    CHECK_THROWS(std::overflow_error,
                 compile_grid_map<float>(linsp_<double>(0, larger_than_float, 1),
                                         linsp_<double>(0, 1, 1)));
    CHECK_THROWS(std::overflow_error,
                 compile_grid_map<float>(linsp_<double>(larger_than_float, 1, 1),
                                         linsp_<double>(0, 1, 1)));
    check_float_grid_subnormals(std::integral_constant<bool,
        std::numeric_limits<float>::has_denorm == std::denorm_present>());
    const long double maximum = (std::numeric_limits<long double>::max)();
    const long double minimum = (std::numeric_limits<long double>::min)();
    CHECK_THROWS(std::overflow_error,
                 compile_grid_map(linsp_<long double>(0, maximum, 1),
                                  linsp_<long double>(0, minimum, 1)));
    CHECK_THROWS(std::underflow_error,
                 compile_grid_map(linsp_<long double>(0, minimum, 1),
                                  linsp_<long double>(0, maximum, 1)));
    CHECK_THROWS(std::overflow_error,
                 compile_grid_map(linsp_<long double>(maximum, 0, 1),
                                  linsp_<long double>(0, minimum, 1)));
    // The bounded intermediate difference can overflow even though a differently
    // rearranged mathematical quotient would be representable.
    CHECK_THROWS(std::overflow_error,
                 compile_grid_map(linsp_<long double>(maximum, 1, 1),
                                  linsp_<long double>(-maximum, maximum, 1)));
}

void check_rational_compatibility() {
    static_assert(std::is_same<rational_affine_map, rational_index_map>::value,
                  "The old public name remains an alias of the same type");
    static_assert(std::is_same<decltype(rational_index_map(1, 1).shifted_input(0)),
                               rational_affine_map>::value,
                  "Block maps use the canonical type through either name");
    const rational_affine_map unreduced(6, 4, 6);
    CHECK(unreduced.numerator() == 6 && unreduced.denominator() == 4 && unreduced.offset() == 6);
    const rational_affine_map mapping(3, 2, 3);
    const std::int64_t indices[] = {-2, -1, 0, 1, 2};
    const std::int64_t floors[] = {-2, 0, 1, 3, 4};
    const std::int64_t remainders[] = {1, 0, 1, 0, 1};
    const std::int64_t nearest[] = {-1, 0, 2, 3, 5};
    for (std::size_t i = 0; i < sizeof(indices) / sizeof(indices[0]); ++i) {
        const auto result = mapping(indices[i]);
        CHECK(result.floor == floors[i] && result.remainder == remainders[i]);
        CHECK(result.denominator == 2 && result.nearest() == nearest[i]);
        CHECK(result.ceil() == nearest[i]);
    }
    const auto block = mapping.shifted_input(2);
    CHECK(block.numerator() == 3 && block.denominator() == 2 && block.offset() == 9);
    CHECK(block(0).floor == 4 && block(0).remainder == 1);
    CHECK_THROWS(std::invalid_argument, rational_affine_map(1, 0));
    CHECK_THROWS(std::invalid_argument, rational_affine_map(1, -1));
    const std::int64_t minimum = (std::numeric_limits<std::int64_t>::min)();
    const std::int64_t maximum = (std::numeric_limits<std::int64_t>::max)();
    CHECK(rational_affine_map(minimum, 1)(1).floor == minimum);
    CHECK(rational_affine_map(maximum, 1)(1).floor == maximum);
    CHECK_THROWS(std::overflow_error, rational_affine_map(minimum, 1)(-1));
    CHECK_THROWS(std::overflow_error, rational_affine_map(maximum, 1, 1)(1));
    CHECK_THROWS(std::overflow_error, rational_affine_map(2, 1).shifted_input(maximum));
    const auto constant = rational_affine_map(0, 7, -3)(minimum);
    CHECK(constant.floor == -1 && constant.remainder == 4 && constant.denominator == 7);
    const index_position beyond_integer = {maximum, 1, 2};
    CHECK_THROWS(std::overflow_error, beyond_integer.ceil());
    CHECK_THROWS(std::overflow_error, beyond_integer.nearest());
}

int main() {
    check_floating_type<float>();
    check_floating_type<double>();
    check_floating_type<long double>();
    check_grid_compilation();
    check_rational_compatibility();
}
