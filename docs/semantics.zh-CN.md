# 数学契约

[English](semantics.md) · [文档导航](README.zh-CN.md) · [映射指南](mapping.zh-CN.md)

本文描述当前开发树的行为。相对于原型的变更见 [变更记录](../CHANGELOG.zh-CN.md)。核心处理一维网格，调用方可以组合相互独立的轴。

## 语言标准边界

C++11 基础层包含 `core.hpp`、`version.hpp`、`basic/integer.hpp`、`basic/precision.hpp`、`core/linsp.hpp` 和 `core/linsp_io.hpp`，CMake 目标为 `linscaler::core`。既有数值契约在 C++11 和 C++17 下均适用。

`transform/mapper.hpp`、`transform/floating_affine_map.hpp`、`transform/grid_map.hpp`、`transform/rational_affine_map.hpp` 及兼容头文件 `transform/rational_index_map.hpp` 同样支持 C++11，通过依赖基础层的 `linscaler::transform` 使用。映射需要显式包含，`core.hpp` 保持独立。完整入口及目标包含集合运算，仍要求 C++17。目录名 `core/` 不代表所有头文件都是 C++11：`linsp_ops.hpp` 使用 C++17 的 `optional`，不属于基础层。

`precision::default_epsilon` 是具有内部链接的 `constexpr` 值常量，值为 `1e-6`。它的地址不代表跨翻译单元共享的身份。

## 有限等差序列

`linsp::linsp_<T>(first, step, count)` 表示 `x(i) = first + i * step`，其中 `0 <= i < count`。

只有默认构造产生空序列；显式传入零 count 抛出 `invalid_argument`。允许零步长，按精确零判断，不使用 epsilon 将小步长视为零。负步长保留遍历方向，最后一个点包含在序列内。

坐标类型支持标准浮点类型、不超过 int64_t 的有符号整数，以及有效位数不超过 63 的无符号整数（例如 uint32_t）。bool 和 uint64_t 在编译期被拒绝。count 必须能由 ptrdiff_t 表示。

整数运算使用受检 int64_t 中间结果。即使更宽的运算或另一种重排能表示最终结果，当前运算仍可能抛出 `overflow_error`；这是有界算术契约，不是任意精度运算。浮点输入及构造后的端点必须有限。大原点叠加小步长可能使不同数学点舍入到同一浮点值；需要保留精确区别时，应选择整数或有理数表示。

空序列的 `last()` 抛出 `out_of_range`，`first()` 返回规范存储值零，但该零不是元素。索引访问执行边界检查。`subset(start,count)` 使用半开索引范围 `[start,start+count)`，要求 count 为正，并在验证中避免无符号溢出。

`nearest(value)` 将结果钳制到序列范围，中点选择较大的序列索引。为了兼容既有行为，空序列和常量序列返回零；调用方必须先检查 `empty()`，不能直接把它视为有效索引。

相等运算精确比较保存的 first、step 和 count，属于结构相等，不是集合相等；重复元素数量及单点序列的步长元数据都有意义。浮点近似比较应显式使用相应谓词。

迭代器包含所属序列的身份。不同所属对象的迭代器执行减法或顺序比较时抛出 `invalid_argument`；越过 `[begin,end]` 遍历时抛出 `out_of_range`。

## 浮点容差

`precision::is_approx_equal` 使用绝对容差（默认 `1e-6`），拒绝负数或非有限容差，对非有限操作数返回 false。`precision::is_aligned` 比较余数与绝对容差，不将微小非零步长视为零。

`contains(value,epsilon)` 检查最近整数索引及其坐标残差。对于非常量网格，有效 epsilon 不超过步长绝对值的四分之一，避免细网格仅因默认容差较大就接受半步位置。`parallel_to` 保留原有近似步长比较，它是查询，不是两条浮点网格在数学上完全相同的证明。

## 集合运算

集合运算去除重复点，返回升序等差序列；单点结果的步长为零。整数运算在受检算术域内精确。交集使用扩展 GCD 同余解；并集通过候选格点及实际并集基数校验，不填补缺失点。

`try_intersect` 和 `try_unite` 返回 `optional<linsp_<T>>`：

- 有值且包含空序列：数学结果为空集。
- 有值且包含非空序列：结果可以表示。
- `nullopt`：结果无法由一个等差序列表示。

