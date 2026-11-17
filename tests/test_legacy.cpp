#include "linscaler/version.hpp"
#include "linscaler/basic/precision.hpp"
#include "linscaler/core/linsp.hpp"
#include "linscaler/core/linsp_ops.hpp"
#include "linscaler/core/linsp_io.hpp"
#include "linscaler/transform/mapper.hpp"

#include "test_support.hpp"
#include <iostream>
#include <cmath>
#include <string>

using namespace linscaler;

// ==================== version tests ====================

void test_version_macros() {
    CHECK(LINSCALER_VERSION_MAJOR == 0);
    CHECK(LINSCALER_VERSION_MINOR == 1);
    CHECK(LINSCALER_VERSION_PATCH == 0);
    CHECK(LINSCALER_VERSION == 0x000100);
    CHECK(LINSCALER_VERSION_CHECK(0, 1, 0));
    CHECK(!LINSCALER_VERSION_CHECK(0, 2, 0));
    CHECK(!LINSCALER_VERSION_CHECK(1, 0, 0));
    std::cout << "  [PASS] test_version_macros\n";
}

void test_version_constants() {
    CHECK(version_major == 0);
    CHECK(version_minor == 1);
    CHECK(version_patch == 0);
    CHECK(version == 0x000100);
    std::cout << "  [PASS] test_version_constants\n";
}

void test_version_string() {
    std::string s = version_string();
    CHECK(s == "0.1.0");
    std::cout << "  [PASS] test_version_string (v" << s << ")\n";
}

// ==================== precision tests ====================

void test_is_approx_equal_double() {
    CHECK(precision::is_approx_equal(1.0, 1.0));
    CHECK(precision::is_approx_equal(1.0, 1.0 + 1e-7));
    CHECK(!precision::is_approx_equal(1.0, 1.0 + 1e-4));
    CHECK(precision::is_approx_equal(0.0, 0.0));
    CHECK(precision::is_approx_equal(-1.0, -1.0));
    std::cout << "  [PASS] test_is_approx_equal_double\n";
}

void test_is_approx_equal_int() {
    CHECK(precision::is_approx_equal(1, 1));
    CHECK(!precision::is_approx_equal(1, 2));
    CHECK(precision::is_approx_equal(0, 0));
    CHECK(precision::is_approx_equal(-5, -5));
    std::cout << "  [PASS] test_is_approx_equal_int\n";
}

void test_is_aligned_double() {
    CHECK(precision::is_aligned(6.0, 2.0));
    CHECK(!precision::is_aligned(5.0, 2.0));
    CHECK(precision::is_aligned(0.0, 3.0));
    std::cout << "  [PASS] test_is_aligned_double\n";
}

void test_is_aligned_int() {
    CHECK(precision::is_aligned(6, 2));
    CHECK(!precision::is_aligned(5, 2));
    CHECK(precision::is_aligned(0, 3));
    CHECK(precision::is_aligned(0, 0));
    CHECK(!precision::is_aligned(5, 0));
    std::cout << "  [PASS] test_is_aligned_int\n";
}

// ==================== linsp tests ====================

void test_linsp_basic_double() {
    linsp::linsp_<double> ls(1.0, 2.0, 5);
    CHECK(ls.first() == 1.0);
    CHECK(ls.step() == 2.0);
    CHECK(ls.count() == 5);
    CHECK(precision::is_approx_equal(ls.last(), 9.0));
    CHECK(!ls.empty());
    CHECK(!ls.constant());
    std::cout << "  [PASS] test_linsp_basic_double\n";
}

void test_linsp_basic_int() {
    linsp::linsp_<int> ls(1, 3, 4);
    CHECK(ls.first() == 1);
    CHECK(ls.step() == 3);
    CHECK(ls.count() == 4);
    CHECK(ls.last() == 10);
    CHECK(!ls.empty());
    CHECK(!ls.constant());
    std::cout << "  [PASS] test_linsp_basic_int\n";
}

void test_linsp_default() {
    linsp::linsp_<double> ls;
    CHECK(ls.empty());
    CHECK(ls.count() == 0);
    std::cout << "  [PASS] test_linsp_default\n";
}

void test_linsp_constant() {
    linsp::linsp_<double> ls(5.0, 0.0, 3);
    CHECK(ls.constant());
    CHECK(precision::is_approx_equal(ls[0], 5.0));
    CHECK(precision::is_approx_equal(ls[1], 5.0));
    CHECK(precision::is_approx_equal(ls[2], 5.0));
    std::cout << "  [PASS] test_linsp_constant\n";
}

