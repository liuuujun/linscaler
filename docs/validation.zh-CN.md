# 本地验证记录

[English](validation.md) · [文档导航](README.zh-CN.md)

## 当前验证范围 — 2026-11-16

当前 0.1.0 预发布阶段的数值实现已有以下用户执行的本地 Windows／
MSVC 19.42.34436／SDK 10.0.22621.0 验证结果：

| 配置 | 结果 | 总耗时 | 包消费者检查 |
| --- | --- | --- | --- |
| Debug | 18/18 通过 | 42.46 秒 | 34.93 秒 |
| Release | 18/18 通过 | 39.50 秒 | 34.77 秒 |

两次均使用 `out/build/msvc-22621`，覆盖数值测试、独立头文件／ODR、示例、
安装包／重定位和子项目消费者。Release 中预期失败的测试还确认了 `NDEBUG`
下 CHECK 仍然生效。这些是用户执行的本地结果，不是托管 CI 或对所有支持编译器
的验证。MSVC 兼容性目标使用 C++14；当前代码的严格 C++11、Linux/macOS、
sanitizer 和托管 CI 仍未验证。

原始日志已保存在本地的
`out/verification/2026-11-16-user-msvc-debug-LastTest.log` 和
`out/verification/2026-11-16-user-msvc-release-LastTest.log`。
Release 日志已核对包含全部 18 项成功记录。上述运行之后的构建入口调整单独记录于
下方，不能把这些数值测试结果说成新版脚本的端到端通过记录。

下方环境诊断属于带日期的历史，不是当前安装说明。旧项目内工具下载脚本及 LLVM
预设已经退役；请使用当前[开发流程](development.zh-CN.md)。

## 更新后的构建入口 — 2026-11-16

代理从 Git Bash 执行了新版 `./scripts/check.sh --preset msvc-local`。
脚本通过发现机制找到已安装的 Visual Studio CMake，没有使用固定安装路径，
并通过被忽略的 CMakeUserPresets.json 选择 SDK 10.0.22621.0。
配置、两种构建和两轮完整 CTest 均通过：

| 配置 | 结果 | 测试总耗时 | 包消费者检查 |
| --- | --- | --- | --- |
| Debug | 18/18 通过 | 43.63 秒 | 37.11 秒 |
| Release | 18/18 通过 | 41.27 秒 | 37.57 秒 |

这验证了新版入口在现有本地 MSVC 构建树上的完整流程，包含全新构建的安装包／
重定位及子项目消费者。最初的沙箱运行无法读取本机 SDK 元数据，获得共享工具及
元数据访问权限后完整运行成功。没有修改 SDK 文件或持久系统设置。日志见
`out/verification/2026-11-16-entrypoint-msvc-local-shared-tools.log`。

另已检查 Bash 语法／参数处理、Windows PowerShell 5 解析、预设发现、Git Bash
工具路径转换，以及配置失败后停止。源码／许可证哈希确认本次整理未修改
include/tests/examples 下及根目录 LICENSE/CMakeLists.txt 共 38 个文件。

## 用户验证的 MSVC Debug 通过记录 — 2026-11-16

测试源码兼容性修正后，用户在 Git Bash 中对 `out/build/msvc-22621` 执行了
`-C Debug --output-on-failure` 的 CTest 验证。提供的输出显示 **18/18 项测试全部通过**，
失败数为零，总耗时 42.46 秒。本地 `Testing/Temporary/LastTest.log` 也记录了这些
通过结果。这是用户执行的本地 Windows／MSVC 19.42.34436／SDK 22621 验证，
不是代理进程执行或托管 CI 的结果。

本次 Debug 验证通过了全部数值测试、独立头文件／ODR 检查、三个示例，以及安装包／
重定位和子项目消费者检查。`package_consumer` 耗时 34.93 秒，因为它为两种接入方式
分别创建全新的消费者构建目录，并配置、编译和运行三个消费者程序。这是接入检查
开销，不是映射算法性能基准。

在此时 Release 验证仍待完成，随后已通过，结果见上方。名称含 `_cxx11` 的测试是兼容性目标，但 MSVC
实际使用最低可选的 C++14 模式，因此本次通过不证明严格 C++11 兼容性。GCC／Clang
严格 C++11、其他平台、sanitizer 和托管 CI 仍需单独执行。下方保留此前代理进程的
失败记录作为历史证据。

