# Local validation record

[中文](validation.zh-CN.md) · [Documentation](README.md)

## Current verification scope — 2026-11-16

The current 0.1.0 prerelease-stage numerical implementation has these user-run
local Windows/MSVC 19.42.34436 / SDK 10.0.22621.0 results:

| Configuration | Result | Total time | Package consumer |
| --- | --- | --- | --- |
| Debug | 18/18 passed | 42.46 s | 34.93 s |
| Release | 18/18 passed | 39.50 s | 34.77 s |

Both runs used `out/build/msvc-22621` and included numerical suites, standalone
headers/ODR, examples, installed/relocated packages and subproject consumers.
The Release expected-failure test also confirms that CHECKs remain active with
`NDEBUG`. These are user-executed local results, not hosted CI or a verification
of every supported compiler. MSVC compatibility targets use C++14; strict C++11,
Linux/macOS, sanitizers and hosted CI remain unverified for the current code.

The original logs are preserved locally as
`out/verification/2026-11-16-user-msvc-debug-LastTest.log` and
`out/verification/2026-11-16-user-msvc-release-LastTest.log`.
The Release log was checked for all 18 successful entries. Build-entry changes
made after these runs are recorded separately below; these numerical passes
must not be described as end-to-end runs of the updated scripts.

Earlier environment findings below are dated history, not current setup
instructions. The old project-local tool bootstrap and LLVM presets have been
retired; use the current [development procedure](development.md).

## Updated build entry point — 2026-11-16

The agent ran the updated `./scripts/check.sh --preset msvc-local` from Git Bash.
The script discovered the installed Visual Studio CMake without a hard-coded
path and selected SDK 10.0.22621.0 through ignored CMakeUserPresets.json.
Configuration, both builds and both complete CTest runs passed:

| Configuration | Result | Total test time | Package consumer |
| --- | --- | --- | --- |
| Debug | 18/18 passed | 43.63 s | 37.11 s |
| Release | 18/18 passed | 41.27 s | 37.57 s |

This verifies the updated entry point on the existing local MSVC build tree,
including fresh installed/relocated and subproject consumers. An initial
sandboxed attempt could not read local SDK metadata; the complete run succeeded
with access to the shared tools and metadata. No SDK files or persistent system
settings were changed. The log is
`out/verification/2026-11-16-entrypoint-msvc-local-shared-tools.log`.

Bash syntax/argument handling, Windows PowerShell 5 parsing, preset discovery,
Git Bash tool-path conversion and stopping on a failed configure were checked.
Source/license hashes confirm that all 38 files under include/tests/examples
and the root LICENSE/CMakeLists.txt remain unchanged by this cleanup.

## User-verified MSVC Debug pass — 2026-11-16

After the test-source portability fixes, the user ran CTest from Git Bash against
`out/build/msvc-22621` with `-C Debug --output-on-failure`. The supplied output
reports **18/18 tests passed**, zero failures, and 42.46 seconds total. The local
`Testing/Temporary/LastTest.log` also records the successful tests. This is a
user-executed local Windows/MSVC 19.42.34436 / SDK 22621 result, not an agent-run
or hosted-CI result.

All numerical suites, standalone-header/ODR checks, three examples and the
installed/relocated plus subproject consumer check passed in this Debug run.
`package_consumer` took 34.93 seconds because it creates fresh consumer build
directories and configures, compiles and runs three consumer executables for each
of two integration modes. This is integration-test overhead, not a mapping benchmark.

Release verification was pending at this point and subsequently passed as
recorded above. The `_cxx11` test
names identify compatibility targets, but MSVC runs them in its lowest selectable
C++14 mode; this run does not establish strict C++11 compatibility. GCC/Clang
strict C++11, other platforms, sanitizers and hosted CI still require separate
execution. Earlier agent-context failures below are retained as historical evidence.

## User-run MSVC build and test-source fixes — 2026-11-16

A subsequent build log supplied by the user, from their Git Bash/VS Code context
using `out/build/msvc-22621` and SDK 22621, successfully built all three examples
and numerous test executables. It stopped on warnings treated as errors in test
sources: C4127 for constant `has_denorm` conditions in the affine suite, and C4244
for an implicit int64-to-int conversion in the core-header peer. This log contains
no CTest execution results and does not establish a complete Debug or Release pass.

The test sources now use C++11 tag dispatch for optional subnormal checks and
`integer::value_type` for the peer's grid coordinates. All checks remain active;
the warning-as-error policy and public numerical behavior are unchanged.
Read-only source review found the original test cases retained. The agent's
targeted rebuild attempt still encountered invalid SDK header content before it
could verify the corrected targets; see
`out/verification/2026-11-16-msvc-test-portability.log`.

The user's successful source compilation shows that SDK 22621 can be used in
their execution context. Earlier agent-context reports of unreadable headers do
not establish that the installed SDK is physically damaged. The reason for the
different access/visibility behavior has not been determined. The subsequent
user-run Debug verification is recorded above; no SDK repair is inferred from
this test-source failure.

