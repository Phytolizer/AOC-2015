#include <advent.hpp>
#include <ctre.hpp>
#include <fstream>

constexpr ctll::fixed_string HEX_RE = R"(\\x[0-9a-f]{2})";
constexpr ctll::fixed_string QUOTE_RE = R"(\\\")";
constexpr ctll::fixed_string SLASH_RE = R"(\\\\)";

int main()
{
    std::ifstream input{"day08/input.txt"};
    std::string line;
    ADVENT_FOREACH_LINE(input, line)
    {
        if (line.empty())
        {
            continue;
        }

        std::string origLine = line;
    }
}