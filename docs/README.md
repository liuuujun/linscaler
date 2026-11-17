# Documentation

[中文](README.zh-CN.md) · [Project overview](../README.md)

English pages use `.md`; their Chinese counterparts use `.zh-CN.md`. Both describe the same implemented behavior. Keep the pair synchronized when changing a contract.

| Topic | English | 中文 |
| --- | --- | --- |
| Model, module relationships and roadmap | [Architecture](architecture.md) | [架构](architecture.zh-CN.md) |
| Selecting and composing mappings | [Mapping guide](mapping.md) | [映射指南](mapping.zh-CN.md) |
| Domains, rounding, complexity and exceptions | [Mathematical contracts](semantics.md) | [数学契约](semantics.zh-CN.md) |
| Toolchains, presets and verification procedure | [Development](development.md) | [开发说明](development.zh-CN.md) |
| Actual local verification and historical evidence | [Validation record](validation.md) | [验证记录](validation.zh-CN.md) |
| Requirements before a public release | [Release checklist](release-checklist.md) | [发布检查表](release-checklist.zh-CN.md) |
| Changes and compatibility notes | [Changelog](../CHANGELOG.md) | [变更记录](../CHANGELOG.zh-CN.md) |
| Contributor workflow | [Contributing](../CONTRIBUTING.md) | [贡献说明](../CONTRIBUTING.zh-CN.md) |

Start with the project overview and mapping guide, then consult the mathematical contracts for edge cases. The architecture roadmap explicitly distinguishes implemented modules from future candidates. Implementation and test definitions do not establish a passed verification run; the validation record reports what was actually run.

The earlier optimization proposal has been consolidated into these documents. Application-specific migration notes and machine-local source links are not part of the public mathematical API.
