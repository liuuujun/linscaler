#ifndef LINSCALER_CORE_LINSP_IO_HPP
#define LINSCALER_CORE_LINSP_IO_HPP

#include "linsp.hpp"

#include <sstream>
#include <ostream>
#include <string>

namespace linscaler {
namespace linsp {

/**
 * @brief Convert a linsp_ to a human-readable string representation.
 * @param ls The linsp_ instance
 * @return Formatted string like "{1, 2, 3, ..., 10}, d=1, n=10"
 */
template<typename T>
inline std::string to_string(const linsp_<T>& ls) {
    std::ostringstream oss;

    switch(ls.count()) {
    case 0:
        return "{}";
    case 1:
        oss << "{" << ls.first() << "}";
        break;
    case 2:
        oss << "{" << ls.first() << ", " << ls.last() << "}";
        break;
    case 3:
        oss << "{" << ls.first() << ", " << ls.first() + ls.step() << ", " << ls.last() << "}";
        break;
    default:
        oss << "{" << ls.first() << ", " << ls.first() + ls.step() << ", ..., " << ls.last() << "}";
        break;
    }

    oss << ", d=" << ls.step()
        << ", n=" << ls.count();
    return oss.str();
}

/**
 * @brief Stream output operator for linsp_
 */
template<typename T>
std::ostream& operator<<(std::ostream& os, const linsp_<T>& ls) {
    os << to_string(ls);
    return os;
}

} // namespace linsp
} // namespace linscaler

#endif // LINSCALER_CORE_LINSP_IO_HPP
