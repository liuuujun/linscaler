# Changelog

[中文](CHANGELOG.zh-CN.md) · [Documentation](docs/README.md)

## Unreleased — engineering foundation

The current version is 0.1.0, designated as prerelease-stage work that has not
been published. The first stable release is planned as 1.0.0.
This tree contains behavioral changes from the original prototype; it is not a
drop-in binary/API compatibility promise.

### Public build entry points (2026-11-16)

- Retired project-local LLVM/Ninja presets and tool-download scripts. Windows
  verification uses installed Visual Studio 2022 tools, with explicit tool/preset
  overrides and Visual Studio discovery instead of workstation-specific paths.
- Unified Git Bash and PowerShell verification around CMake configure/build/test
  presets. Local SDK selection belongs in ignored CMakeUserPresets.json; the
  public msvc preset remains portable across installed Windows SDK versions.
- Separated public setup instructions from dated machine diagnostics in both
  languages and added LF handling for shell scripts and source-archive exclusions.
- Recorded the user's local MSVC Debug and Release results: 18/18 tests passed
  in each configuration. Strict C++11, other platforms, sanitizers and hosted CI
  remain unverified for the current implementation.

### Release plan (2026-11-16)

- Recorded the owner's decision to treat the current 0.1.0 as prerelease-stage
  work, with no stable API commitment, and target 1.0.0 for the first stable release.
- Kept the code and CMake version at 0.1.0; no release tag or publication was created.

### MSVC test portability (2026-11-16)

- Replaced constant `has_denorm` branches in affine tests with C++11 tag dispatch,
  preserving subnormal coverage without MSVC C4127 or requiring `if constexpr`.
- Kept the core-header peer's grid coordinate type consistent with checked int64
  arithmetic, removing an implicit narrowing conversion reported as C4244.
- Warning-as-error settings and public numerical behavior are unchanged.

### Affine mapping and bilingual documentation (2026-11-16)

- Added the C++11 `floating_affine_map<Real>` value type in its own grid-independent
  header, with finite coefficients, FMA evaluation, explicit `compose(first, second)`
  order and continuous inverse. Zero scale is valid except when taking an inverse.
- Added `compile_grid_map<Real=long double>` in `transform/grid_map.hpp`, using
  long double intermediates and checked coefficient narrowing. Constant sources
  are accepted, empty grids and constant targets are rejected, and extrapolation
  remains explicit. A nonzero scale that collapses to zero during coefficient
  computation throws `underflow_error`; offset/evaluation underflow may round to zero.
- Kept existing `mapper.hpp` formulas, truncation and exceptions unchanged.
  Compiled coefficients and FMA form a distinct floating evaluation path; results
  can differ substantially near large origins or later quantization boundaries.
- Introduced `rational_affine_map` as the canonical name. The previous
  `rational_index_map` name remains a C++11 alias and its header forwards to the
  canonical header. Constructor order, accessors, int64 checks, nearest ties and
  global/block phase behavior are preserved; no rational composition is added.
- Extended affine, compatibility, independent-header/ODR and consumer coverage.
  Actual execution results, including any environment failures, are recorded in
  [validation](docs/validation.md); test definitions are not a claim of passing CI.
- Reorganized architecture, contracts, mapping usage, development and release
  documentation into English/Chinese pairs with navigation. Consolidated and
  removed the earlier optimization proposal; private source paths are not public API.
- Kept linsp central, namespace and MIT notices unchanged, and interval/preimage,
  generic rounding, rational composition and compensation work explicitly on the roadmap.

### Shared Windows development tools (2026-10-16)

- The E: checkout uses machine-installed Visual Studio tools for its Windows
  verification entry points. Git Bash can run scripts/check.sh; check.ps1 and the
  hosted Windows job now use the same shared MSVC preset without bootstrapping.
- Earlier LLVM-MinGW results remain historical evidence. The E: checkout at that date
  reached shared MSVC configuration with SDK 22621 but source compilation failed
  in that SDK's ucrt/float.h. Its default SDK 26100 is missing x64 gdi32.lib;
  no MSVC build or test had passed on E: at that date.

### C++11 mapping layer (2026-10-13)

- Made transform/mapper.hpp compatible with C++11 using shared tag-dispatched
  coordinate conversion, preserving interfaces, precision, truncation and exceptions.
- Added linscaler::transform, a C++11 interface target depending on linscaler::core.
  Set operations and the full library entry point still require C++17.
- Added strict C++11 mapping regression and standalone-header/ODR tests, including
  the existing rational_index_map; installed and subproject consumers exercise
  all three public targets.
- Diagnosed the grid_mapping output anomaly as long double formatting in the pinned
  Windows runtime. The example now validates its long double numeric result, prints
  this bounded result as double, and tests both output and exit status; mapper
  return types and calculation precision are unchanged.

### C++11 basic layer (2026-10-13)

- Added <linscaler/core.hpp> and the exported linscaler::core target for C++11
  grid storage, queries, iteration, checked integers, precision and formatting.
- Replaced basic-layer if constexpr with tag dispatch, preserving numeric formulas,
  validation order, rounding, tolerance, complexity and exceptions.
- default_epsilon remains a constexpr value of 1e-6, now with internal linkage;
  its address is no longer a single shared identity across translation units.
- The full entry point, set operations and linscaler::linscaler remain C++17.
- Added strict C++11 core/integer regression runs and standalone-header/ODR checks;
  installed and subproject consumers exercise both public targets.

### Build and verification

- Public headers moved from linscaler/ to include/linscaler/. Include spellings stay
  <linscaler/...>; manual compiler invocations must use -I include.
- CMake minimum raised to 3.24; C++17 propagates through the full interface target.
- Tests/examples default off for subprojects; warning flags do not leak to consumers.
- Package version/config files and relocation/subdirectory consumer checks added.
- Fixed-version local Windows toolchain, presets, CI definitions and documentation added.
- Original 28 test functions retained with always-active checks; edge, oracle,
  rational mapping and independent-header tests added.

### Mathematical behavior

- Integer arithmetic now throws on unsupported overflow instead of wrapping or invoking UB.
- Empty last() throws; subset arithmetic avoids unsigned overflow.
- nearest() handles repeated constant values; iterator equality includes owner identity.
- Tiny floating steps remain nonzero; non-finite inputs are handled explicitly.
- Equality is exact structural equality, replacing implicit approximate equality.
- Intersections normalize to ascending unique points and handle constants and shifted grids.
- Unions no longer fill gaps; interleaving grids and two singleton values can be represented.
- try_unite/try_intersect distinguish unrepresentable sets from empty sets.
  unite/intersect throw domain_error for nonrepresentable results.
- map_index truncates the complete coordinate, correcting negative-origin behavior.
- Added continuous coordinate/index operations and exact rational_index_map.
- Generic coordinate type domain is explicit: bool and uint64_t are not supported.

### Scope

No interpolation/filtering engine, multidimensional image storage or RIP business
logic is introduced. Public release remains pending.

### License

- The owner selected MIT on 2026-10-12; added the license text.
- Copyright attribution is Copyright (c) 2026 Liu Jun, confirmed by the owner.
- CMake installation includes LICENSE under the configured data directory.

