# Release gate

[中文](release-checklist.zh-CN.md) · [Documentation](README.md) · [Validation record](validation.md)

A public repository may describe ongoing development. A tagged release requires evidence for its advertised behavior and support. A stable 1.x release additionally requires a deliberate compatibility commitment; implementing the roadmap is not itself a release gate.

- [x] Owner selects a license: MIT; the license text is in LICENSE.
- [x] Owner confirms attribution: Copyright (c) 2026 Liu Jun.
- [x] Owner sets the version plan: current 0.1.0 is in the prerelease stage; the first stable release will be 1.0.0.
- [ ] Review the semantic changes in both changelog versions.
- [ ] Confirm version.hpp, the CMake project version and version tests agree.
- [ ] Review public names, constructor argument order, compatibility aliases and headers.
- [ ] Review numerical domains, FMA behavior, rounding, underflow/overflow and changed exception behavior.
- [ ] Confirm the English and Chinese contracts, examples, module status and links agree.
- [ ] Run CI on Windows/MSVC, Linux/GCC, Linux/Clang and macOS/Clang.
- [x] Record local MSVC Debug and Release passes, including package relocation and subproject consumers (18/18 each, user-run 2026-11-16).
- [ ] Confirm sanitizer checks pass on a supported host.
- [ ] Confirm strict C++11 basic/mapping tests and all core/transform/full consumer targets pass.
- [ ] Retain independent mathematical oracles; do not treat composed/inverse implementations as their only proof.
- [ ] Verify examples in at least two independent mathematical application domains.
- [x] Prepare and inspect the local 0.1.0 source archive, excluding .tools, out, build and local configuration; preserve local generated material in place. Recreate the archive after any further source changes.
- [ ] Add repository and issue-reporting URLs after selecting a remote repository.
- [ ] Document supported toolchains using actual results; make performance claims only with measured evidence.
- [ ] Tag a release only after the owner approves publication.

The current 0.1.0 is in the prerelease stage, has not been published and carries no stable API commitment. Keep the code and CMake version at 0.1.0 until preparing the first stable release, 1.0.0. Local implementation does not publish a repository. Hosted CI configuration and historical results do not establish that the current code has passed CI. Index intervals, preimage queries and compensated arithmetic are future candidates, not completed features.

## Source material

Keep `include`, `tests`, `examples`, `cmake`, `scripts`, `docs`, the public project
configuration, license and bilingual root documents in the source distribution.
Do not ZIP the entire working directory: `.gitignore` does not filter ZIP tools.
Exclude `out`, `build`, `.tools`, `.vs`, `CMakeUserPresets.json` and generated CMake
files; retain local verification logs and any legacy-source snapshot in place.
After Git initialization, inspect the staged file list before committing.
The `.gitattributes` export rules also exclude local material and `AGENTS.md`
from `git archive`; they do not remove files from an existing commit.

