#if defined(_MSVC_LANG)
#if _MSVC_LANG < 201703L
#error "The full library target must enable C++17 or newer"
#endif
#elif __cplusplus < 201703L
#error "The full library target must enable C++17 or newer"
#endif

#include <linscaler/linscaler.hpp>
#include <optional>
int main() {
    std::optional<linscaler::linsp::linsp_<int>> result =
        linscaler::linsp::try_unite<int>({0, 2, 3}, {1, 2, 3});
    const auto position = linscaler::transform::rational_index_map(5, 3)(6);
    const auto exact = linscaler::transform::rational_affine_map(5, 3)(6);
    const auto compiled = linscaler::transform::compile_grid_map<double>(
        linscaler::linsp::linsp_<int>(10, 6, 5),
        linscaler::linsp::linsp_<double>(4, 4, 10));
    const auto composed = linscaler::transform::compose(
        compiled, linscaler::transform::floating_affine_map<double>(2, -1));
    return result && result->count() == 6 && position.floor == 10 && exact.floor == 10 &&
           compiled(2) == 4.5 && composed(2) == 8 ? 0 : 1;
}


