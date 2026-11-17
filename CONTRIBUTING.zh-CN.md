# 贡献说明

[English](CONTRIBUTING.md) · [文档导航](docs/README.zh-CN.md)

项目采用 [MIT](LICENSE)，版权署名为 Copyright (c) 2026 Liu Jun。公开贡献政策仍待所有者决定发布安排。开发阶段遵循以下流程：

1. 修改数值行为前阅读[数学契约](docs/semantics.zh-CN.md)。
2. 保持 linsp 作为核心有限序列模型，映射值对象用于支持数学关系；公共接口不包含应用类型和规则。
3. 使用 CMake preset 和机器共享工具。Windows 下运行 Git Bash `scripts/check.sh` 或 `powershell.exe -NoProfile -ExecutionPolicy RemoteSigned -File ./scripts/check.ps1`，通过 `--preset NAME` 或 `-Preset NAME` 选择本机预设。本机 SDK 选择保存在已忽略的 `CMakeUserPresets.json` 中，生成文件放入 `out/`。环境配置见[开发说明](docs/development.zh-CN.md)，不向工作区下载构建工具。
4. 保持基础／映射头文件兼容 C++11，集合／完整入口要求 C++17，公共目标没有外部依赖。
5. 执行 Debug 和 Release。添加有意义的边界回归与可行的独立数学验证依据，保留全局／分块映射精确一致性检查。
6. 公共头文件独立编译并跨翻译单元链接，验证安装包、重定位和子项目消费者。严格 C++11 证据必须来自真正提供该语言模式的编译器。
7. 同步更新英中两版文档，包括[变更记录](CHANGELOG.zh-CN.md)、契约和迁移说明。未来 API 必须明确标注尚未实现。
8. 在[验证记录](docs/validation.zh-CN.md)中记录实际工具、命令、结果和未验证范围。托管配置及历史通过不能代替新的执行结果。

测试使用 NDEBUG 下仍有效的 CHECK，预期失败测试验证这一机制。新 API 必须说明复杂度、有效域、舍入和异常。验证数值契约时不启用 fast-math。开发告警、sanitizer 和运行库链接选项不得泄漏到公共 INTERFACE 目标。

问题报告应提供编译器／CMake 版本、预设、最小数学复现和期望结果，不包含私有应用源码或本机业务路径。保留 LICENSE 和版权声明原文。公开发布由所有者单独决定。
