#pragma once

#include <string>
#include <string_view>

#if __has_include(<config.hpp>)
#include <config.hpp>
namespace advent {
constexpr std::string_view kInput =
    CMAKE_CURRENT_SOURCE_DIR "/" CMAKE_TARGET "/input.txt";
} // namespace advent
#endif

#define ADVENT_FOREACH_CHAR(File, CharVar) while ((File) >> (CharVar))
#define ADVENT_FOREACH_LINE(File, LineVar)                                     \
    while (std::getline((File), (LineVar)))

namespace advent {
std::string readEntireFile(const std::string& fileName);

template <typename T, typename... Rest>
void hashCombine(size_t& seed, const T& v, Rest... rest) {
    seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    (hashCombine(seed, rest), ...);
}
} // namespace advent
