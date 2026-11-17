#include "cxx11_mode.hpp"
#include <linscaler/core.hpp>
#include <string>

std::string core_header_peer() {
    linscaler::precision::validate_epsilon(linscaler::precision::default_epsilon);
    const linscaler::linsp::linsp_<linscaler::integer::value_type> grid(
        linscaler::integer::add(1, 1), 3, 4);
    return linscaler::linsp::to_string(grid);
}
