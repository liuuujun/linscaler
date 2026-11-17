# 文档导航

[English](README.md) · [项目概览](../README.zh-CN.md)

英文文档使用 `.md`，对应中文文档使用 `.zh-CN.md`。两种语言描述相同的已实现行为；修改契约时应同步更新。

| 主题 | 中文 | English |
| --- | --- | --- |
| 数据模型、模块关系和路线图 | [架构](architecture.zh-CN.md) | [Architecture](architecture.md) |
| 映射选择、编译和复合 | [映射指南](mapping.zh-CN.md) | [Mapping guide](mapping.md) |
| 数值域、舍入、复杂度和异常 | [数学契约](semantics.zh-CN.md) | [Mathematical contracts](semantics.md) |
| 工具链、预设和验证流程 | [开发说明](development.zh-CN.md) | [Development](development.md) |
| 实际本地验证与历史证据 | [验证记录](validation.zh-CN.md) | [Validation record](validation.md) |
| 公开发布前的要求 | [发布检查表](release-checklist.zh-CN.md) | [Release checklist](release-checklist.md) |
| 变更和兼容说明 | [变更记录](../CHANGELOG.zh-CN.md) | [Changelog](../CHANGELOG.md) |
| 贡献工作流程 | [贡献说明](../CONTRIBUTING.zh-CN.md) | [Contributing](../CONTRIBUTING.md) |

建议先阅读项目概览和映射指南，再通过数学契约查阅边界情况。架构路线图区分已实现模块与未来候选能力。代码和测试配置的存在不代表验证通过；实际执行情况以验证记录为准。

原优化建议的有效内容已整合到这些文档中。应用专有迁移说明及本机源码路径不属于公共数学 API。
