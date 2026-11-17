# Choosing a mapping

[中文](mapping.zh-CN.md) · [Documentation](README.md) · [Exact contracts](semantics.md)

## Sequence, coordinate and index

A `linsp_<T>` stores a finite uniform sequence. Its integer element ordinal and its coordinate are different values. For `linsp_<double>(0.25, 0.5, 4)`:

| Index | 0 | 1 | 2 | 3 |
| --- | --- | --- | --- | --- |
| Coordinate | 0.25 | 0.75 | 1.25 | 1.75 |

Continuous index `1.5` denotes a position between two sequence elements. It is not a directly addressable element. Mapping computes numbers; it does not fetch or interpolate stored data.

## Direct conversions

```cpp
#include <linscaler/transform/mapper.hpp>

using linscaler::linsp::linsp_;
using namespace linscaler::transform; // In caller code, not a public header.

linsp_<int> source(10, 6, 5);
linsp_<int> target(4, 4, 10);
auto coordinate = coordinate_at(source, 2);           // 22
auto target_index = fractional_index(target, 22);     // 4.5
auto same_position = map_index_between(source, target, 2); // 4.5
```

Both grids describe the same coordinate system and units. The last call first computes a source coordinate, then a target index. It returns `long double` and permits extrapolation. It does not choose an integer target index or clamp to the target's count.

Do not substitute legacy helpers based on their names:

| Helper | Meaning |
| --- | --- |
| `find_index(grid, value)` | Returns a double index; nonempty constant grids return zero |
| `map_index(grid, index)` | Returns a coordinate of the grid's type; integer coordinates truncate toward zero |
| `map_value(source, target, value)` | Requires source membership and preserves the index, not the coordinate |

In this example, `map_value(source, target, 22)` returns `12`: the source value is at index 2, and the target coordinate at index 2 is 12.

## Compile a floating relationship

For `source(i)=s0+ds*i` and `target(j)=t0+dt*j`, equal coordinates imply:

```text
j = (ds/dt)*i + (s0-t0)/dt
```

```cpp
#include <linscaler/transform/grid_map.hpp>

auto mapping = compile_grid_map<double>(source, target);
auto scale = mapping.scale();   // 1.5
auto offset = mapping.offset(); // 1.5
auto index = mapping(2.0);      // 4.5; same as mapping.evaluate(2.0)
```

Omitting `<double>` selects `long double`. Source and target coordinate types may differ. A compiled map owns its coefficients, not the grids. It has no count or clipping policy. A constant source is allowed; an empty source/target or constant target is rejected.

The floating value type can also be used without grids:

```cpp
#include <linscaler/transform/floating_affine_map.hpp>

using namespace linscaler::transform;

floating_affine_map<double> first(2.0, 3.0);   // 2*x + 3
floating_affine_map<double> second(-1.0, 10.0); // -x + 10
auto combined = compose(first, second); // second(first(x)) = -2*x + 7
auto value = combined(2.0);             // 3
auto backwards = inverse(first);       // 0.5*x - 1.5
auto original = backwards(7.0);         // 2
```

These are unquantized relationships. A chain that rounds or clamps between stages generally cannot be replaced by the composed map. Evaluation and composition offsets use FMA. Compiling coefficients changes the floating evaluation path, sometimes substantially: with binary64 origin `2^53`, unit spacing and identical grids, staged conversion at index 1 can return 0 while the compiled identity returns 1. Read the contracts before replacing an existing path; there is no automatic promise of identical rounded results or higher performance.

## Preserve a known rational relationship

```cpp
#include <linscaler/transform/rational_affine_map.hpp>

using linscaler::transform::rational_affine_map;

// Conversion between 44,100 and 48,000 uniform samples per time unit.
rational_affine_map rate(160, 147); // (160*i + 0)/147
auto position = rate(441);
// position.floor == 480, remainder == 0, denominator == 147

auto block = rate.shifted_input(4096);
// block(local) and rate(4096 + local) have identical quotient/remainder
// whenever both paths' checked integer intermediates are representable.
```

Constructor order is **numerator, denominator, offset**. The offset is part of the numerator: `(a*i+b)/d`, not `a*i/d+b`. In the earlier grid example, `rational_affine_map(3,2,3)` gives `4 + 1/2` at index 2. No conversion through floating-point is needed.

The result's `floor`, `remainder` and `denominator` retain the exact representation. `ceil()` and `nearest()` make explicit integer selections; nearest ties choose the higher integer, including for negative values. `value()` is only a floating approximation. Arithmetic is checked int64, not arbitrary precision. No automatic ratio reduction or inference from floating calibration values occurs.

The old include `<linscaler/transform/rational_index_map.hpp>` and type `rational_index_map` remain available as compatibility spellings. New code can use the canonical name without changing argument order or rounding rules.

## Integration

All mapping headers above support C++11 and can be included individually. With CMake, link `linscaler::transform`; the dependency on `linscaler::core` is transitive. Use `linscaler::linscaler` and `<linscaler/linscaler.hpp>` for the complete C++17 library. `core.hpp` intentionally does not include mapping.

Return values and standard exceptions are the public error interface. The caller chooses numerical representation, units, quantization and domain-specific error handling. Exact differences in integers, floating values and quotient/remainder representations must not be hidden by an application-independent epsilon.
