#ifndef LINSCALER_HPP
#define LINSCALER_HPP

/**
 * @file linscaler.hpp
 * @brief Unified entry point for the linscaler library.
 *
 * Include this single header to access all linscaler components:
 *   - basic/precision.hpp  : Floating-point tolerance utilities
 *   - basic/integer.hpp    : Checked signed integer arithmetic
 *   - core/linsp.hpp       : Finite uniform sequence (arithmetic progression)
 *   - core/linsp_ops.hpp   : Intersection and union of progressions
 *   - core/linsp_io.hpp    : String/stream output for debugging
 *   - transform/mapper.hpp : Coordinate/index conversion between uniform grids
 *   - transform/floating_affine_map.hpp : Floating affine values and operations
 *   - transform/grid_map.hpp : Compile affine relationships between grids
 *   - transform/rational_affine_map.hpp : Exact rational index relationships
 *   - transform/rational_index_map.hpp : Compatibility name for rational mapping
 */

#include "core.hpp"
#include "core/linsp_ops.hpp"
#include "transform/mapper.hpp"
#include "transform/floating_affine_map.hpp"
#include "transform/grid_map.hpp"
#include "transform/rational_affine_map.hpp"
#include "transform/rational_index_map.hpp"

#endif // LINSCALER_HPP
