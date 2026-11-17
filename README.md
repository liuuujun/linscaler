# linscaler

[中文](README.zh-CN.md) · [Documentation](docs/README.md) · [Mathematical contracts](docs/semantics.md)

A header-only mathematics library centered on finite uniform sequences (`linsp`). It computes coordinates, index relationships and representable set operations without external dependencies beyond the C++ standard library. Basic and mapping layers support C++11; the complete library requires C++17.

**Status:** version 0.1.0 is in the prerelease stage and has not been published; APIs may change during this stage. The first stable release is planned as 1.0.0. **License:** [MIT](LICENSE). Copyright (c) 2026 Liu Jun.

## Capabilities

- Finite arithmetic progressions with positive, negative or zero steps; queries, slices and random-access iteration.
- Checked integer arithmetic and intersection/union when representable as one progression, without filling gaps.
- Direct coordinate/index conversion through `mapper.hpp`.
- Floating affine value objects, composition, inverse and compilation from two grids.
- Exact rational affine index maps, quotient/remainder results and block phase preservation.

The numerical model is independent of the meaning or storage of data at a coordinate. Callers select units and application rules. Interpolation, filtering, data storage and device execution are outside the library. Index intervals and input-range queries remain [roadmap items](docs/architecture.md), not implemented APIs.

## Basic sequence (C++11)

```cpp
#include <linscaler/core.hpp>

linscaler::linsp::linsp_<double> grid(0.0, 0.5, 100);
auto coordinate = grid[3]; // 1.5
auto part = grid.subset(10, 20);
```

Include `<linscaler/core/linsp.hpp>` directly when only the data model is needed, without text formatting. `core.hpp` does not include transforms or C++17 set operations.

## Grid relationships (C++11)

```cpp
#include <linscaler/transform/grid_map.hpp>
#include <linscaler/transform/rational_affine_map.hpp>

using linscaler::linsp::linsp_;
using namespace linscaler::transform;
linsp_<int> source(10, 6, 5);
linsp_<int> target(4, 4, 10);
auto floating = compile_grid_map<double>(source, target);
auto index = floating(2.0); // 4.5

auto composed = compose(floating, floating_affine_map<double>(2.0, -1.0));
auto result = composed(2.0); // 8: first floating, then 2*x - 1

rational_affine_map exact(3, 2, 3); // (3*i + 3)/2
auto position = exact(2); // floor=4, remainder=1, denominator=2
```

`compile_grid_map` defaults to `long double` when the output type is omitted. Floating evaluation uses FMA; compiled and staged grid conversion need not produce identical floating or quantized results. The existing `mapper.hpp` interfaces are unchanged. `rational_index_map` and its old header remain compatibility spellings of `rational_affine_map`. See the [mapping guide](docs/mapping.md) for time-grid examples and precise distinctions.

For set operations include `<linscaler/core/linsp_ops.hpp>` or the full `<linscaler/linscaler.hpp>` (C++17). `try_unite` and `try_intersect` distinguish an empty result from one that cannot be represented by a single progression. Arithmetic failure is an exception, never an empty result.

## Consume

Copy `include/linscaler/` into an include search path while retaining [LICENSE](LICENSE), or use CMake:

```cmake
add_subdirectory(path/to/linscaler)
target_link_libraries(your_target PRIVATE linscaler::transform)
```

For an installed package, replace `add_subdirectory` with:

```cmake
find_package(linscaler 0.1 CONFIG REQUIRED)
```

| Target | Minimum standard | Use |
| --- | --- | --- |
| `linscaler::core` | C++11 | Basic sequence and numeric support |
| `linscaler::transform` | C++11 | Mapping; depends on core |
| `linscaler::linscaler` | C++17 | Full library, including sets |

Tests and examples default off in subprojects. Development compiler/linker options do not leak into public targets. No target forces a consumer to downgrade a newer language mode. Install a configured build using `cmake --install <build-directory> --prefix <destination>`.

## Build and verification

On Windows, install Visual Studio 2022 with the Desktop development with C++ workload and a Windows SDK, plus CMake 3.24 or newer. From Git Bash in the project root:

```bash
./scripts/check.sh
```

This uses the `msvc` preset and runs Debug, Release and consumer checks with machine-installed tools. The PowerShell equivalent is `powershell.exe -NoProfile -ExecutionPolicy RemoteSigned -File ./scripts/check.ps1`. A local SDK selection can be kept in an ignored `CMakeUserPresets.json` and selected with `./scripts/check.sh --preset msvc-local`; see [development](docs/development.md) for the complete example, tool discovery, VS Code and Linux/macOS commands.

The current numerical code passed all 18 tests in both Debug and Release on the user's Windows/MSVC 19.42 / SDK 22621 setup. Strict C++11, other platforms, sanitizers and hosted CI remain unverified for this code. The [validation record](docs/validation.md) records the evidence and its limits.

See [contributing](CONTRIBUTING.md), [changes](CHANGELOG.md) and the [release checklist](docs/release-checklist.md). Implementing a feature does not authorize publication or establish a release version.

