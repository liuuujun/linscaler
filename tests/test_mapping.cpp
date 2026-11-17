#include "test_support.hpp"
#include <linscaler/transform/mapper.hpp>
#include <linscaler/transform/rational_index_map.hpp>
#include <cmath>
#include <cstdint>
#include <limits>
using linscaler::linsp::linsp_;
using namespace linscaler::transform;

template<typename T>
void check_mapping_type() {
    const linsp_<T> source(T(2), T(2), 4);
    const linsp_<T> target(T(4), T(4), 2);
    CHECK(coordinate_at(source, -0.5L) == 1.0L);
    CHECK(fractional_index(source, 5.0L) == 1.5L);
    CHECK(find_index(source, T(6)) == 2.0);
    CHECK(find_index(linsp_<T>(T(2), T(0), 3), T(8)) == 0.0);
    CHECK(map_index(target, 1.5) == T(10));
    CHECK(map_index(target, 1.125) == T(8.5)); // Only integer outputs truncate.
    CHECK(map_index_between(source, target, 1.5L) == 0.25L);
    CHECK(map_value(source, target, T(8)) == T(16)); // Index-preserving extrapolation.
    CHECK_THROWS(std::out_of_range, map_value(source, target, T(5)));
}

int main() {
    check_mapping_type<signed char>();
    check_mapping_type<short>();
    check_mapping_type<int>();
    check_mapping_type<std::int64_t>();
    check_mapping_type<std::uint32_t>();
    check_mapping_type<float>();
    check_mapping_type<double>();
    check_mapping_type<long double>();
    const linsp_<double> coarse(0, 1.0 / 300, 100);
    const linsp_<double> fine(0, 1.0 / 600, 200);
    CHECK(std::abs(map_index_between(coarse, fine, 3) - 6) < 1e-12L);
    CHECK(std::abs(map_index_between(fine, coarse, 3) - 1.5L) < 1e-12L);
    CHECK(std::abs(coordinate_at(coarse, fractional_index(coarse, 0.003L)) - 0.003L) < 1e-12L);
    CHECK_THROWS(std::invalid_argument, fractional_index(linsp_<double>(1, 0, 5), 1));
    CHECK_THROWS(std::invalid_argument, coordinate_at(linsp_<double>{}, 0));
    CHECK_THROWS(std::invalid_argument, coordinate_at(coarse, std::numeric_limits<long double>::infinity()));
    CHECK_THROWS(std::invalid_argument, find_index(linsp_<int>{}, 0));
    CHECK_THROWS(std::overflow_error, map_index(linsp_<int>(0, 1, 2), 1e30));
    CHECK(map_index(linsp_<int>(-10, 5, 3), 0.5) == -7); // Truncate complete coordinate.
    CHECK(fractional_index(linsp_<unsigned>(10, 2, 3), 0) == -5);
    CHECK_THROWS(std::overflow_error, map_index(linsp_<std::int64_t>(0, 1, 2), std::ldexp(1.0, 63)));
    CHECK(map_index(linsp_<std::int64_t>(0, 1, 2), -std::ldexp(1.0, 63)) ==
          (std::numeric_limits<std::int64_t>::min)());
    CHECK(map_index(linsp_<signed char>(0, 1, 2), 127.75) == 127);
    CHECK(map_index(linsp_<signed char>(0, 1, 2), -128.75) == -128);
    CHECK_THROWS(std::overflow_error, map_index(linsp_<signed char>(0, 1, 2), 128.0));
    CHECK_THROWS(std::overflow_error, map_index(linsp_<signed char>(0, 1, 2), -129.0));
    CHECK(map_index(linsp_<std::uint32_t>(0, 1, 2), -0.75) == 0);
    CHECK_THROWS(std::overflow_error, map_index(linsp_<std::uint32_t>(0, 1, 2), -1.0));
    CHECK_THROWS(std::overflow_error, map_index(linsp_<std::uint32_t>(0, 1, 2), std::ldexp(1.0, 32)));
    CHECK_THROWS(std::overflow_error, map_index(linsp_<float>(0, 1, 2),
                                               static_cast<double>((std::numeric_limits<float>::max)()) * 2));

    const double nan = std::numeric_limits<double>::quiet_NaN();
    const double infinity = std::numeric_limits<double>::infinity();
    const linsp_<double> constant(3, 0, 2);
    CHECK(coordinate_at(constant, -100) == 3);
    CHECK_THROWS(std::invalid_argument, find_index(constant, nan));
    CHECK_THROWS(std::invalid_argument, find_index(coarse, infinity));
    CHECK_THROWS(std::invalid_argument, map_index(coarse, nan));
    CHECK_THROWS(std::invalid_argument, map_index(linsp_<int>(0, 1, 2), infinity));
    CHECK_THROWS(std::invalid_argument, fractional_index(coarse, nan));
    CHECK_THROWS(std::invalid_argument, fractional_index(linsp_<double>{}, 0));
    CHECK_THROWS(std::invalid_argument, map_index_between(coarse, constant, 1));
    CHECK_THROWS(std::invalid_argument, map_index_between(linsp_<double>{}, coarse, 1));
    CHECK_THROWS(std::invalid_argument, map_index_between(coarse, linsp_<double>{}, 1));
    const long double max_real = (std::numeric_limits<long double>::max)();
    CHECK_THROWS(std::overflow_error, coordinate_at(linsp_<long double>(0, max_real, 2), 2));
    CHECK_THROWS(std::overflow_error, fractional_index(
        linsp_<long double>(0, (std::numeric_limits<long double>::min)(), 2), max_real));

    // Independently tabulated shared-coordinate solutions, including reverse
    // target direction, fractional inputs and extrapolation beyond either grid.
    const linsp_<int> forward(4, 2, 4);
    const linsp_<double> reverse(10, -4, 3);
    const long double indices[] = {-8, -0.5L, 0, 1, 3, 12};
    const long double expected[] = {5.5L, 1.75L, 1.5L, 1, 0, -4.5L};
    for (std::size_t i = 0; i < sizeof(indices) / sizeof(indices[0]); ++i) {
        CHECK(map_index_between(forward, reverse, indices[i]) == expected[i]);
        CHECK(map_index_between(reverse, forward, expected[i]) == indices[i]);
    }
    const linsp_<double> audio_source(0, 1.0 / 44100, 44100);
    const linsp_<double> audio_target(0, 1.0 / 48000, 48000);
    CHECK(std::abs(map_index_between(audio_source, audio_target, 441) - 480.0L) < 1e-10L);
    const rational_index_map half_pixel(2, 4, -1);
    CHECK(half_pixel(0).floor == -1);
    CHECK(half_pixel(0).remainder == 3);
    CHECK(half_pixel(0).ceil() == 0);
    CHECK(half_pixel(0).nearest() == 0);
    CHECK(half_pixel(1).value() == 0.25L);
    CHECK(rational_index_map(1, 2)(-1).nearest() == 0);
    CHECK(rational_index_map(-1, 2)(1).nearest() == 0);
    CHECK_THROWS(std::invalid_argument, rational_index_map(1, 0));
    CHECK_THROWS(std::overflow_error, rational_index_map(2, 1)((std::numeric_limits<std::int64_t>::max)()));
    // Random-access and block-local evaluation must agree exactly, including phase.
    for (std::int64_t numerator : {-7, -1, 0, 1, 5, 7})
        for (std::int64_t denominator : {1, 2, 3, 11, 160}) {
            const rational_index_map mapping(numerator, denominator, -3);
            for (std::int64_t begin : {-10000, -7, 0, 4096, 1000000000}) {
                const auto block = mapping.shifted_input(begin);
                for (std::int64_t local = 0; local < 257; ++local) {
                    const auto global = mapping(begin + local);
                    const auto split = block(local);
                    CHECK(global.floor == split.floor);
                    CHECK(global.remainder == split.remainder);
                    CHECK(global.denominator == split.denominator);
                    const auto reference = (numerator * (begin + local) - 3) /
                                           static_cast<long double>(denominator);
                    CHECK(global.floor == static_cast<std::int64_t>(std::floor(reference)));
                    CHECK(global.ceil() == static_cast<std::int64_t>(std::ceil(reference)));
                    CHECK(global.remainder >= 0 && global.remainder < denominator);
                }
            }
        }
}

