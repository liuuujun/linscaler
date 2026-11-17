# linscaler

[English](README.md) · [文档导航](docs/README.zh-CN.md) · [数学契约](docs/semantics.zh-CN.md)

以有限均匀序列（linsp）为核心的纯头文件数学库。计算坐标、索引关系和可表示的集合运算，除 C++ 标准库外没有外部依赖。基础层和映射层支持 C++11，完整库要求 C++17。

**状态：** 当前 0.1.0 处于预发布阶段，尚未公开发布；此阶段不承诺 API 稳定。首个正式稳定版本计划为 1.0.0。**许可证：** [MIT](LICENSE)。Copyright (c) 2026 Liu Jun。

## 当前能力

- 正、负、零步长的有限等差序列；查询、切片和随机访问迭代。
- 受检整数运算，以及单个等差序列可表示的交并集，不填平缺口。
- `mapper.hpp` 提供直接坐标／索引转换。
- 浮点仿射值对象、复合、求逆及两条网格之间的系数编译。
- 精确有理数仿射索引映射、商余数结果及分块相位保持。

数值模型不依赖坐标处的数据含义或存储方式，单位与应用规则由调用方决定。插值、滤波、数据存储和设备执行不属于该库。索引区间及输入范围反查仍是[路线图候选项](docs/architecture.zh-CN.md)，不是已实现 API。

## 基础序列（C++11）

```cpp
#include <linscaler/core.hpp>

linscaler::linsp::linsp_<double> grid(0.0, 0.5, 100);
auto coordinate = grid[3]; // 1.5
auto part = grid.subset(10, 20);
```

仅需数据模型、不需要文本格式化时，可直接包含 `<linscaler/core/linsp.hpp>`。`core.hpp` 不包含映射或 C++17 集合运算。

## 网格关系（C++11）

```cpp
#include <linscaler/transform/grid_map.hpp>
#include <linscaler/transform/rational_affine_map.hpp>

using linscaler::linsp::linsp_;
using namespace linscaler::transform;
linsp_<int> source(10, 6, 5);
linsp_<int> target(4, 4, 10);
auto floating = compile_grid_map<double>(source, target);
auto index = floating(2.0); // 4.5

auto composed = compose(floating, floating_affine_map<double>(2.0, -1.0));
auto result = composed(2.0); // 8：先执行 floating，再执行 2*x - 1

rational_affine_map exact(3, 2, 3); // (3*i + 3)/2
auto position = exact(2); // floor=4，remainder=1，denominator=2
```

`compile_grid_map` 省略输出类型时默认使用 long double。浮点求值使用 FMA，编译求值与逐步网格转换不保证浮点或量化结果一致。已有 `mapper.hpp` 接口保持不变。`rational_index_map` 及其旧头文件保留为 `rational_affine_map` 的兼容写法。时间网格示例及接口差异见[映射指南](docs/mapping.zh-CN.md)。

集合运算使用 `<linscaler/core/linsp_ops.hpp>` 或完整入口 `<linscaler/linscaler.hpp>`（C++17）。`try_unite` 和 `try_intersect` 区分空结果与无法用单个等差序列表示的结果。算术失败是异常，不会伪装为空集。

## 接入

可复制 `include/linscaler/` 到头文件搜索路径并保留 [LICENSE](LICENSE)，也可使用 CMake：

```cmake
add_subdirectory(path/to/linscaler)
target_link_libraries(your_target PRIVATE linscaler::transform)
```

使用安装包时，将 `add_subdirectory` 替换为：

```cmake
find_package(linscaler 0.1 CONFIG REQUIRED)
```

| 目标 | 最低标准 | 用途 |
| --- | --- | --- |
| `linscaler::core` | C++11 | 基础序列和数值支持 |
| `linscaler::transform` | C++11 | 映射；依赖基础层 |
| `linscaler::linscaler` | C++17 | 包含集合运算的完整库 |

作为子项目时，测试和示例默认关闭。开发编译／链接选项不会泄漏到公共目标，目标也不会强迫使用较新语言标准的调用方降级。配置完成后可通过 `cmake --install <build-directory> --prefix <destination>` 安装。

## 构建与验证

Windows 环境需要安装 Visual Studio 2022 的“使用 C++ 的桌面开发”工作负载、Windows SDK，以及 CMake 3.24 或更新版本。在项目根目录的 Git Bash 中执行：

```bash
./scripts/check.sh
```

脚本使用 `msvc` 预设及机器上安装的共享工具，执行 Debug、Release 和消费者接入检查。等价的 PowerShell 入口是 `powershell.exe -NoProfile -ExecutionPolicy RemoteSigned -File ./scripts/check.ps1`。本机 SDK 选择可以保存在已忽略的 `CMakeUserPresets.json` 中，再通过 `./scripts/check.sh --preset msvc-local` 使用；完整示例、工具查找规则、VS Code 及 Linux/macOS 命令见[开发说明](docs/development.zh-CN.md)。

当前数值代码已在用户的 Windows／MSVC 19.42／SDK 22621 环境中通过 Debug 和 Release 各 18 项测试。严格 C++11、其他平台、sanitizer 和托管 CI 尚未完成这版代码的验证。实际证据及适用范围见[验证记录](docs/validation.zh-CN.md)。

另见[贡献说明](CONTRIBUTING.zh-CN.md)、[变更记录](CHANGELOG.zh-CN.md)及[发布检查表](docs/release-checklist.zh-CN.md)。完成某项功能不代表已经获准发布，也不自动确定公开版本号。

