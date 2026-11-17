# Development and verification procedure

[中文](development.zh-CN.md) · [Documentation](README.md) · [Actual validation record](validation.md)

## Prerequisites and presets

Use machine-installed tools. Do not download, extract or copy a compiler, SDK, CMake or Ninja into the checkout. CMake presets define the build settings for command-line use, editors and verification. Keep generated files under ignored `out/`; use separate build directories for different compiler or SDK selections. Preserve an existing `out/legacy-source` snapshot.

On Windows, install Visual Studio 2022 with the Desktop development with C++ workload, an installed Windows SDK and CMake 3.24 or newer. The Visual Studio CMake component can supply CMake and CTest. Git Bash is needed for `scripts/check.sh`; PowerShell users can call `scripts/check.ps1` directly. The `msvc` preset builds x64 and lets Visual Studio select an installed SDK. No Ninja installation is needed for this preset.

On Linux or macOS, install CMake 3.24 or newer, Ninja, and GCC or Clang outside the project. Use the corresponding presets described below.

## Windows verification

From Git Bash in the project root:

```bash
./scripts/check.sh
# Run one configuration when needed:
./scripts/check.sh Debug
./scripts/check.sh Release
```

The script delegates to `scripts/check.ps1`, configures once, then builds and tests the selected configurations, including package consumers. With no configuration argument it runs both Debug and Release. The direct Windows PowerShell entry point is:

```powershell
powershell.exe -NoProfile -ExecutionPolicy RemoteSigned -File ./scripts/check.ps1
```

Append `-Configuration Debug` or `-Configuration Release` to run one configuration. `RemoteSigned` applies only to this child process and does not change persistent user or system policy. The Git Bash wrapper and VS Code task already pass this process-level option.

Both entry points locate shared CMake in this order: `LINSCALER_CMAKE`, then PATH, then the CMake component in a Visual Studio 2022 installation discovered with `vswhere`. CTest is taken from the same directory as CMake. `LINSCALER_CMAKE` must identify an installed executable. No tools are downloaded by these scripts.

The default configure preset is `msvc`. Set `LINSCALER_PRESET`, or pass Bash `--preset NAME` / PowerShell `-Preset NAME`, to use another Visual Studio configure preset. An explicit argument takes precedence over the environment variable. The selected preset must have corresponding build and test presets named `NAME-debug` and `NAME-release`. These Windows wrappers are for Visual Studio presets; use the direct commands below for GCC and Clang.

When CMake and CTest are on PATH, the default verification can also be run directly:

```text
cmake --preset msvc
cmake --build --preset msvc-debug
ctest --preset msvc-debug
cmake --build --preset msvc-release
ctest --preset msvc-release
```

Run each test command only after its build succeeds. Numerical changes require both Debug and Release.

## Selecting a local Windows SDK

Keep machine-specific selections in `CMakeUserPresets.json` at the project root. This file is ignored by Git. CMake automatically reads the shared `CMakePresets.json` alongside it; do not edit shared presets to encode a local SDK requirement.

The following complete example selects **an already installed** SDK 10.0.22621.0. Replace the SDK version and build directory as needed for your installation. The `version=` generator-platform syntax requires CMake 3.27 or newer; the ordinary `msvc` preset continues to require only 3.24. If a user-presets file already exists, merge these presets into it rather than replacing unrelated settings.

```json
{
  "version": 5,
  "cmakeMinimumRequired": { "major": 3, "minor": 27, "patch": 0 },
  "configurePresets": [
    {
      "name": "msvc-local",
      "inherits": "msvc",
      "architecture": "x64,version=10.0.22621.0",
      "binaryDir": "${sourceDir}/out/build/msvc-22621"
    }
  ],
  "buildPresets": [
    {
      "name": "msvc-local-debug",
      "inherits": "msvc-debug",
      "configurePreset": "msvc-local"
    },
    {
      "name": "msvc-local-release",
      "inherits": "msvc-release",
      "configurePreset": "msvc-local"
    }
  ],
  "testPresets": [
    {
      "name": "msvc-local-debug",
      "inherits": "msvc-debug",
      "configurePreset": "msvc-local"
    },
    {
      "name": "msvc-local-release",
      "inherits": "msvc-release",
      "configurePreset": "msvc-local"
    }
  ]
}
```