`intersect` 和 `unite` 对不可表示结果抛出 `domain_error`。算术溢出始终是异常，不会伪装为空集。降序整数序列归一化为升序时，如果正步长无法由 T 表示，也会明确失败。

整数交并集需要 `O(log(max step))` 次算术操作和 `O(1)` 存储。浮点集合运算合并升序样本，时间为 `O(count_a + count_b)`，额外存储为 `O(1)`。匹配使用默认绝对容差，并限制为各非零输入步长绝对值的四分之一；匹配的两个值对称地采用较小坐标。结果必须在该容差下符合单个等差序列。浮点匹配是近似过程，不替代精确集合代数，也不承诺任意浮点网格交集具有常数时间算法。

## 坐标与索引转换

`coordinate_at(grid,index)` 返回 long double 连续坐标。`fractional_index(grid,coordinate)` 计算连续逆关系；空网格和常量网格没有可用的唯一逆，抛出 `invalid_argument`。`map_index_between(source,target,source_index)` 保持物理坐标不变。两条网格必须使用相同坐标系及单位，库不推断单位。

这些操作允许外推，不钳制范围，也不插值数据。现有 mapper 函数均需要 `O(1)` 次算术操作和 `O(1)` 额外存储。连续接口对非有限查询和非法定义域抛出 `invalid_argument`，对非有限结果抛出 `overflow_error`。C++11 与 C++17 使用相同公式及检查。

历史兼容接口：

- `find_index` 返回 double，并将非空常量网格映射到零。
- `map_index` 返回 T；当 T 为整数时，对最终完整坐标向零截断。
- `map_value` 要求输入属于源网格，保持索引而非物理坐标；允许目标外推。不属于源网格的值（包括非有限值）抛出 `out_of_range`。`map_index` 在完成适用的整数截断后，如果结果无法由 T 表示则抛出 `overflow_error`。

大整数坐标转为浮点时可能丢失精度，尤其是在 long double 与 double 同精度的平台上（例如 MSVC）。精确整数索引关系应使用 `rational_affine_map`。

## 精确有理数仿射映射

`rational_affine_map(a,d,b)` 表示 `(a*i+b)/d`，系数和输入 i 为有符号 int64_t，d 严格为正。允许负数或零 a。结果表示为 `floor + remainder/d`，其中 `0 <= remainder < d`。

`ceil()` 和 `nearest()` 使用受检算术；`nearest()` 的中点选择较大的整数。`value()` 仅提供浮点近似，不是精确表示本身。

当两条路径各自的受检中间结果均可表示时，`shifted_input(begin)(local)` 与 `map(begin+local)` 的 floor/remainder 相同。它将全局相位带入局部块，不累加浮点增量。不自动约分；调用方可以先约分系数以扩大可表示输入域。

构造时分母非正会抛出 `std::invalid_argument`。受检整数运算无法表示结果时抛出 `std::overflow_error`，包括 `ceil()`、`nearest()` 和 `shifted_input()` 中的运算。构造、求值和分块平移均使用 O(1) 次算术操作与 O(1) 存储，正常路径不进行动态分配；不提供任意精度运算。

`rational_index_map` 是 `rational_affine_map` 的 C++11 类型别名，旧头文件 `transform/rational_index_map.hpp` 包含正式头文件并额外提供旧别名。仅包含新头文件时提供正式名称。两个名称指向相同类型，保留构造顺序 `(numerator,denominator,offset=0)`、访问器和数值行为。重命名不增加有理数输入参数、自动约分、求逆或复合。

有理数正规化／复合、通用舍入策略、索引区间、输入范围反查和补偿计算仍为未来候选能力。当前不引入单位类型或多维存储布局。

## 浮点仿射映射

`transform/floating_affine_map.hpp` 为 float、double 和 long double 定义 `floating_affine_map<Real>`。构造参数为 `(scale,offset)`，保存 `f(x)=scale*x+offset`，两个系数必须有限。零尺度是合法常量映射，不使用 epsilon 判零。该类型不保存网格、数量、裁剪规则或单位身份，头文件也不依赖 `linsp.hpp`。

`scale()` 和 `offset()` 返回保存的系数。`evaluate(x)` 和 `operator()(x)` 接受有限的 Real 参数，通过 `std::fma(scale,x,offset)` 求值。这是仅在最终结果舍入一次的融合乘加，不是精确实数表示。检查针对已转换为 Real 的参数；调用方应明确选择输入类型。构造函数和求值函数不是任意输入类型的受检转换接口：参数转换发生在函数能够检查参数之前。