## 用户运行的 MSVC 构建与测试源码修正 — 2026-11-16

用户随后提供了在自己的 Git Bash／VS Code 环境中，使用 `out/build/msvc-22621`
和 SDK 22621 的构建日志。该次构建成功生成了三个示例及多个测试可执行文件，随后因
测试源码中的警告视为错误而停止：仿射测试的 `has_denorm` 常量条件触发 C4127，
基础头文件跨翻译单元测试的隐式 int64 到 int 转换触发 C4244。该日志没有 CTest
执行结果，不能证明完整 Debug 或 Release 验证通过。

测试源码现已使用 C++11 标签分派选择次正规数检查，并将 peer 网格坐标类型改为
`integer::value_type`。原有检查均保留，警告视为错误的设置与公共数值行为不变。
只读源码审查确认原有用例保留。代理进程尝试重新构建相关目标时，仍在 SDK 头文件
出现无效内容，未能验证修正后的目标；日志见
`out/verification/2026-11-16-msvc-test-portability.log`。

用户的成功源码编译表明 SDK 22621 在其执行环境中可用。此前代理进程看到不可读的
头文件内容，不能据此认定已安装的 SDK 存在物理损坏。两种执行环境之间访问／可见性
不同的原因尚未确定。后续用户运行的 Debug 验证结果已记录于上方；本次测试源码报错
不构成需要修复 SDK 的依据。

## 仿射映射实现 — 2026-11-16

首批实现新增 `floating_affine_map`、显式网格映射编译，以及规范名称
`rational_affine_map`；旧名称通过类型别名保留。新增仿射测试同时注册到完整库和
C++11 兼容性目标。独立头文件、多翻译单元、安装包及子项目消费者检查均包含新 API；
原有的独立集合预期验证和全局／分块精确映射测试继续保留。配置后的构建树中共注册
18 项 CTest 测试。

**最初的代理进程验证没有得到数值测试通过结果。** 后续用户运行的 Debug 通过结果
已记录于上方。最初尝试如下：

| 检查 | 实际结果 |
| --- | --- |
| Git Bash `scripts/check.sh Debug` 和 `scripts/check.sh Release` | 在配置／编译器检查阶段被 MSB8037 阻断：SDK 10.0.26100.0 不可用于桌面 x64 构建 |
| 从 `msvc` 预设显式选择已安装 SDK 10.0.22621.0 的诊断配置 | 使用 MSVC 19.42.34436 配置成功；这不代表源码构建通过 |
| SDK 22621 诊断构建，Debug 和 Release | 在共享 SDK 的 `ucrt/float.h` 中因非法源码字符失败，包括 C4828/C2018；无法构建测试可执行文件 |
| 安装包消费者检查，Debug 和 Release | 头文件安装、重定位和消费者配置完成；消费者编译在同一组 SDK 头文件中失败 |
| 独立子项目消费者，Debug 和 Release | 配置成功；两种配置的源码构建均在同一组 SDK 头文件中失败，因此未运行消费者测试 |
| 使用已安装的 Visual Studio Clang 18.1.8 尝试严格 C++11 语法检查 | 默认 SDK 和显式选择的 22621 UCRT 头文件均因该进程可见的 `ucrt/math.h` 内容无效而阻断；没有严格 C++11 通过结果 |
| 托管 CI、其他操作系统、sanitizer 和性能测试 | 未运行 |

最初在沙箱内调用 MSBuild 时无法访问本机 SDK 元数据。在允许访问已安装工具后重试，
暴露出上述 SDK 故障；后续失败既不能证明库发生了回归，也不能代表库已成功构建。
没有向项目工作区下载或复制编译器、CMake、Ninja 或 SDK。没有修复或替换 SDK 文件。
WSL 枚举未发现可用于补充验证的已安装发行版。

诊断构建使用现有 Visual Studio 预设和独立的生成目录：

```sh
cmake --preset msvc -B out/build/msvc-sdk22621 -A x64,version=10.0.22621.0
```