## Affine mapping implementation — 2026-11-16

The first implementation batch adds `floating_affine_map`, explicit grid-map
compilation, and the canonical `rational_affine_map` name with an old-name alias.
The new affine suite is registered in both the full-library and C++11 compatibility
targets. Standalone-header, multi-translation-unit, installed-package and subproject
consumers include the new APIs; the existing independent set and exact global/block
mapping tests remain. There are 18 registered CTest entries in the configured tree.

**The initial agent-context attempts did not produce a numerical-test pass.**
The later user-run Debug pass is recorded above. Initial attempts reached the
following results:

| Check | Observed result |
| --- | --- |
| Git Bash `scripts/check.sh Debug` and `scripts/check.sh Release` | Blocked during configuration/compiler checks by MSB8037: SDK 10.0.26100.0 is unavailable for desktop x64 builds |
| Diagnostic configuration from the `msvc` preset with installed SDK 10.0.22621.0 explicitly selected | Passed configuration using MSVC 19.42.34436; this is not a source-build pass |
| Diagnostic SDK 22621 Debug and Release source builds | Failed in the shared SDK's `ucrt/float.h` with invalid source characters (including C4828/C2018); test executables could not be built |
| Installed-package consumer checks, Debug and Release | Header installation, relocation and consumer configuration completed; consumer compilation failed in the same SDK headers |
| Independent subproject consumer, Debug and Release | Configuration passed; both source builds failed in the same SDK headers, so consumer tests did not run |
| Strict C++11 syntax-check attempt with the installed Visual Studio Clang 18.1.8 | Blocked by invalid SDK `ucrt/math.h` content visible to that process, using both the default SDK and explicit 22621 UCRT headers; no strict C++11 pass |
| Hosted CI, other operating systems, sanitizers and performance | Not run |

The initial sandboxed MSBuild invocation could not access local SDK metadata.
Repeating it with access to the installed tools exposed the SDK failures above;
the remaining failures are not evidence of a library regression or a successful
library build. No compiler, CMake, Ninja or SDK was downloaded or copied into the
checkout. No SDK files were repaired or replaced. WSL enumeration returned no
installed distribution for supplemental validation.

The diagnostic build uses the existing Visual Studio preset and a separate
generated directory:

```sh
cmake --preset msvc -B out/build/msvc-sdk22621 -A x64,version=10.0.22621.0
```

It does not change the project's supported build entry point. Use an execution
context that can access the installed SDK, refresh any stale generated cache and
rerun full Debug and Release verification, including package consumers. A shared GCC/Clang
environment is also required to complete strict C++11 validation; MSVC's lowest
selectable C++14 mode does not establish it.

Logs for these attempts are local generated files under
`out/verification/2026-11-16-*.log`. The new tests cover independent dyadic oracles,
composition order, hand-solved inverses, FMA cancellation, finite-result overflow
rescue, non-finite inputs, scale underflow, coefficient narrowing, reverse/constant
grids, large-origin divergence from the legacy mapper, and rational-name compatibility.
At that point these described coverage added, not passes in the agent's execution
context. The later user-run Debug and Release results are recorded above.

Static documentation checks passed: 127 local Markdown links resolve, every page
under `docs/` has an English/Chinese counterpart, and the obsolete proposal is
absent. Public-header guard presence/uniqueness checks also passed. The CTest JSON
inventory confirms 18 registered entries. These checks do not replace compilation
or numerical execution; their outputs are retained with the local verification logs.

## Historical Windows SDK diagnosis — 2026-10-16

Historical checkout note: the project was copied to E: without its
project-local tools or generated builds. The dated LLVM-MinGW results below describe
the earlier D: environment. Current Windows entry points use the shared Visual
Studio installation. CMake 3.29.5-msvc4 itself works, but the default SDK 26100
configuration failed at MSBuild MSB8037 because its x64 gdi32.lib is absent.
Selecting the installed SDK 22621 configured successfully with MSVC 19.42.34436,
but compilation failed in the system SDK's ucrt/float.h due to invalid characters
(C4828/C2018/C1004). No project tests ran on E:. The failed generated build
directories were removed, and no toolchain was downloaded into this checkout.
Repairing a shared Windows SDK is outside this repository's source changes.

## C++11 mapping layer — 2026-10-13

Ran scripts/bootstrap-windows.ps1 and scripts/check.ps1 with the same pinned
Windows x64 LLVM-MinGW / Clang 19.1.7, CMake 3.31.6 and Ninja 1.12.1 toolchain.

| Configuration | Result |
| --- | --- |
| Debug, C++11 basic/mapping layers and C++17 full library | 15/15 CTest entries passed |
| Release, C++11 basic/mapping layers and C++17 full library | 15/15 CTest entries passed |

The mapping suite now runs from the same source in C++11 and C++17. It covers
signed/unsigned integer and float/double/long double outputs, fractional results,
truncation boundaries, non-finite inputs, coordinate/result overflow, constant and
empty grids, reverse axes, extrapolation and mixed-type grid pairs. Existing exact
global/block map equality and independent set-oracle tests remain in the suite.

