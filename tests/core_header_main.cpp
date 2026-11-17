#include "cxx11_mode.hpp"
#include <linscaler/core.hpp>
#include "test_support.hpp"
#include <sstream>
#include <string>

std::string core_header_peer();

int main() {
    const linscaler::linsp::linsp_<int> grid(2, 3, 4);
    std::ostringstream output;
    output << grid;
    CHECK(output.str() == "{2, 5, ..., 11}, d=3, n=4");
    CHECK(core_header_peer() == output.str());
    CHECK(linscaler::precision::is_approx_equal(1.0, 1.0));
    CHECK(linscaler::integer::floor_div(-3, 2) == -2);
    CHECK(linscaler::version == LINSCALER_VERSION);
}
