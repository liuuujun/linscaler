# 变更记录

[English](CHANGELOG.md) · [文档导航](docs/README.zh-CN.md)

## 未发布——工程基础

当前版本为 0.1.0，定位为尚未公开发布的预发布阶段；首个正式稳定版本计划为 1.0.0。当前开发树相对原型包含行为变化，不承诺可直接替换的二进制／API 兼容性。

### 公共构建入口（2026-11-16）

- 退役项目内 LLVM/Ninja 预设及工具下载脚本。Windows 验证使用已安装的 Visual
  Studio 2022 工具，支持显式工具／预设覆盖，通过 Visual Studio 发现机制替代本机固定路径。
- Git Bash 与 PowerShell 统一使用 CMake 的配置、构建和测试预设。本机 SDK 选择放入
  被忽略的 CMakeUserPresets.json，公共 msvc 预设适用于不同的已安装 Windows SDK。
- 中英文公共安装说明与带日期的本机诊断记录分离，补充 shell 脚本 LF 换行规则及源码归档排除项。
- 记录用户本地 MSVC Debug 和 Release 均 18/18 通过；当前实现的严格 C++11、其他平台、
  sanitizer 和托管 CI 仍未验证。

### 发布计划（2026-11-16）

- 记录所有者的决定：当前 0.1.0 作为预发布阶段，不承诺 API 稳定；首个正式稳定版本目标为 1.0.0。
- 代码和 CMake 版本保持 0.1.0，未创建发布标签或执行公开发布。

### MSVC 测试兼容性（2026-11-16）

- 将仿射测试中的 `has_denorm` 常量条件改为 C++11 标签分派，保留次正规数覆盖，避免 MSVC C4127，不引入 `if constexpr`。
- 基础头文件跨翻译单元测试采用与受检 int64 运算一致的网格坐标类型，消除触发 C4244 的隐式收窄。
- 保留警告视为错误的设置，公共数值行为不变。

### 仿射映射与双语文档（2026-11-16）

- 新增 C++11 `floating_affine_map<Real>` 值类型，独立头文件不依赖网格；支持有限系数、FMA 求值、明确的 `compose(first,second)` 顺序和连续求逆。除求逆外，零尺度合法。
- 在 `transform/grid_map.hpp` 新增 `compile_grid_map<Real=long double>`，采用 long double 中间运算及受检系数收窄。允许常量源网格，拒绝空网格和常量目标网格，保持明确的外推语义。系数计算中非零尺度变成零时抛出 `underflow_error`；偏移／求值下溢可以舍入为零。
- 保留已有 `mapper.hpp` 的公式、截断和异常。编译系数与 FMA 构成不同的浮点求值路径，在大原点或后续量化边界附近可能产生明显差异。
- 引入正式名称 `rational_affine_map`，旧名称 `rational_index_map` 保留为 C++11 别名，旧头文件转发到正式头文件。构造参数顺序、访问器、int64 检查、nearest 中点规则和全局／分块相位行为均不变，未增加有理数复合。
- 扩展仿射、兼容性、独立头文件／ODR 和消费者测试覆盖。实际运行结果（包括可能的环境失败）记录于[验证文档](docs/validation.zh-CN.md)，测试定义本身不代表 CI 通过。
- 架构、契约、映射使用、开发和发布说明整理为英中双语，并增加导航。整合后删除原优化建议；私有源码路径不是公共 API。
- 保持 linsp 核心地位、命名空间和 MIT 声明，区间／输入范围反查、通用舍入、有理数复合及补偿计算明确保留为路线图。

### Windows 共享开发工具（2026-10-16）

- E: 工作区的 Windows 验证入口改用机器安装的 Visual Studio 工具。Git Bash 可运行 scripts/check.sh；check.ps1 和托管 Windows 任务使用相同共享 MSVC 预设，不再进行工具引导下载。
- 早期 LLVM-MinGW 结果保留为历史证据。当日 E: 工作区使用 SDK 22621 完成共享 MSVC 配置，但在 SDK 的 ucrt/float.h 中编译失败；默认 SDK 26100 缺少 x64 gdi32.lib。当日 E: 尚无通过的 MSVC 构建或测试。

### C++11 映射层（2026-10-13）

- 使用共享的标签分派坐标转换，使 transform/mapper.hpp 兼容 C++11，保留接口、精度、截断和异常。
- 增加依赖 linscaler::core 的 C++11 INTERFACE 目标 linscaler::transform。集合运算和完整入口仍要求 C++17。
- 增加严格 C++11 映射回归、独立头文件／ODR 测试，包括既有 rational_index_map；安装包和子项目消费者覆盖三个公共目标。
- 将 grid_mapping 输出异常定位为指定 Windows 运行库的 long double 格式化问题。示例先验证 long double 数值，再将该有界结果作为 double 输出，同时检查输出和退出状态；mapper 返回类型及计算精度不变。

### C++11 基础层（2026-10-13）

- 增加 `<linscaler/core.hpp>` 及导出的 linscaler::core 目标，为 C++11 提供网格存储、查询、迭代、受检整数、精度和格式化。
- 将基础层 if constexpr 替换为标签分派，保留数值公式、验证顺序、舍入、容差、复杂度和异常。
- default_epsilon 仍为值 1e-6 的 constexpr 常量，现采用内部链接；其地址不再代表跨翻译单元的统一身份。
- 完整入口、集合运算及 linscaler::linscaler 仍要求 C++17。
- 增加严格 C++11 core/integer 回归及独立头文件／ODR 检查；安装包和子项目消费者覆盖当时的两个公共目标。

### 构建与验证

- 公共头文件从 linscaler/ 移到 include/linscaler/，包含写法仍为 `<linscaler/...>`；手动编译需要 `-I include`。
- CMake 最低版本提高到 3.24，完整 INTERFACE 目标传播 C++17 要求。
- 子项目默认关闭测试／示例，告警选项不泄漏给消费者。
- 增加包版本／配置文件及重定位／子目录消费者检查。
- 当时增加固定版本的本地 Windows 工具链、预设、CI 定义和文档。
- 保留原有 28 个测试函数，改用始终有效的检查；增加边界、独立判据、有理映射及独立头文件测试。

### 数学行为

- 整数运算对不支持的溢出抛出异常，不再回绕或触发未定义行为。
- 空序列 last() 抛出异常；subset 验证避免无符号溢出。
- nearest() 正确处理常量重复值；迭代器相等包含所属对象身份。
- 微小非零浮点步长保持非零，显式处理非有限输入。
- 相等运算采用精确结构比较，替代隐式近似比较。
- 交集归一化为升序唯一点，处理常量和偏移网格。
- 并集不再填平缺口；可表示交错网格及两个单点值的并集。
- try_unite/try_intersect 区分不可表示结果与空集；unite/intersect 对不可表示结果抛出 domain_error。
- map_index 截断完整坐标，修正负原点行为。
- 增加连续坐标／索引操作及精确 rational_index_map。
- 明确通用坐标类型域：不支持 bool 和 uint64_t。

### 范围

不引入插值／滤波引擎、多维图像存储或 RIP 业务逻辑。公开发布仍待决定。

### 许可证

- 所有者于 2026-10-12 选择 MIT，增加许可证原文。
- 所有者确认署名 Copyright (c) 2026 Liu Jun。
- CMake 安装将 LICENSE 放入配置的数据目录。
