#include "test_support.hpp"
#include <linscaler/basic/integer.hpp>
#include <cmath>
#include <cstdint>
#include <limits>
using namespace linscaler::integer;
int main() {
    for (std::int64_t a = -100; a <= 100; ++a)
        for (std::int64_t b = -31; b <= 31; ++b) {
            CHECK(add(a, b) == a + b);
            CHECK(subtract(a, b) == a - b);
            CHECK(multiply(a, b) == a * b);
            if (b) {
                CHECK(floor_div(a, b) == static_cast<std::int64_t>(std::floor(double(a) / double(b))));
                CHECK(ceil_div(a, b) == static_cast<std::int64_t>(std::ceil(double(a) / double(b))));
            }
        }
    const auto max = (std::numeric_limits<std::int64_t>::max)();
    const auto min = (std::numeric_limits<std::int64_t>::min)();
    CHECK(add(min, max) == -1);
    CHECK(multiply(min, 1) == min);
    CHECK(multiply(min, 0) == 0);
    CHECK(floor_div(min, max) == -2);
    CHECK(ceil_div(min, max) == -1);
    CHECK(floor_div(max, min) == -1);
    CHECK(ceil_div(max, min) == 0);
    CHECK_THROWS(std::overflow_error, add(max, 1));
    CHECK_THROWS(std::overflow_error, subtract(min, 1));
    CHECK_THROWS(std::overflow_error, subtract(max, min));
    CHECK_THROWS(std::overflow_error, multiply(min, -1));
    CHECK_THROWS(std::overflow_error, multiply(max, 2));
    CHECK_THROWS(std::overflow_error, floor_div(min, -1));
    CHECK_THROWS(std::invalid_argument, ceil_div(1, 0));
}

