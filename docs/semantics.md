# Mathematical contracts

[中文](semantics.zh-CN.md) · [Documentation](README.md) · [Mapping guide](mapping.md)

These contracts describe the current development tree. Changes from the original
prototype are listed in CHANGELOG.md. The core operates on one-dimensional grids;
independent axes can be composed by the caller.

## Language boundary

The C++11 basic layer consists of core.hpp, version.hpp, basic/integer.hpp,
basic/precision.hpp, core/linsp.hpp and core/linsp_io.hpp. Its CMake target is
linscaler::core. Existing numeric contracts apply in both C++11 and C++17.
The mapping headers transform/mapper.hpp, transform/floating_affine_map.hpp,
transform/grid_map.hpp, transform/rational_affine_map.hpp and the compatibility
header transform/rational_index_map.hpp also support C++11 via linscaler::transform,
which depends on linscaler::core. Include
mapping explicitly; core.hpp stays independent of transforms. The full entry point
and target, including set operations, remain C++17.
The directory name core/ does not imply all its headers are C++11: linsp_ops.hpp
uses C++17 optional and is outside the basic layer.

precision::default_epsilon is a constexpr value constant with internal linkage.
Its value is 1e-6; its address is not a shared identity across translation units.

## Finite progression

`linsp::linsp_<T>(first, step, count)` represents
`x(i) = first + i * step`, for `0 <= i < count`.
Only the default constructor creates an empty progression; an explicit zero count
throws invalid_argument. A zero step is permitted and is exactly zero, not
"within an epsilon of zero". Negative steps preserve traversal direction.
The last point is included.

Coordinate types: standard floating-point types, signed integral types through int64_t,
and unsigned integral types with at most 63 value bits (for example uint32_t).
bool and uint64_t are rejected at compile time. Counts must fit ptrdiff_t.

Integer calculations use checked int64_t intermediates. They can throw overflow_error
even when an alternative wider rearrangement could represent the final answer.
This is a deliberate bounded-arithmetic contract, not arbitrary-precision arithmetic.
Floating inputs and constructed endpoints must be finite. Very large origins and
small spacings may round distinct mathematical points to the same floating value;
use integer or rational representations when exact distinctions matter.

`last()` on empty throws out_of_range. `first()` on empty returns the canonical
stored zero; it is not an element. Indexed access is checked.
`subset(start, count)` uses a half-open index range [start, start+count), requires
positive count, and avoids unsigned overflow in validation.
`nearest(value)` clamps to the sequence, breaks ties toward the higher sequence
index, and returns zero for an empty or constant sequence for legacy compatibility.
The caller must inspect empty() before treating that result as a valid index.

Equality compares stored first, step and count exactly. It is structural equality,
not set equality; duplicate counts and singleton step metadata matter.
Use explicit approximate predicates for floating-point comparisons.
Iterators include owner identity. Subtraction/ordering across unrelated owners throws
invalid_argument, and traversal outside [begin,end] throws out_of_range.

## Floating tolerance

precision::is_approx_equal uses absolute tolerance (default 1e-6), rejects negative
or non-finite tolerance, and reports false for non-finite operands.
precision::is_aligned measures the remainder against an absolute tolerance and does
not classify tiny nonzero steps as zero.
contains(value, epsilon) checks the nearest integer index and its coordinate residual.
For nonconstant grids epsilon is capped at one quarter of the step magnitude, so
half-step positions are not accepted just because the grid is fine.
parallel_to retains the original approximate-step comparison; it is a query, not a
proof that two floating grids are mathematically identical.

## Set operations

Set operations collapse duplicates and return ascending progressions. Singletons
have zero step. Integral operations are exact within the checked arithmetic domain.
Intersection uses an extended-GCD congruence solution. Union uses a candidate lattice
and verifies its cardinality against the actual union; it never fills missing points.

try_intersect and try_unite return optional<linsp_<T>>:
- engaged empty progression: the mathematical set is empty;
- engaged nonempty progression: representable result;
- nullopt: selected points cannot be represented by one progression.

intersect and unite throw domain_error for a nonrepresentable result.
Arithmetic overflow is an exception, never an empty result.
Normalizing a descending integer progression can fail when its positive step cannot
fit T; this limitation is explicit rather than silently wrapping.

