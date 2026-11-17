#include "../cxx11_mode.hpp"
#include <linscaler/core.hpp>
#include <sstream>

int main() {
    const linscaler::linsp::linsp_<int> integers(8, -2, 5);
    const linscaler::linsp::linsp_<double> samples(0.0, 0.25, 5);
    std::ostringstream output;
    output << integers;
    return integers.last() == 0 && integers.contains(4) &&
           samples.contains(0.5) && samples.nearest(0.6) == 2 &&
           linscaler::integer::floor_div(-3, 2) == -2 &&
           linscaler::version == LINSCALER_VERSION &&
           output.str() == "{8, 6, ..., 0}, d=-2, n=5" ? 0 : 1;
}