自由函数 `compose(first,second)` 接受相同 Real 类型的两个映射，在代数上表示 `second(first(x))`。若 `first(x)=a*x+b`、`second(x)=c*x+d`，新尺度为在 Real 中计算的 `c*a`，新偏移为 `std::fma(c,b,d)`。自由函数 `inverse(map)` 要求尺度精确非零，在 Real 中计算 `1/scale` 和 `-offset/scale`。微小非零尺度不被视为奇异，但其逆系数可能不可表示。

| 情况 | 结果 |
| --- | --- |
| 非有限构造系数或查询参数 | `std::invalid_argument` |
| 对精确零尺度求逆 | `std::invalid_argument` |
| 计算出的系数或求值结果非有限 | `std::overflow_error` |
| 复合或求逆时，数学上非零的尺度计算成零 | `std::underflow_error` |
| 有限次正规结果，以及偏移或求值结果下溢 | 接受普通浮点舍入，包括舍入为零 |

操作返回新值，不修改输入映射，所以失败不会留下被部分改写的输入。同一个映射可以同时作为两个复合参数。各操作均使用 `O(1)` 次算术操作和存储，正常路径没有动态分配；构造异常对象可能分配内存。

这些契约以普通浮点语义为前提，包括非有限值和 FMA 行为；不能在 fast-math 或将次正规数直接归零的设置下宣称这些保证仍然成立。不提供全局容差、自动整数取整、精确结合律或跨平台逐位一致承诺。

## 从两条网格编译映射

`transform/grid_map.hpp` 提供：

```cpp
auto map = linscaler::transform::compile_grid_map(source, target);
// floating_affine_map<long double>
auto compact = linscaler::transform::compile_grid_map<double>(source, target);
// floating_affine_map<double>
```

源和目标的坐标类型可以不同。两条网格必须非空，目标步长必须精确非零，否则抛出 `std::invalid_argument`。常量源网格合法，编译为常量映射。两条网格必须使用相同坐标系和单位，这个前提不能由库自动推断。

对于 `source(i)=s0+ds*i` 和 `target(j)=t0+dt*j`，编译：

```text
scale  = ds / dt
offset = (s0 - t0) / dt
```

网格参数先转换为 long double，再计算和检查尺度、原点差及偏移。原点差本身可能溢出，即使另一种代数重排能表示最终偏移，当前实现也会抛出 `std::overflow_error`，不静默切换表示或使用无界算术。

收窄前要求计算出的系数有限且位于 Real 的有限表示范围内，否则抛出 `std::overflow_error`。非零源步长的比例在中间运算或收窄时变成零，将抛出 `std::underflow_error`，避免静默变成常量映射。偏移下溢、有限次正规值和普通精度损失允许存在。不假设 long double 比 double 更宽；大整数参数第一次转换为 long double 时就可能失去精度。

编译需要 `O(1)` 次算术操作和存储，正常路径没有动态分配。它读取网格参数并返回独立系数，count 不会成为裁剪边界，后续求值仍允许外推。不进行整数取整、有理数近似重建或应用规则选择。

## 编译求值与逐步浮点求值

既有 `mapper.hpp` 的公式和异常行为保持不变。编译是显式的另一条计算路径，不是对旧函数的透明优化。不同的中间值、系数舍入及 FMA 会改变结果，也可能改变所有受检运算都能成功的输入域。即使没有量化，代数复合也不保证与逐次浮点求值逐位相同。

差异不一定只是末位误差。使用 binary64 算术时，若两条网格的原点均为 `2^53`、步长均为 `1`，在索引 `1` 处，逐步表达式 `((2^53+1)-2^53)/1` 得到 `0`，而编译系数 `scale=1,offset=0` 得到 `1`。这适用于 long double 与 double 同精度的平台，包括 MSVC。任一路径变化都可能跨越整数取整边界。迁移时必须按调用方所需结果验证，不能从代数等价推断结果一致。

复合描述未量化函数关系，不能保留中间进行舍入、钳制或其他改写的计算链。当关系具有明确的有理数表示且精确区别很重要时，应保留商余数，不要先转换为浮点。
