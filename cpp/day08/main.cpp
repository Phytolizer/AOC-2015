#include <advent.hpp>
#include <array>
#include <ctre.hpp>
#include <fstream>
#include <iostream>
#include <variant>
#include <vector>

constexpr ctll::fixed_string ENCODED_CHAR_PATTERN =
    R"re([^\\]|\\(?:x([0-9a-fA-F]{2})|([\\"])))re";

using DecodedChar = std::variant<char, std::uint8_t, std::array<char, 2>>;

int main() {
    std::ifstream input{std::string{advent::kInput}};
    if (!input) {
        throw std::runtime_error{"Failed getting puzzle input"};
    }
    std::string line;

    std::size_t originalLength = 0;
    std::size_t decodedLength = 0;
    ADVENT_FOREACH_LINE(input, line) {
        if (line.empty()) {
            continue;
        }

        originalLength += line.length();
        auto [matched, inner] = ctre::match<R"re("(.*)")re">(line);
        if (!matched) {
            continue;
        }
        line = inner.to_string();

        std::vector<DecodedChar> decoded;

        for (auto match : ctre::range<ENCODED_CHAR_PATTERN>(line)) {
            if (match.get<1>()) {
                decoded.emplace_back(static_cast<std::uint8_t>(
                    std::stoi(match.get<1>().to_string(), nullptr, 16)));
            } else if (match.get<2>()) {
                decoded.emplace_back(
                    std::array<char, 2>{'\\', match.get<2>().to_view()[0]});
            } else {
                decoded.emplace_back(match.get<0>().to_view()[0]);
            }
        }
        decodedLength += decoded.size();

        std::string origLine = line;
    }

    std::cout << "Original length: " << originalLength << '\n';
    std::cout << "Decoded length: " << decodedLength << '\n';
    std::cout << "Savings: " << originalLength - decodedLength << '\n';
}