Integral intersection/union cost O(log(max step)) arithmetic operations and O(1) storage.
Floating set operations merge ascending samples, O(count_a + count_b) time and O(1)
extra storage. Matching uses the default absolute tolerance capped by one quarter of
each nonzero input step; matched values use the smaller coordinate symmetrically.
The result must match one arithmetic progression within that tolerance.
Floating matching is approximate and is not a substitute for exact set algebra.
There is intentionally no claimed constant-time arbitrary floating-grid intersection.

## Coordinate versus index mapping

coordinate_at(grid, index) computes a continuous coordinate as long double.
fractional_index(grid, coordinate) computes its continuous inverse; empty and
constant grids have no usable inverse and throw invalid_argument.
map_index_between(source, target, source_index) preserves the physical coordinate.
Both grids must use the same physical coordinate system and units; no unit inference occurs.
These operations allow extrapolation. They do not clamp or interpolate data.
All current mapper functions take O(1) arithmetic operations and O(1) extra storage.
The continuous mappings reject non-finite queries and invalid domains with
invalid_argument; non-finite results throw overflow_error. The C++11 and C++17
builds share the same formulas and exception checks.

Legacy helpers:
- find_index returns double and maps a nonempty constant grid to zero.
- map_index returns T, truncating the final coordinate toward zero for integer T.
- map_value requires membership in the source and preserves index, not physical coordinate;
  target extrapolation is allowed. Nonmembers (including non-finite values) throw
  out_of_range. map_index throws overflow_error if its output cannot fit T after
  integer truncation, where applicable.

Floating conversions of large integer coordinates can lose precision, especially
where long double is the same as double (MSVC). Use rational_affine_map for exact
integer index relationships.

## Exact rational affine mapping

rational_affine_map(a, d, b) represents (a*i + b)/d, with signed int64_t coefficients,
signed integer i and strictly positive d. Negative and zero a are allowed.
Its result is floor + remainder/d, where 0 <= remainder < d.
ceil() and nearest() use checked arithmetic; nearest ties go to the higher integer.
value() is a convenience floating approximation, not the exact representation.

shifted_input(begin)(local) has the same floor/remainder as map(begin+local),
whenever both evaluations' checked intermediates are representable.
It carries the global phase into a block and does not accumulate floating increments.
No automatic ratio reduction is performed; callers can reduce coefficients first to
increase the representable domain.

Construction with a nonpositive denominator throws `std::invalid_argument`.
Unrepresentable checked integer operations, including those in `ceil()`,
`nearest()` or `shifted_input()`, throw `std::overflow_error`. Construction,
evaluation and block shifting each use O(1) arithmetic and O(1) storage, with no
normal-path allocation. This does not provide unbounded-precision arithmetic.

`rational_index_map` is a C++11 type alias of `rational_affine_map`. The old
`transform/rational_index_map.hpp` includes the canonical header and additionally
exposes the old alias. Including the new header alone exposes the canonical name.
Both names denote the same type and retain constructor order
`(numerator, denominator, offset=0)`, accessors and numerical behavior.
The rename does not add rational input arguments,
automatic reduction, inverse or composition.

Rational normalization/composition, generic rounding policies, index intervals,
input-range queries and compensated arithmetic remain future candidates. No unit
types or multidimensional storage layouts are introduced.

## Floating affine mappings

`transform/floating_affine_map.hpp` defines `floating_affine_map<Real>` for
`float`, `double` and `long double`. The constructor takes `(scale, offset)` and
stores `f(x) = scale*x + offset`; both coefficients must be finite. Zero scale is
a valid constant map, with no epsilon-based test for zero. The type has no grid,
count, clipping rule or unit identity. Its header does not depend on `linsp.hpp`.

`scale()` and `offset()` expose the stored coefficients. `evaluate(x)` and
`operator()(x)` accept a finite `Real` argument and evaluate
`std::fma(scale, x, offset)`. This is a fused multiply-add with one final rounding,
not an exact real-number representation. The checks apply to arguments after
conversion to `Real`; callers must choose their input types deliberately. The
constructor and evaluation functions are not checked casts from arbitrary input
types: argument conversion happens before either function can validate it.

