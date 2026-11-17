#ifndef LINSCALER_TEST_SUPPORT_HPP
#define LINSCALER_TEST_SUPPORT_HPP
#include <cstdlib>
#include <exception>
#include <iostream>

namespace test {
inline void check(bool success, const char* expression, const char* file, int line) {
    if (!success) {
        std::cerr << file << ':' << line << ": CHECK failed: " << expression << '\n';
        std::exit(EXIT_FAILURE);
    }
}
template<typename Exception, typename Function>
void throws(Function function, const char* expression, const char* file, int line) {
    try { function(); }
    catch (const Exception&) { return; }
    catch (const std::exception& error) {
        std::cerr << "Unexpected exception: " << error.what() << '\n';
    }
    check(false, expression, file, line);
}
}
#define CHECK(...) ::test::check(static_cast<bool>((__VA_ARGS__)), #__VA_ARGS__, __FILE__, __LINE__)
#define CHECK_THROWS(type, ...) ::test::throws<type>([&] { (void)(__VA_ARGS__); }, #__VA_ARGS__, __FILE__, __LINE__)
#endif

