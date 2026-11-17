#include "test_support.hpp"
#include <linscaler/core.hpp>
#include <cstdint>
#include <limits>
#include <vector>
using linscaler::linsp::linsp_;

template<typename T>
void check_coordinate_type() {
    // These exact small values exercise every numeric dispatch path, including
    // unsigned and all standard floating types, without a rounding-dependent oracle.
    const linsp_<T> grid(T(2), T(2), 4);
    CHECK(grid[2] == T(6));
    CHECK(grid.contains(T(6)));
    CHECK(!grid.contains(T(5)));
    CHECK(grid.nearest(T(5)) == 2);
    CHECK(grid.aligned_with(linsp_<T>(T(4), T(2), 4)));
    CHECK(!grid.aligned_with(linsp_<T>(T(3), T(2), 4)));
    CHECK(!grid.parallel_to(linsp_<T>(T(2), T(3), 4)));
    CHECK(grid.subset(1, 2).last() == T(6));
    CHECK(*(grid.begin() + 2) == T(6));
}

int main() {
    check_coordinate_type<signed char>();
    check_coordinate_type<short>();
    check_coordinate_type<int>();
    check_coordinate_type<std::int64_t>();
    check_coordinate_type<std::uint32_t>();
    check_coordinate_type<float>();
    check_coordinate_type<double>();
    check_coordinate_type<long double>();
    using I = std::int64_t;
    const auto max = (std::numeric_limits<I>::max)();
    const auto min = (std::numeric_limits<I>::min)();
    CHECK_THROWS(std::out_of_range, linsp_<int>{}.last());
    CHECK_THROWS(std::invalid_argument, linsp_<int>(0, 1, 0));
    CHECK(linsp_<int>(5, 0, 9).nearest(100) == 0);
    CHECK(linsp_<int>(5, 0, 9).nearest(-100) == 0);
    CHECK_THROWS(std::overflow_error, linsp_<int>((std::numeric_limits<int>::max)(), 1, 2));
    CHECK_THROWS(std::overflow_error, linsp_<I>(max, 1, 2));
    CHECK_THROWS(std::overflow_error, linsp_<I>(0, max, 3));
    CHECK(linsp_<I>(min, 1, 2)[1] == min + 1);
    const linsp_<I> wide(max, min, 2);
    CHECK(wide.last() == -1);
    CHECK(wide.nearest(0) == 1);
    CHECK(wide.contains(-1));
    const linsp_<unsigned> unsigned_grid(10u, 2u, 4);
    CHECK(!unsigned_grid.contains(0u));
    CHECK(unsigned_grid.nearest(0u) == 0);
    CHECK(unsigned_grid.last() == 16u);
    const linsp_<int> descending(10, -2, 6);
    CHECK(descending.to_vector() == std::vector<int>({10, 8, 6, 4, 2, 0}));
    CHECK(descending.nearest(9) == 1);
    CHECK(descending.nearest(100) == 0);
    CHECK(descending.nearest(-100) == 5);
    CHECK_THROWS(std::out_of_range, descending.subset(1, (std::numeric_limits<std::size_t>::max)()));
    auto it = descending.end();
    it += -2;
    CHECK(*it == 2);
    CHECK(*(it - 2) == 6);
    const auto other = descending;
    CHECK(descending.begin() != other.begin());
    CHECK_THROWS(std::invalid_argument, descending.begin() - other.begin());
    CHECK_THROWS(std::out_of_range, descending.begin() - 1);
    CHECK_THROWS(std::out_of_range, *descending.end());
    const linsp_<double> tiny(0.0, 1e-12, 5);
    CHECK(!tiny.constant());
    CHECK(tiny.contains(2e-12));
    CHECK(!tiny.contains(0.5e-12));
    CHECK(tiny.nearest(2.1e-12) == 2);
    CHECK(!tiny.contains(std::numeric_limits<double>::quiet_NaN()));
    CHECK_THROWS(std::invalid_argument, tiny.nearest(std::numeric_limits<double>::infinity()));
    CHECK_THROWS(std::invalid_argument, linsp_<double>(0, std::numeric_limits<double>::infinity(), 2));
    CHECK_THROWS(std::invalid_argument, tiny.contains(0, -1));
    CHECK_THROWS(std::overflow_error, linsp_<double>((std::numeric_limits<double>::max)(),
                                                    (std::numeric_limits<double>::max)(), 2));
    CHECK(linscaler::precision::is_aligned(min, I(-1)));
    CHECK(!linscaler::precision::is_aligned(std::uint64_t(1),
                                           (std::numeric_limits<std::uint64_t>::max)()));
    CHECK(!linscaler::precision::is_approx_equal(1, 2, 100.0));
    CHECK(linscaler::precision::is_aligned(0, 0));
    CHECK(!linscaler::precision::is_aligned(1, 0));
    CHECK(linscaler::precision::is_aligned(0.0, 0.0));
    CHECK(!linscaler::precision::is_aligned(1.0, 0.0));
    CHECK_THROWS(std::invalid_argument, linscaler::precision::is_approx_equal(1, 1, -1));
    CHECK_THROWS(std::invalid_argument, linscaler::precision::is_aligned(0, 0, -1));
    CHECK_THROWS(std::invalid_argument, linsp_<int>{}.contains(0, -1));
    CHECK_THROWS(std::invalid_argument, linsp_<double>{}.contains(0.0, -1));
    const auto nan = std::numeric_limits<double>::quiet_NaN();
    CHECK_THROWS(std::invalid_argument, linsp_<double>{}.nearest(nan));
    CHECK_THROWS(std::invalid_argument, linsp_<double>(1, 0, 2).nearest(nan));
    CHECK_THROWS(std::invalid_argument, linsp_<double>(nan, 1, 2));
    CHECK_THROWS(std::invalid_argument, linscaler::precision::is_aligned(0.0, 1.0, nan));
    CHECK(!linscaler::precision::is_aligned(nan, 1.0));
    CHECK(linscaler::linsp::to_string(linsp_<int>{}) == "{}");
    CHECK(linscaler::linsp::to_string(descending) == "{10, 8, ..., 0}, d=-2, n=6");
    CHECK(!linscaler::precision::is_approx_equal(std::numeric_limits<double>::infinity(),
                                               std::numeric_limits<double>::infinity()));
    CHECK(linscaler::precision::is_aligned(2e-12, 1e-12, 1e-20));
    CHECK(!linscaler::precision::is_aligned(0.5e-12, 1e-12, 1e-20));
    // Every element is present, and subset extraction preserves global sequence values.
    for (int start = -10; start <= 10; ++start)
        for (int step = -5; step <= 5; ++step) {
            const linsp_<int> sequence(start, step, 10);
            for (std::size_t i = 0; i < sequence.count(); ++i) {
                CHECK(sequence.contains(sequence[i]));
                CHECK(sequence.subset(i, sequence.count() - i).last() == sequence.last());
            }
        }
}

