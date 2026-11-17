# 开发与验证流程

[English](development.md) · [文档导航](README.zh-CN.md) · [实际验证记录](validation.zh-CN.md)

## 环境要求与预设

使用机器上安装的共享工具，不将编译器、SDK、CMake 或 Ninja 下载、解压或复制到工作区。命令行、编辑器和验证使用的构建设置均以 CMake preset 为准。生成文件放入已忽略的 `out/`，不同编译器或 SDK 选择使用独立构建目录。保留已有的 `out/legacy-source` 快照。

Windows 环境需要安装 Visual Studio 2022 的“使用 C++ 的桌面开发”工作负载、Windows SDK，以及 CMake 3.24 或更新版本。Visual Studio 的 CMake 组件可以提供 CMake 和 CTest。`scripts/check.sh` 需要 Git Bash；PowerShell 用户可以直接调用 `scripts/check.ps1`。`msvc` 预设构建 x64，并让 Visual Studio 选择已安装的 SDK。此预设不需要安装 Ninja。

Linux 或 macOS 环境需要在项目之外安装 CMake 3.24 或更新版本、Ninja，以及 GCC 或 Clang，然后使用下方对应的预设。

## Windows 验证

在项目根目录的 Git Bash 中执行：

```bash
./scripts/check.sh
# 必要时只运行一个配置：
./scripts/check.sh Debug
./scripts/check.sh Release
```

脚本调用 `scripts/check.ps1`，先配置一次，再构建并测试所选配置，包含消费者接入检查。不指定配置时会执行 Debug 和 Release。直接使用 Windows PowerShell 时执行：

```powershell
powershell.exe -NoProfile -ExecutionPolicy RemoteSigned -File ./scripts/check.ps1
```

在命令末尾追加 `-Configuration Debug` 或 `-Configuration Release` 可只运行一个配置。`RemoteSigned` 仅作用于此子进程，不修改用户或系统的持久执行策略。Git Bash 包装脚本和 VS Code 任务已传入这一进程级选项。

两个入口都按以下顺序查找共享 CMake：`LINSCALER_CMAKE`、PATH、通过 `vswhere` 发现的 Visual Studio 2022 安装中的 CMake 组件。CTest 使用同一目录中的版本。`LINSCALER_CMAKE` 必须指向已安装的可执行文件，脚本不会下载工具。

默认配置预设为 `msvc`。可以设置 `LINSCALER_PRESET`，或传入 Bash `--preset NAME`／PowerShell `-Preset NAME`，使用另一套 Visual Studio 配置预设；显式参数优先于环境变量。该配置预设必须有对应的 `NAME-debug` 和 `NAME-release` 构建及测试预设。这两个 Windows 包装脚本用于 Visual Studio 预设；GCC 和 Clang 使用下方的直接命令。

CMake 和 CTest 已加入 PATH 时，也可以直接执行默认验证：

```text
cmake --preset msvc
cmake --build --preset msvc-debug
ctest --preset msvc-debug
cmake --build --preset msvc-release
ctest --preset msvc-release
```

每次构建成功后再执行对应的测试命令。数值改动必须验证 Debug 和 Release。

## 选择本机 Windows SDK

将本机配置保存在项目根目录的 `CMakeUserPresets.json` 中。Git 已忽略此文件。CMake 会同时读取共享的 `CMakePresets.json`，不应通过修改共享预设来固定某台机器的 SDK。

下面的完整示例选择**已安装的** SDK 10.0.22621.0。请根据自己的安装情况调整 SDK 版本和构建目录。生成器平台中的 `version=` 语法要求 CMake 3.27 或更新版本，普通 `msvc` 预设仍只要求 3.24。如果已有用户预设文件，应合并这些预设，保留其他设置。

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

使用该本机配置运行 Debug 和 Release：

```bash
./scripts/check.sh --preset msvc-local
# 或仅运行 Release：
./scripts/check.sh Release --preset msvc-local
```

直接使用 PowerShell 时，在上面的 `powershell.exe` 命令末尾追加 `-Preset msvc-local`。如果需要在 Git Bash 中反复使用，先运行 `export LINSCALER_PRESET=msvc-local`，再执行 `./scripts/check.sh`；PowerShell 中则先设置 `$env:LINSCALER_PRESET = 'msvc-local'`，再执行上述 PowerShell 命令。更换 SDK 或编译器时应使用独立构建目录，不复用生成器平台不同的缓存。

## 严格 C++11 与其他平台

MSVC 不提供严格 C++11 模式。兼容目标使用其最低可选的 C++14 模式，不能据此证明严格 C++11 兼容。GCC/Clang 检查使用实际 C++11 模式、禁用扩展、启用 `-pedantic-errors`，并检查实际 `__cplusplus`。完整库仍要求 C++17。

在已安装共享 CMake、Ninja 及 GCC 的主机上：

```sh
cmake --preset gcc-debug
cmake --build --preset gcc-debug
ctest --preset gcc-debug
cmake --preset gcc-release
cmake --build --preset gcc-release
ctest --preset gcc-release
```

使用 Clang 时替换为 `clang-debug` 和 `clang-release`。Linux 上的 `sanitizers` 预设启用地址和未定义行为检查：

```sh
cmake --preset sanitizers
cmake --build --preset sanitizers
ctest --preset sanitizers
```

CI 矩阵定义了 Windows/MSVC、Linux/GCC、Linux/Clang、macOS/Clang 和 Linux sanitizer 任务。严格 C++11 检查包含在 GCC/Clang 任务中。工作流配置不代表已运行通过，编译器版本和结果必须来自实际执行。当前本地证据及待完成验证见[验证记录](validation.zh-CN.md)。

## 必须覆盖的验证内容

- Debug 和 Release 下始终有效的 CHECK，包括预期失败的检查机制测试。
- 独立集合运算判据，以及分块／全局商余数的精确一致性。
- 仿射求值、复合顺序、常量与反向映射、FMA 消去、非法输入、溢出、尺度下溢、收窄及大原点差异。
- 严格 C++11 基础／映射行为和独立头文件检查，以及 C++17 完整库行为。
- 跨翻译单元链接，检测缺失 inline 定义及 ODR 问题。
- 三个公共目标的安装包、重定位和 `add_subdirectory` 消费者接入。
- 公共 INTERFACE 目标不泄漏开发告警、sanitizer 或运行库链接选项。

消费者测试创建独立暂存目录，安装并移动包，然后分别构建、运行安装包和子项目消费者。子项目默认不能启用开发测试／示例。错误和舍入契约同时记录在英中两版[数学契约](semantics.zh-CN.md)中。

正常映射路径不应逐点分配内存。性能结论需要基准测试，分别测量系数准备与重复求值，并同时报告数值结果和耗时。不能从代数简化推断加速或跨平台逐位一致。

## VS Code

安装推荐的 C/C++ 和 CMake Tools 扩展。在命令面板执行 **CMake: Select Configure Preset**，选择 `msvc` 或本机的 `msvc-local`。使用 CMake Tools 命令时，选择对应的 Debug 或 Release 构建／测试预设。

项目的 **linscaler: verify Debug and Release** 任务（`Ctrl+Shift+B`）调用 `scripts/check.ps1`，并要求输入配置预设名。使用上述 SDK 示例时输入 `msvc-local`，默认值为 `msvc`。任务中的选择与 CMake Tools 的选择相互独立。如果 CMake Tools 无法发现 CMake，可在 VS Code 用户设置中将 `cmake.cmakePath` 指向已安装的可执行文件。历史环境诊断保存在[验证记录](validation.zh-CN.md)中，不写入共享编辑器设置。