Both mapping headers also compile independently in strict C++11 and link across
multiple translation units. Installed/relocated and subproject consumers exercise
core, transform and full-library targets, checking their effective language modes.
The verification log is retained in ignored out/verification-mapper-cxx11.log.
This remains local reference-toolchain evidence; older compilers, native MSVC,
Linux/macOS and hosted CI have not been verified.

### Floating example output diagnosis

The previously incorrect printed value was reproduced outside the library with the
constant 480.0L. Direct long double formatting through cout, ostringstream and the
default printf path was incorrect in both -O0 and -O2 diagnostics; converting to
double printed 480. Independently, the mapper's long double result passed a 1e-10
absolute-error check against 480.0L.

Inspection traced libc++ num_put<long double> through snprintf_l/vsnprintf into
UCRT __stdio_common_vsprintf, where the extended-precision argument meets the
Windows double-formatting convention. Enabling MinGW ANSI stdio fixed printf in
the diagnostic but did not fix the precompiled libc++ stream implementation.
Diagnostic sources and binaries are retained in ignored out/mapper-diagnostics/.

The example now checks the original long double value for finiteness and accuracy,
then prints this bounded value as double. Its CTest wrapper checks both exit status
and the exact expected output, "Source index 441 maps to target index 480".
No mapper return type, arithmetic precision or public compiler flags were changed.
The general toolchain long double stream-formatting limitation remains; this
example workaround is not a general-purpose formatting conversion policy.

## C++11 basic layer — 2026-10-13

Ran scripts/bootstrap-windows.ps1, then scripts/check.ps1 using the pinned Windows
x64 LLVM-MinGW / Clang 19.1.7, CMake 3.31.6 and Ninja 1.12.1 toolchain.

| Configuration | Result |
| --- | --- |
| Debug, C++11 basic layer and C++17 full library | 13/13 CTest entries passed |
| Release, C++11 basic layer and C++17 full library | 13/13 CTest entries passed |

The same core and integer behavioral suites run in C++11 and C++17. Added coverage
instantiates signed, unsigned and all standard floating coordinate types, and
checks alignment, nearest-index ties, invalid tolerances, non-finite inputs and
the unsigned-maximum / signed-minus-one remainder distinction.

Six basic headers compile independently in strict C++11, alongside a multi-file
linkage check. Strict tests use -std=c++11, -pedantic-errors and a compile-time
__cplusplus check. Installed/relocated and add_subdirectory consumers exercise
both linscaler::core (C++11) and linscaler::linscaler (C++17).

The existing independent set oracles and global/block mapping tests also passed.
Output is retained in ignored out/verification-cxx11.log. This verifies language
compatibility on the reference compiler, not compatibility with every historical
C++11 compiler. Native MSVC, Linux/macOS and hosted CI remain unverified.

At this earlier stage, example_grid_mapping's CTest exit status checked only its
rational result, not its printed floating index. Its incorrect output was still
unresolved; see the subsequent mapping-layer diagnosis and example fix above.

## Engineering foundation — 2026-10-10

Date: 2026-10-10 (Asia/Shanghai).

Reference tools: Windows x64, LLVM-MinGW 20250114 / Clang 19.1.7,
CMake 3.31.6, Ninja 1.12.1. Tool archives match the SHA-256 values recorded in
the then-current scripts/toolchain-windows.json. That bootstrap and manifest
have since been retired; these are historical results, not current reproduction
instructions.

| Configuration | Result |
| --- | --- |
| Debug, C++17, warnings as errors | 10/10 CTest entries passed |
| Release, C++17, warnings as errors, NDEBUG | 10/10 CTest entries passed |

Coverage includes the original 28 test functions, checked arithmetic and boundary
regressions, 354,025 integral progression pairs checked against std::set,
2,401 dyadic floating progression pairs, global/block rational-map equality,
floating-coordinate rounding duplicates, individual public headers and ODR linkage.

Each configuration also installs and relocates the CMake package, builds/runs a
separate find_package consumer, and verifies add_subdirectory defaults and C++17
propagation. The intentionally failing CHECK test passed its expected-failure
contract in both configurations, proving checks remain active in Release.

The final local command output is retained in ignored out/verification-final.log.
Windows development executables use static runtime linkage to avoid dependency on
toolchain DLL lookup paths. This is not a consumer requirement of the library.

Not verified here: hosted CI, native MSVC source compilation, Linux/macOS runtime
behavior, sanitizer execution, or performance benchmarks. Their absence must not be
represented as a successful verification. CI definitions are ready for a future
remote repository. Public release approval is still pending.

License update (2026-10-12): the owner selected MIT and confirmed copyright
attribution to Liu Jun. Local CMake configuration and installation succeeded;
the installed LICENSE matched the source file by SHA-256. This license-only
verification does not constitute a new numerical test run.
