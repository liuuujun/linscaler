# 如何选择映射接口

[English](mapping.md) · [文档导航](README.zh-CN.md) · [精确契约](semantics.zh-CN.md)

## 序列、坐标与索引

`linsp_<T>` 保存有限均匀序列。整数元素序号与它的坐标是不同的数值。例如 `linsp_<double>(0.25,0.5,4)`：

| 索引 | 0 | 1 | 2 | 3 |
| --- | --- | --- | --- | --- |
| 坐标 | 0.25 | 0.75 | 1.25 | 1.75 |

连续索引 `1.5` 表示两个元素之间的位置，不是可以直接访问的元素。映射计算数值，不读取或插值所存储的数据。

## 直接转换

```cpp
#include <linscaler/transform/mapper.hpp>

using linscaler::linsp::linsp_;
using namespace linscaler::transform; // 仅用于调用方代码，不放入公共头文件。

linsp_<int> source(10, 6, 5);
linsp_<int> target(4, 4, 10);
auto coordinate = coordinate_at(source, 2);           // 22
auto target_index = fractional_index(target, 22);     // 4.5
auto same_position = map_index_between(source, target, 2); // 4.5
```

两条网格使用相同坐标系和单位。最后一个调用先计算源坐标，再计算目标索引；返回 long double，允许外推，不选择整数目标索引，也不根据目标 count 裁剪。

不能仅凭名字相似替换历史兼容接口：

| 接口 | 含义 |
| --- | --- |
| `find_index(grid,value)` | 返回 double 索引；非空常量网格返回零 |
| `map_index(grid,index)` | 返回网格坐标类型的值；整数坐标向零截断 |
| `map_value(source,target,value)` | 要求源网格包含该值，保持索引而非坐标 |

本例中，`map_value(source,target,22)` 返回 `12`：源值位于索引 2，目标索引 2 的坐标为 12。

## 编译浮点关系

对于 `source(i)=s0+ds*i` 与 `target(j)=t0+dt*j`，要求坐标相同可以得到：

```text
j = (ds/dt)*i + (s0-t0)/dt
```

```cpp
#include <linscaler/transform/grid_map.hpp>

auto mapping = compile_grid_map<double>(source, target);
auto scale = mapping.scale();   // 1.5
auto offset = mapping.offset(); // 1.5
auto index = mapping(2.0);      // 4.5，等同于 mapping.evaluate(2.0)
```

省略 `<double>` 时采用 long double。源和目标坐标类型可以不同。编译结果持有系数，不持有网格，不携带 count 或裁剪策略。允许常量源网格；空源／目标网格或常量目标网格被拒绝。

浮点映射值类型也可以不依赖网格单独使用：

```cpp
#include <linscaler/transform/floating_affine_map.hpp>

using namespace linscaler::transform;

floating_affine_map<double> first(2.0, 3.0);    // 2*x + 3
floating_affine_map<double> second(-1.0, 10.0); // -x + 10
auto combined = compose(first, second); // second(first(x)) = -2*x + 7
auto value = combined(2.0);             // 3
auto backwards = inverse(first);       // 0.5*x - 1.5
auto original = backwards(7.0);         // 2
```

这些是未量化关系。中间发生舍入或钳制的计算链，通常不能由复合映射替代。求值和复合偏移使用 FMA。编译系数会改变浮点求值路径，差异有时很大：在 binary64 精度下，两条网格都使用原点 `2^53` 和单位步长，索引 1 的逐步转换可能得到 0，而编译后的恒等映射得到 1。替换现有路径前应阅读契约；不能自动承诺最终舍入结果相同或性能更高。

## 保留已知的有理数关系

```cpp
#include <linscaler/transform/rational_affine_map.hpp>

using linscaler::transform::rational_affine_map;

// 每单位时间 44,100 与 48,000 个均匀样本之间的转换。
rational_affine_map rate(160, 147); // (160*i + 0)/147
auto position = rate(441);
// position.floor == 480，remainder == 0，denominator == 147

auto block = rate.shifted_input(4096);
// 当两条路径各自的受检中间结果都可表示时，
// block(local) 与 rate(4096 + local) 的商余数完全相同。
```

构造参数顺序为 **numerator、denominator、offset**。offset 属于分子：`(a*i+b)/d`，不是 `a*i/d+b`。前述网格例子可以使用 `rational_affine_map(3,2,3)`，在索引 2 处得到 `4 + 1/2`，无需经过浮点转换。

结果的 floor、remainder 和 denominator 保留精确表示。`ceil()` 和 `nearest()` 显式选择整数；nearest 的中点取较大整数，负数同样如此。`value()` 仅提供浮点近似。运算采用受检 int64，不是任意精度计算；不自动约分，也不从浮点校准值推断有理数比例。

旧头文件 `<linscaler/transform/rational_index_map.hpp>` 与类型 `rational_index_map` 保留为兼容写法。新代码可以采用正式名称，构造参数顺序和舍入规则不变。

## 接入

上述映射头文件都支持 C++11，可分别包含。使用 CMake 时链接 `linscaler::transform`，它自动依赖 `linscaler::core`。完整 C++17 库使用 `linscaler::linscaler` 和 `<linscaler/linscaler.hpp>`。`core.hpp` 特意保持不包含映射。

返回值和标准异常构成公共错误接口。调用方决定数值表示、单位、量化方式及业务错误处理。整数、浮点值和商余数表示之间的区别，不应被一个与应用无关的 epsilon 隐藏。
