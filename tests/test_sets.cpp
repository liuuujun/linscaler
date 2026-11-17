#include "test_support.hpp"
#include <linscaler/linscaler.hpp>
#include <algorithm>
#include <iterator>
#include <set>
#include <vector>
using namespace linscaler::linsp;
template<typename T>
std::set<T> values(const linsp_<T>& sequence) { return {sequence.begin(), sequence.end()}; }
bool is_progression(const std::set<int>& values) {
    if (values.size() < 3) return true;
    auto it = values.begin();
    int previous = *it++;
    const int step = *it - previous;
    for (; it != values.end(); ++it) {
        if (*it - previous != step) return false;
        previous = *it;
    }
    return true;
}
int main() {
    CHECK(intersect(linsp_<int>(1, 2, 6), linsp_<int>(2, 3, 5)).to_vector() == std::vector<int>({5, 11}));
    CHECK(!try_unite(linsp_<int>(0, 2, 2), linsp_<int>(8, 2, 2)));
    CHECK_THROWS(std::domain_error, unite(linsp_<int>(0, 2, 2), linsp_<int>(8, 2, 2)));
    CHECK(unite(linsp_<int>(0, 2, 3), linsp_<int>(1, 2, 3)).to_vector() == std::vector<int>({0,1,2,3,4,5}));
    CHECK(unite(linsp_<int>(4, 0, 8), linsp_<int>(8, 0, 3)).to_vector() == std::vector<int>({4,8}));
    CHECK(try_unite(linsp_<int>{}, linsp_<int>{}).has_value());
    CHECK(try_unite(linsp_<int>{}, linsp_<int>{})->empty());
    std::vector<linsp_<int>> sequences(1);
    for (int first = -5; first <= 5; ++first)
        for (int step = -4; step <= 4; ++step)
            for (std::size_t count = 1; count <= 6; ++count)
                sequences.emplace_back(first, step, count);
    std::vector<std::set<int>> sets;
    for (const auto& sequence : sequences) sets.push_back(values(sequence));
    std::size_t comparisons = 0;
    for (std::size_t i = 0; i < sequences.size(); ++i)
        for (std::size_t j = 0; j < sequences.size(); ++j) {
            const auto& a = sets[i];
            const auto& b = sets[j];
            std::set<int> expected_intersection, expected_union;
            std::set_intersection(a.begin(), a.end(), b.begin(), b.end(),
                                  std::inserter(expected_intersection, expected_intersection.end()));
            std::set_union(a.begin(), a.end(), b.begin(), b.end(),
                           std::inserter(expected_union, expected_union.end()));
            const auto actual_intersection = intersect(sequences[i], sequences[j]);
            CHECK(values(actual_intersection) == expected_intersection);
            const auto actual_union = try_unite(sequences[i], sequences[j]);
            CHECK(actual_union.has_value() == is_progression(expected_union));
            if (actual_union) CHECK(values(*actual_union) == expected_union);
            ++comparisons;
        }
    std::cout << comparisons << " progression pairs checked against std::set\n";
    // Dyadic coordinates give an independent exact oracle for floating operations.
    for (int first = -3; first <= 3; ++first)
        for (int step = -3; step <= 3; ++step)
            for (int other_first = -3; other_first <= 3; ++other_first)
                for (int other_step = -3; other_step <= 3; ++other_step) {
                    const linsp_<int> ai(first, step, 4), bi(other_first, other_step, 5);
                    const linsp_<double> af(first / 4.0, step / 4.0, 4);
                    const linsp_<double> bf(other_first / 4.0, other_step / 4.0, 5);
                    std::set<double> expected;
                    for (auto value : intersect(ai, bi)) expected.insert(value / 4.0);
                    CHECK(values(intersect(af, bf)) == expected);
                    const auto iu = try_unite(ai, bi);
                    const auto fu = try_unite(af, bf);
                    CHECK(iu.has_value() == fu.has_value());
                    if (iu) {
                        expected.clear();
                        for (auto value : *iu) expected.insert(value / 4.0);
                        CHECK(values(*fu) == expected);
                    }
                }
    CHECK(intersect(linsp_<double>(0.0, 0.5, 5), linsp_<double>(1.0, 0.5, 5)).count() == 3);
    CHECK(intersect(linsp_<double>(2.0, -0.5, 5), linsp_<double>(0.0, 1.0, 3)).last() == 2.0);
    const auto constant = intersect(linsp_<double>(1.0, 0.0, 4), linsp_<double>(0.0, 0.5, 5));
    CHECK(constant.count() == 1 && constant.first() == 1.0);
    const linsp_<double> rounded(1e20, 1.0, 10);
    CHECK(intersect(rounded, rounded).count() == 1);
    CHECK(unite(rounded, rounded).count() == 1);
    CHECK(unite(rounded, linsp_<double>{}).count() == 1);
}