这不会改变项目支持的构建入口。应使用能够访问已安装 SDK 的执行环境，刷新失效的
生成缓存，并重新运行完整 Debug 和 Release 验证，包含包消费者检查。完成严格 C++11
验证还需要可用的共享 GCC/Clang 环境；MSVC 最低可选的 C++14 模式不能证明严格
C++11 兼容性。

这些尝试的日志是本地生成文件，位于 `out/verification/2026-11-16-*.log`。
新增测试覆盖独立的二进制分数预期、复合顺序、手算逆映射、FMA 消去、最终结果有限时
避免中间乘积溢出、非有限输入、尺度下溢、系数收窄、反向／常量网格、大原点下与旧
mapper 的结果差异，以及有理数映射名称兼容性。在当时，这些内容说明新增了哪些
测试覆盖，不表示测试已在代理执行环境中通过；后续用户运行的 Debug、Release
结果已记录于上方。

静态文档检查通过：127 个 Markdown 本地链接均有效，`docs/` 中每个页面均有英中对应版本，
旧优化建议文件已不存在。公共头文件的包含保护存在性／唯一性检查也通过。CTest JSON
清单确认共注册 18 项测试。这些检查不能代替编译或数值执行；输出与本地验证日志一起保留。

## 历史 Windows SDK 诊断 — 2026-10-16

历史工作区说明：项目复制到 E: 时，没有携带项目本地工具或生成的构建文件。
以下带日期的 LLVM-MinGW 结果描述的是更早的 D: 环境。当前 Windows 构建入口使用
共享 Visual Studio 安装。CMake 3.29.5-msvc4 本身可运行，但默认 SDK 26100 缺少
x64 `gdi32.lib`，配置阶段出现 MSBuild MSB8037。
选择已安装的 SDK 22621 后，使用 MSVC 19.42.34436 配置成功，但编译在系统 SDK 的
`ucrt/float.h` 中因非法字符失败，错误包括 C4828/C2018/C1004。
E: 工作区没有运行项目测试。失败的生成构建目录已移除，也没有向该工作区下载工具链。
共享 Windows SDK 的修复不属于本仓库的源码修改。

## C++11 映射层 — 2026-10-13

使用同一套固定版本的 Windows x64 LLVM-MinGW / Clang 19.1.7、CMake 3.31.6 和
Ninja 1.12.1 工具链，运行了 `scripts/bootstrap-windows.ps1` 和 `scripts/check.ps1`。

| 配置 | 结果 |
| --- | --- |
| Debug，C++11 基础／映射层和 C++17 完整库 | 15/15 项 CTest 测试通过 |
| Release，C++11 基础／映射层和 C++17 完整库 | 15/15 项 CTest 测试通过 |

映射测试在 C++11 和 C++17 下使用同一份源文件运行。覆盖有符号／无符号整数及
float/double/long double 输出、小数结果、截断边界、非有限输入、坐标／结果溢出、
常量与空网格、反向轴、外推和混合类型网格对。原有的全局／分块映射精确相等检查及
独立集合预期验证继续保留。

两个映射头文件也在严格 C++11 模式下独立编译，并跨多个翻译单元链接。
安装／重定位和子项目消费者覆盖 core、transform 和完整库目标，并检查实际语言模式。
验证日志保存在被忽略的 `out/verification-mapper-cxx11.log`。
这些仍是本地参考工具链的证据；旧版编译器、原生 MSVC、Linux/macOS 及托管 CI
尚未验证。

### 浮点示例输出诊断

此前错误的打印值已使用常量 `480.0L` 在库外复现。通过 `cout`、`ostringstream` 和
默认 `printf` 路径直接格式化 long double，在 `-O0` 和 `-O2` 诊断中均产生错误；
转换为 double 后打印为 480。独立检查中，mapper 的 long double 结果相对于
`480.0L` 通过了绝对误差 `1e-10` 的校验。

检查将调用链从 libc++ `num_put<long double>`，经 `snprintf_l`/`vsnprintf`，
追踪到 UCRT `__stdio_common_vsprintf`：扩展精度参数在此遇到 Windows 的 double
格式化约定。启用 MinGW ANSI stdio 修复了诊断程序中的 `printf`，但没有修复预编译
libc++ 流实现。诊断源文件和二进制保存在被忽略的 `out/mapper-diagnostics/`。

