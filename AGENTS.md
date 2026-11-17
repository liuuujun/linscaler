# Project working agreement

linscaler is a general-purpose header-only mathematics library for uniform grids.
The basic layer (core.hpp, linscaler::core) and mapping layer (linscaler::transform)
support C++11; the full library remains C++17. Public APIs must remain independent
of RIP, image buffers, codecs, colour
management and other private application logic.

## Build and verification

- Use shared, machine-installed build tools. Never download, extract, or copy a
  compiler, CMake, Ninja, or SDK into this project checkout for validation.
- On this Windows workstation, use the Visual Studio 2022 preset and Git Bash
  `scripts/check.sh` for Debug and Release. Do not use the damaged system MinGW
  or the old `build/` cache.
- Use the CMake presets as the source of truth. Run Debug and Release for numerical
  changes, including the installed-package and subproject checks.
- Tests use always-active CHECK macros, not runtime `assert`.
- Retain independent oracles for set operations and exact block/global map equality.
- Keep public headers self-contained and the installed target dependency-free.
- Keep set dependencies out of the C++11 headers; core.hpp does not include mapping.
  Verify basic and mapping layers in strict C++11 mode, not just with a minimum
  feature flag.
- Never add development warnings, sanitizers or runtime-linkage options to the
  public INTERFACE target. Those belong to development targets/presets.

## Contracts and release

- Read `docs/semantics.md` before editing numerical behavior. Record changes in
  `CHANGELOG.md`; document rounding, domains, complexity and exceptions explicitly.
- Preserve checked integer arithmetic. Do not turn overflow/unrepresentability into
  an empty result, and do not fill gaps in a union.
- The owner selected MIT. Preserve the license text and copyright notices.
  Copyright attribution is Copyright (c) 2026 Liu Jun, as confirmed by the owner.
- No public release or remote repository has been created. Hosted CI configuration
  is not a passed CI run. State local versus remote verification accurately.
- `out/` and the historical `build/` are generated/local material and must remain
  outside source control. Preserve any existing `out/legacy-source` intake snapshot.
