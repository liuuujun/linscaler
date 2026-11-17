#ifndef LINSCALER_TEST_CXX11_MODE_HPP
#define LINSCALER_TEST_CXX11_MODE_HPP

// MSVC does not provide a strict C++11 mode. Its lowest selectable language
// mode is tested separately; GCC/Clang must actually compile as C++11.
#if !defined(_MSC_VER) && __cplusplus != 201103L
#error "Compatibility tests must compile in strict C++11 mode"
#endif

#endif