Free `compose(first, second)` accepts maps of the same `Real` type and returns
`second(first(x))` algebraically. For `first(x)=a*x+b` and `second(x)=c*x+d`, the
new scale is `c*a` evaluated in `Real`, and the new offset is `std::fma(c,b,d)`.
Free `inverse(map)` requires an exactly nonzero scale and computes coefficients
`1/scale` and `-offset/scale` in `Real`. Very small nonzero scales are not classified
as singular; their inverse can nevertheless be unrepresentable.

| Situation | Result |
| --- | --- |
| Non-finite constructor coefficient or query argument | `std::invalid_argument` |
| Inverse of an exactly zero scale | `std::invalid_argument` |
| Non-finite computed coefficient or evaluation | `std::overflow_error` |
| A mathematically nonzero scale computed as zero during composition or inversion | `std::underflow_error` |
| Finite subnormal results, and underflow of offsets or evaluated values | Accepted with ordinary floating rounding, including zero |

Operations return values rather than modifying input maps, so failure does not
leave an input partially rewritten. Reusing a map as both composition arguments
is valid. All operations use O(1) arithmetic and storage, with no dynamic allocation
on the normal path. Error construction may allocate.

These contracts assume ordinary floating-point semantics, including non-finite
values and FMA behavior; fast-math or flush-to-zero settings must not be used to
claim the documented guarantees. There is no global tolerance, automatic integer
rounding, exact associativity or cross-platform bitwise-equality promise.

## Compiling two grids

`transform/grid_map.hpp` provides:

```cpp
auto map = linscaler::transform::compile_grid_map(source, target);
// floating_affine_map<long double>
auto compact = linscaler::transform::compile_grid_map<double>(source, target);
// floating_affine_map<double>
```

Source and target coordinate types may differ. Both grids must be nonempty and
the target step must be exactly nonzero; violations throw `std::invalid_argument`.
A constant source is valid and compiles to a constant map. Both grids must use the
same coordinate system and units, a precondition that the library cannot infer.

For `source(i)=s0+ds*i` and `target(j)=t0+dt*j`, compile:

```text
scale  = ds / dt
offset = (s0 - t0) / dt
```

Grid parameters are converted to `long double` before arithmetic. Scale, origin
difference and offset are computed and checked in that type. The origin
difference can overflow even when a different algebraic rearrangement could
represent the final offset; this implementation reports `std::overflow_error`.
It does not silently use another representation or an unbounded arithmetic path.

Computed coefficients must be finite and within the finite range of `Real`
before narrowing. Otherwise compilation throws `std::overflow_error`. A nonzero
source step whose ratio becomes zero in intermediate arithmetic or narrowing
throws `std::underflow_error`; this must not silently become a constant map.
Offset underflow, finite subnormal values and ordinary precision loss are allowed.
`long double` is not assumed to be wider than `double`. Large integer parameters
can lose precision when first converted to `long double`.

Compilation uses O(1) arithmetic and storage without normal-path allocation.
It reads the grids once and returns independent coefficients. Counts do not become
clipping bounds, and evaluation continues to allow extrapolation. No integer
quantization, approximate rational reconstruction or application-specific rule
selection occurs.

## Compiled versus staged floating evaluation

Existing `mapper.hpp` formulas and exception behavior are unchanged. Compilation
is an explicit alternative, not a transparent optimization of those functions.
Different intermediates, coefficient rounding and FMA can change results and the
domain on which all checked operations succeed. Even unquantized composition
need not reproduce successive floating evaluations bit for bit.

The difference need not be a last-bit error. Under binary64 arithmetic, let both
grids have origin `2^53` and step `1`. At index `1`, the staged expression
`((2^53 + 1) - 2^53) / 1` evaluates to `0`, while the compiled coefficients
`scale=1, offset=0` evaluate to `1`. This applies where `long double` has double
precision, including MSVC. Either path can move a result across an integer
quantization boundary. Callers must assess migration against their required
results, not assume equality from algebraic equivalence.

Composition describes the unquantized function relation. It does not preserve a
chain that rounds, clamps or otherwise changes the intermediate output. When the
relationship is explicitly rational and exact distinctions matter, retain the
quotient/remainder representation rather than converting through floating values.


