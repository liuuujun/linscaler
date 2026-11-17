#ifndef LINSCALER_VERSION_HPP
#define LINSCALER_VERSION_HPP

/**
 * @file version.hpp
 * @brief Library version information.
 *
 * Version encoding: 0xMMmmpp (Major.minor.patch)
 *   e.g., v1.2.3 -> 0x010203
 */

#define LINSCALER_VERSION_MAJOR 0
#define LINSCALER_VERSION_MINOR 1
#define LINSCALER_VERSION_PATCH 0

#define LINSCALER_VERSION \
    ((LINSCALER_VERSION_MAJOR << 16) | \
     (LINSCALER_VERSION_MINOR <<  8) | \
     (LINSCALER_VERSION_PATCH))

/* Stringification helpers */
#define LINSCALER_STRINGIFY(x) #x
#define LINSCALER_TOSTRING(x) LINSCALER_STRINGIFY(x)

#define LINSCALER_VERSION_STRING \
    LINSCALER_TOSTRING(LINSCALER_VERSION_MAJOR) "." \
    LINSCALER_TOSTRING(LINSCALER_VERSION_MINOR) "." \
    LINSCALER_TOSTRING(LINSCALER_VERSION_PATCH)

/**
 * @brief Helper macro for version comparison.
 *
 * Usage:
 *   #if LINSCALER_VERSION_CHECK(1, 2, 0)
 *   // code requiring v1.2.0+
 *   #endif
 */
#define LINSCALER_VERSION_CHECK(major, minor, patch) \
    (LINSCALER_VERSION >= ((major << 16) | (minor << 8) | patch))

namespace linscaler {

constexpr int version_major = LINSCALER_VERSION_MAJOR;
constexpr int version_minor = LINSCALER_VERSION_MINOR;
constexpr int version_patch = LINSCALER_VERSION_PATCH;
constexpr int version = LINSCALER_VERSION;

/**
 * @brief Get version as a human-readable string.
 * @return Version string in "M.m.p" format
 */
inline const char* version_string() {
    return LINSCALER_VERSION_STRING;
}

} // namespace linscaler

#endif // LINSCALER_VERSION_HPP