Run both configurations with this local selection:

```bash
./scripts/check.sh --preset msvc-local
# Or only Release:
./scripts/check.sh Release --preset msvc-local
```

For direct PowerShell use, append `-Preset msvc-local` to the `powershell.exe` command above. For repeated use in Git Bash, run `export LINSCALER_PRESET=msvc-local`, then `./scripts/check.sh`. In PowerShell set `$env:LINSCALER_PRESET = 'msvc-local'` before running the PowerShell command. Changing the SDK or compiler requires a separate build directory; do not reuse a cache configured with a different generator platform.

## Strict C++11 and other platforms

MSVC has no strict C++11 mode. The compatibility targets use its lowest selectable C++14 mode, which cannot establish strict C++11 compatibility. GCC/Clang runs use the actual C++11 mode, extensions disabled, `-pedantic-errors`, and an effective `__cplusplus` check. The full library still requires C++17.

On a host with shared CMake, Ninja and GCC installed:

```sh
cmake --preset gcc-debug
cmake --build --preset gcc-debug
ctest --preset gcc-debug
cmake --preset gcc-release
cmake --build --preset gcc-release
ctest --preset gcc-release
```

Use `clang-debug` and `clang-release` instead for Clang. On Linux, the `sanitizers` preset enables address and undefined-behavior checks:

```sh
cmake --preset sanitizers
cmake --build --preset sanitizers
ctest --preset sanitizers
```

The CI matrix defines Windows/MSVC, Linux/GCC, Linux/Clang, macOS/Clang and Linux sanitizer jobs. Strict C++11 checks run as part of the GCC/Clang jobs. A workflow definition is not a passed run: record compiler versions and outcomes from actual execution. Current local evidence and outstanding verification are in the [validation record](validation.md).

## What verification must cover

- Always-active CHECK assertions in Debug and Release, including the expected-failure check test.
- Independent set oracles and exact global/block quotient-remainder equality.
- Affine evaluation, composition order, constant and reverse maps, FMA cancellation, invalid inputs, overflow, scale underflow, narrowing and large-origin differences.
- Strict C++11 basic/mapping behavior and standalone-header checks; C++17 full-library behavior.
- Multiple translation units to detect missing inline definitions and ODR problems.
- Installed, relocated and `add_subdirectory` consumers for all three public targets.
- No leaked development warning, sanitizer or runtime-linkage options in public INTERFACE targets.

The package consumer test creates a unique staging directory, installs and relocates the package, then builds and runs installed-package and subdirectory consumers. Subprojects must leave development tests/examples disabled by default. Error and rounding contracts are documented in both language versions of [semantics](semantics.md).

Normal mapping paths should not allocate per point. Performance claims require benchmarks separating coefficient preparation from repeated evaluation, and reporting numerical results as well as timing. No speedup or cross-platform bitwise equality is inferred from algebraic simplification.

## VS Code

Install the recommended C/C++ and CMake Tools extensions. Open the command palette and select **CMake: Select Configure Preset**, then choose `msvc` or your local `msvc-local` preset. Select the matching Debug or Release build/test preset when using the CMake Tools commands.

The project's **linscaler: verify Debug and Release** task (`Ctrl+Shift+B`) invokes `scripts/check.ps1` and asks for a configure preset name. Enter `msvc-local` when using the SDK example above; the default is `msvc`. This task selection is independent of the CMake Tools selection. If CMake Tools cannot find CMake, set `cmake.cmakePath` in VS Code user settings to an installed executable. Historical environment diagnostics belong in the [validation record](validation.md), not in shared editor settings.
