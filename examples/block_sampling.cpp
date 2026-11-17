#include <linscaler/linscaler.hpp>
#include <iostream>
int main() {
    // Half-cell aligned 2x output sampling: source index = (2 * output_index - 1) / 4.
    const linscaler::transform::rational_affine_map map(2, 4, -1);
    constexpr std::int64_t begin = 4096;
    const auto block = map.shifted_input(begin);
    for (std::int64_t i = 0; i < 256; ++i) {
        const auto whole = map(begin + i), part = block(i);
        if (whole.floor != part.floor || whole.remainder != part.remainder) return 1;
    }
    std::cout << "256 block-local positions match the global mapping exactly.\n";
}

