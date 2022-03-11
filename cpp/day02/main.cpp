// ReSharper disable CppUseRangeAlgorithm
#include <fmt/format.h>

#include <advent.hpp>
#include <array>
#include <ctre.hpp>
#include <fstream>
#include <iostream>
#include <regex>

int main() {
    std::ifstream file{"day02/input.txt"};
    if (!file.is_open()) {
        throw std::runtime_error{"Could not open input.txt!"};
    }

    long result = 0;
    long result2 = 0;
    std::string line;
    ADVENT_FOREACH_LINE(file, line) {
        ctre::regex_results match = ctre::match<R"((\d+)x(\d+)x(\d+))">(line);
        auto sx = std::string{match.get<1>().to_view()};
        long x = std::strtol(sx.c_str(), nullptr, 10);
        auto sy = std::string{match.get<2>().to_view()};
        long y = std::strtol(sy.c_str(), nullptr, 10);
        auto sz = std::string{match.get<3>().to_view()};
        long z = std::strtol(sz.c_str(), nullptr, 10);
        std::array<long, 3> sides{
            x * y,
            y * z,
            z * x,
        };
        const long smallestSide = *std::min_element(sides.begin(), sides.end());
        const long surfaceArea =
            2 * (sides[0] + sides[1] + sides[2]) + smallestSide;
        result += surfaceArea;

        std::array<long, 3> edges{x, y, z};
        std::vector<long> ribbonLengths;
        for (int i = 0; i < edges.size(); ++i) {
            for (int j = 0; j < edges.size(); ++j) {
                if (i == j) {
                    continue;
                }
                ribbonLengths.push_back(2 * edges[i] + 2 * edges[j]);
            }
        }
        result2 +=
            *std::min_element(ribbonLengths.begin(), ribbonLengths.end()) +
            x * y * z;
    }
    fmt::print("Part 1: {}\n", result);
    fmt::print("Part 2: {}\n", result2);
}