void test_linsp_parallel() {
    linsp::linsp_<double> a(1.0, 2.0, 5);
    linsp::linsp_<double> b(10.0, 2.0, 3);
    linsp::linsp_<double> c(0.0, 3.0, 4);
    CHECK(a.parallel_to(b));
    CHECK(!a.parallel_to(c));
    std::cout << "  [PASS] test_linsp_parallel\n";
}

void test_linsp_aligned() {
    linsp::linsp_<double> a(0.0, 2.0, 5);
    linsp::linsp_<double> b(4.0, 2.0, 3);
    linsp::linsp_<double> c(1.0, 2.0, 3);
    CHECK(a.aligned_with(b));
    CHECK(!a.aligned_with(c));
    std::cout << "  [PASS] test_linsp_aligned\n";
}

void test_linsp_operator_bracket() {
    linsp::linsp_<int> ls(10, 5, 4);
    CHECK(ls[0] == 10);
    CHECK(ls[1] == 15);
    CHECK(ls[2] == 20);
    CHECK(ls[3] == 25);
    std::cout << "  [PASS] test_linsp_operator_bracket\n";
}

void test_linsp_to_vector() {
    linsp::linsp_<int> ls(1, 2, 4);
    auto v = ls.to_vector();
    CHECK(v.size() == 4);
    CHECK(v[0] == 1);
    CHECK(v[1] == 3);
    CHECK(v[2] == 5);
    CHECK(v[3] == 7);
    std::cout << "  [PASS] test_linsp_to_vector\n";
}

void test_linsp_to_string() {
    linsp::linsp_<int> ls0;
    CHECK(linsp::to_string(ls0) == "{}");

    linsp::linsp_<int> ls1(5, 0, 1);
    CHECK(linsp::to_string(ls1) == "{5}, d=0, n=1");

    linsp::linsp_<int> ls2(1, 2, 2);
    CHECK(linsp::to_string(ls2) == "{1, 3}, d=2, n=2");

    linsp::linsp_<int> ls(1, 2, 5);
    auto s = linsp::to_string(ls);
    CHECK(s.find("1") != std::string::npos);
    CHECK(s.find("d=2") != std::string::npos);
    CHECK(s.find("n=5") != std::string::npos);
    std::cout << "  [PASS] test_linsp_to_string\n";
}

void test_linsp_equality() {
    linsp::linsp_<double> a(1.0, 2.0, 5);
    linsp::linsp_<double> b(1.0, 2.0, 5);
    linsp::linsp_<double> c(1.0, 3.0, 5);
    CHECK(a == b);
    CHECK(!(a == c));
    std::cout << "  [PASS] test_linsp_equality\n";
}

void test_linsp_inequality() {
    linsp::linsp_<int> a(1, 2, 5);
    linsp::linsp_<int> b(1, 2, 5);
    linsp::linsp_<int> c(1, 3, 5);
    linsp::linsp_<int> d(0, 2, 5);
    CHECK(!(a != b));
    CHECK(a != c);
    CHECK(a != d);
    std::cout << "  [PASS] test_linsp_inequality\n";
}

void test_linsp_intersect() {
    linsp::linsp_<int> a(0, 2, 6);  // 0,2,4,6,8,10
    linsp::linsp_<int> b(0, 3, 4);  // 0,3,6,9
    auto r = linsp::intersect(a, b);
    CHECK(r.count() == 2);
    CHECK(r[0] == 0);
    CHECK(r[1] == 6);
    std::cout << "  [PASS] test_linsp_intersect\n";
}

void test_linsp_negative_step() {
    linsp::linsp_<int> ls(10, -2, 5);
    CHECK(ls.first() == 10);
    CHECK(ls.last() == 2);
    CHECK(ls[0] == 10);
    CHECK(ls[4] == 2);
    std::cout << "  [PASS] test_linsp_negative_step\n";
}

void test_linsp_exception() {
    bool caught = false;
    try {
        linsp::linsp_<int>(0, 1, 0);
    } catch (const std::invalid_argument&) {
        caught = true;
    }
    CHECK(caught);

    caught = false;
    try {
        linsp::linsp_<int>(0, 1, 3)[5];
    } catch (const std::out_of_range&) {
        caught = true;
    }
    CHECK(caught);
    std::cout << "  [PASS] test_linsp_exception\n";
}

