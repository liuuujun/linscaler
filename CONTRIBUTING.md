# Contributing

[中文](CONTRIBUTING.zh-CN.md) · [Documentation](docs/README.md)

The project uses [MIT](LICENSE), with Copyright (c) 2026 Liu Jun. Public contribution policy remains pending the owner's release decision. During development:

1. Read [mathematical contracts](docs/semantics.md) before changing numerical behavior.
2. Keep linsp as the central finite-sequence model; mapping values support mathematical relationships. Keep application types and rules outside public interfaces.
3. Use CMake presets and shared machine-installed tools. On Windows run Git Bash `scripts/check.sh` or `powershell.exe -NoProfile -ExecutionPolicy RemoteSigned -File ./scripts/check.ps1`; use `--preset NAME` or `-Preset NAME` for a local preset. Keep machine-specific SDK selections in ignored `CMakeUserPresets.json` and generated files in `out/`. See [development](docs/development.md) for setup; do not download build tools into the checkout.
4. Preserve C++11 basic/mapping headers, C++17 set/full-library entry points and dependency-free public targets.
5. Run Debug and Release. Add meaningful boundary regressions and independent mathematical oracles where feasible; retain exact global/block map equality checks.
6. Compile public headers individually and across translation units. Verify installed, relocated and subproject consumers. Use a compiler with an actual C++11 mode for strict compatibility evidence.
7. Update English and Chinese documentation together, including [changelog](CHANGELOG.md), contracts and migration notes. Clearly label future APIs as not implemented.
8. Record actual tools, commands, outcomes and unverified areas in the [validation record](docs/validation.md). Do not substitute hosted workflow definitions or historical passes for new results.

Tests use CHECK macros that remain active under NDEBUG. The expected-failure test verifies that mechanism. New APIs must state complexity, valid domains, rounding and exceptions. Do not enable fast-math when validating these numerical contracts. Development warnings, sanitizers and runtime-linkage flags must not leak into public INTERFACE targets.

Failure reports should include compiler/CMake versions, preset, a small mathematical reproducer and the expected result. Do not include private application source or local business paths. Keep LICENSE and copyright notices intact. Publication is a separate owner decision.