示例随后改为先检查原始 long double 值的有限性与精度，再将这个范围明确的值转换成
double 输出。其 CTest 包装同时检查退出状态和确切预期输出：
`Source index 441 maps to target index 480`。
没有修改 mapper 返回类型、计算精度或公共编译选项。工具链通用的 long double 流
格式化限制仍然存在；示例中的处理方式不是通用格式化转换策略。

## C++11 基础层 — 2026-10-13

使用固定版本的 Windows x64 LLVM-MinGW / Clang 19.1.7、CMake 3.31.6 和
Ninja 1.12.1 工具链，先运行 `scripts/bootstrap-windows.ps1`，再运行
`scripts/check.ps1`。

| 配置 | 结果 |
| --- | --- |
| Debug，C++11 基础层和 C++17 完整库 | 13/13 项 CTest 测试通过 |
| Release，C++11 基础层和 C++17 完整库 | 13/13 项 CTest 测试通过 |

同一组 core 和 integer 行为测试在 C++11 与 C++17 下运行。新增覆盖实例化有符号、
无符号及所有标准浮点坐标类型，并检查对齐、最近索引的中点规则、非法容差、非有限
输入，以及无符号最大值与有符号负一的余数区别。

六个基础头文件在严格 C++11 模式下独立编译，同时执行多文件链接检查。
严格测试使用 `-std=c++11`、`-pedantic-errors` 和编译期 `__cplusplus` 检查。
安装／重定位和 `add_subdirectory` 消费者同时覆盖 `linscaler::core`（C++11）及
`linscaler::linscaler`（C++17）。

原有的独立集合预期验证及全局／分块映射测试也通过。输出保存在被忽略的
`out/verification-cxx11.log`。这证明的是参考编译器上的语言兼容性，不代表兼容所有
历史 C++11 编译器。原生 MSVC、Linux/macOS 和托管 CI 仍未验证。

在这一较早阶段，`example_grid_mapping` 的 CTest 退出状态只检查有理数计算结果，
没有检查打印出的浮点索引。错误输出当时尚未解决；参见上文随后进行的映射层诊断及
示例修复。

## 工程基础 — 2026-10-10

日期：2026-10-10（Asia/Shanghai）。

参考工具：Windows x64、LLVM-MinGW 20250114 / Clang 19.1.7、CMake 3.31.6、
Ninja 1.12.1。工具归档与当时 `scripts/toolchain-windows.json` 记录的 SHA-256 值一致。
该下载脚本与清单现已退役；这些是历史结果，不是当前复现步骤。
当时通过运行 `scripts/check.ps1` 复现。

| 配置 | 结果 |
| --- | --- |
| Debug，C++17，将警告视为错误 | 10/10 项 CTest 测试通过 |
| Release，C++17，将警告视为错误，NDEBUG | 10/10 项 CTest 测试通过 |

覆盖原有 28 个测试函数、受检算术和边界回归、与 `std::set` 对照的 354,025 对整数
等差序列、2,401 对二进制分数浮点等差序列、全局／分块有理数映射相等性、浮点坐标
舍入造成的重复值、公共头文件独立编译和 ODR 链接。

每种配置还安装并重定位 CMake 包，构建／运行独立的 `find_package` 消费者，并验证
`add_subdirectory` 默认行为和 C++17 要求传递。故意失败的 CHECK 测试在两种配置下
都符合预期失败契约，证明 Release 下的检查仍有效。

最终本地命令输出保存在被忽略的 `out/verification-final.log`。
Windows 开发可执行文件使用静态运行库链接，以避免依赖工具链 DLL 搜索路径；
这不是库对消费者提出的要求。

此处未验证：托管 CI、原生 MSVC 源码编译、Linux/macOS 运行行为、sanitizer 执行及
性能基准。不能将这些缺失项表述为验证成功。CI 配置已经为未来的远程仓库准备好。
公开发布仍待批准。

许可证更新（2026-10-12）：所有者选择 MIT，并确认版权署名为 Liu Jun。
本地 CMake 配置和安装成功；安装后的 LICENSE 与源文件 SHA-256 一致。
这次仅针对许可证的验证不构成新的数值测试运行。