void test_linsp_contains() {
    linsp::linsp_<int> ls(0, 3, 5);  // 0,3,6,9,12
    CHECK(ls.contains(0));
    CHECK(ls.contains(3));
    CHECK(ls.contains(6));
    CHECK(ls.contains(9));
    CHECK(ls.contains(12));
    CHECK(!ls.contains(1));
    CHECK(!ls.contains(2));
    CHECK(!ls.contains(5));
    CHECK(!ls.contains(15));
    CHECK(!ls.contains(-3));

    // Empty sequence contains nothing
    linsp::linsp_<int> empty;
    CHECK(!empty.contains(0));

    // Constant sequence
    linsp::linsp_<double> cs(5.0, 0.0, 3);
    CHECK(cs.contains(5.0));
    CHECK(!cs.contains(6.0));
    std::cout << "  [PASS] test_linsp_contains\n";
}

void test_linsp_nearest() {
    linsp::linsp_<int> ls(0, 5, 5);  // 0,5,10,15,20

    CHECK(ls.nearest(0)  == 0);
    CHECK(ls.nearest(2)  == 0);   // closer to 0 than 5
    CHECK(ls.nearest(3)  == 1);   // closer to 5 than 0
    CHECK(ls.nearest(5)  == 1);
    CHECK(ls.nearest(7)  == 1);   // closer to 5 than 10
    CHECK(ls.nearest(8)  == 2);   // closer to 10 than 5
    CHECK(ls.nearest(10) == 2);
    CHECK(ls.nearest(20) == 4);
    CHECK(ls.nearest(22) == 4);   // clamped to last
    CHECK(ls.nearest(-3) == 0);   // clamped to first

    // Empty returns 0
    linsp::linsp_<int> empty;
    CHECK(empty.nearest(42) == 0);

    // Single element
    linsp::linsp_<double> single(7.0, 0.0, 1);
    CHECK(single.nearest(100.0) == 0);

    std::cout << "  [PASS] test_linsp_nearest\n";
}

void test_linsp_subset() {
    linsp::linsp_<int> ls(0, 2, 10);  // 0,2,4,...,18

    auto sub = ls.subset(3, 4);  // start at index 3, take 4 elements
    CHECK(sub.count() == 4);
    CHECK(sub.first() == 6);   // ls[3] = 6
    CHECK(sub.step() == 2);
    CHECK(sub.last() == 12);   // ls[6] = 12
    CHECK(sub[0] == 6);
    CHECK(sub[3] == 12);

    // Subset at start
    auto sub_start = ls.subset(0, 1);
    CHECK(sub_start.count() == 1);
    CHECK(sub_start.first() == 0);

    // Subset at end
    auto sub_end = ls.subset(9, 1);
    CHECK(sub_end.count() == 1);
    CHECK(sub_end.first() == 18);

    // Exception on invalid
    bool caught = false;
    try { ls.subset(8, 3); }  // 8+3 > 10
    catch (const std::out_of_range&) { caught = true; }
    CHECK(caught);

    caught = false;
    try { ls.subset(10, 1); }
    catch (const std::out_of_range&) { caught = true; }
    CHECK(caught);

    caught = false;
    try { ls.subset(0, 0); }
    catch (const std::invalid_argument&) { caught = true; }
    CHECK(caught);

    std::cout << "  [PASS] test_linsp_subset\n";
}

void test_linsp_iteration() {
    linsp::linsp_<int> ls(1, 2, 4);  // 1,3,5,7

    // range-for
    int expected[] = {1, 3, 5, 7};
    size_t i = 0;
    for (auto v : ls) {
        CHECK(v == expected[i]);
        ++i;
    }
    CHECK(i == 4);

    // iterator arithmetic
    auto it = ls.begin();
    CHECK(*it == 1);
    CHECK(*(it + 1) == 3);
    CHECK(*(it + 2) == 5);
    CHECK(*(it + 3) == 7);

    // iterator difference
    CHECK(ls.end() - ls.begin() == 4);

    // distance
    CHECK(std::distance(ls.begin(), ls.end()) == 4);

    std::cout << "  [PASS] test_linsp_iteration\n";
}

// ==================== mapper tests ====================

void test_find_index() {
    // Integer grid
    linsp::linsp_<int> grid(0, 5, 5);
    CHECK(transform::find_index(grid, 0)   == 0.0);
    CHECK(transform::find_index(grid, 5)   == 1.0);
    CHECK(transform::find_index(grid, 10)  == 2.0);
    CHECK(transform::find_index(grid, 2)   == 0.4);  // fractional
    CHECK(transform::find_index(grid, 7)   == 1.4);
    CHECK(transform::find_index(grid, -5)  == -1.0); // outside range, but still valid math

    double _6 = 6.0 / 600.0;
    double _3 = 3.0 / 600.0;

    // DPI grid (600 DPI: step = 1/600)
    linsp::linsp_<double> dpi600(0.0, 1.0 / 600.0, 100);
    CHECK(std::abs(transform::find_index(dpi600, 0.0)) < 1e-12);
    CHECK(std::abs(transform::find_index(dpi600, _6) - 6.0) < 1e-12);
    CHECK(std::abs(transform::find_index(dpi600, _3) - 3.0) < 1e-12);

    // Constant grid
    linsp::linsp_<int> constant(5, 0, 3);
    CHECK(transform::find_index(constant, 5) == 0.0);

    std::cout << "  [PASS] test_find_index\n";
}

void test_map_index() {
    // Integer grid
    linsp::linsp_<int> grid(10, 5, 5);  // 10,15,20,25,30
    CHECK(transform::map_index(grid, 0.0) == 10);
    CHECK(transform::map_index(grid, 1.0) == 15);
    CHECK(transform::map_index(grid, 2.0) == 20);
    CHECK(transform::map_index(grid, 1.5) == 17);  // 10 + 1.5 * 5 = 17.5 -> truncates to 17

    // DPI grid (600 DPI)
    linsp::linsp_<double> dpi600(0.0, 1.0 / 600.0, 100);
    CHECK(std::abs(transform::map_index(dpi600, 6.0) - 6.0 / 600.0) < 1e-12);
    CHECK(std::abs(transform::map_index(dpi600, 0.5) - 0.5 / 600.0) < 1e-12);

    std::cout << "  [PASS] test_map_index\n";
}

void test_map_value() {
    // 300 DPI -> 600 DPI mapping
    // 300 DPI grid: step = 1/300, positions at 0, 1/300, 2/300, ...
    // 600 DPI grid: step = 1/600, positions at 0, 1/600, 2/600, ...
    linsp::linsp_<double> grid300(0.0, 1.0 / 300.0, 10);
    linsp::linsp_<double> grid600(0.0, 1.0 / 600.0, 20);

    // Value 0 in 300DPI -> 0 in 600DPI
    CHECK(std::abs(transform::map_value(grid300, grid600, 0.0)) < 1e-12);

    // Value 1/300 in 300DPI -> same index (1) in 600DPI = 1/600
    double expected = 1.0 / 600.0;
    CHECK(std::abs(transform::map_value(grid300, grid600, 1.0 / 300.0) - expected) < 1e-12);

    // Value 1/600 is NOT on the 300DPI grid (halfway between index 0 and 1)
    // map_value should throw out_of_range
    bool caught = false;
    try {
        transform::map_value(grid300, grid600, 1.0 / 600.0);
    } catch (const std::out_of_range&) {
        caught = true;
    }
    CHECK(caught);

    // Integer grid: 0,2,4,6,8  ->  0,1,2,3,4
    linsp::linsp_<int> coarse(0, 2, 5);  // 0,2,4,6,8
    linsp::linsp_<int> fine(0, 1, 9);    // 0,1,2,3,4,5,6,7,8

    CHECK(transform::map_value(coarse, fine, 0) == 0);
    CHECK(transform::map_value(coarse, fine, 2) == 1);
    CHECK(transform::map_value(coarse, fine, 4) == 2);
    CHECK(transform::map_value(coarse, fine, 6) == 3);
    CHECK(transform::map_value(coarse, fine, 8) == 4);

    // Value outside range should throw
    caught = false;
    try {
        transform::map_value(coarse, fine, 10);
    } catch (const std::out_of_range&) {
        caught = true;
    }
    CHECK(caught);

    caught = false;
    try {
        transform::map_value(coarse, fine, -2);
    } catch (const std::out_of_range&) {
        caught = true;
    }
    CHECK(caught);

    std::cout << "  [PASS] test_map_value\n";
}

// ==================== main ====================

int main() {
    std::cout << "=== version tests ===\n";
    test_version_macros();
    test_version_constants();
    test_version_string();

    std::cout << "\n=== precision tests ===\n";
    test_is_approx_equal_double();
    test_is_approx_equal_int();
    test_is_aligned_double();
    test_is_aligned_int();

    std::cout << "\n=== linsp tests ===\n";
    test_linsp_basic_double();
    test_linsp_basic_int();
    test_linsp_default();
    test_linsp_constant();
    test_linsp_parallel();
    test_linsp_aligned();
    test_linsp_operator_bracket();
    test_linsp_to_vector();
    test_linsp_to_string();
    test_linsp_equality();
    test_linsp_inequality();
    test_linsp_intersect();
    test_linsp_negative_step();
    test_linsp_exception();
    test_linsp_contains();
    test_linsp_nearest();
    test_linsp_subset();
    test_linsp_iteration();

    std::cout << "\n=== mapper tests ===\n";
    test_find_index();
    test_map_index();
    test_map_value();

    std::cout << "\nAll tests passed!\n";
    return 0;
}
