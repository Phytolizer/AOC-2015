#include <advent.hpp>
#include <fstream>
#include <iostream>

bool isNice(std::string_view str)
{
    bool hasDouble = false;
    bool hasBadPair = false;
    int nvowels = 0;

    char pc = ' ';
    for (char c : str)
    {
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
        {
            ++nvowels;
        }
        if (pc == 'a' && c == 'b' || pc == 'c' && c == 'd' || pc == 'p' && c == 'q' || pc == 'x' && c == 'y')
        {
            hasBadPair = true;
            break;
        }
        if (pc == c)
        {
            hasDouble = true;
        }
        pc = c;
    }

    return nvowels >= 3 && hasDouble && !hasBadPair;
}

bool hasPair(std::string_view pool, std::string_view pair)
{
    char pc = ' ';
    for (char c : pool)
    {
        if (pc == pair[0] && c == pair[1])
        {
            return true;
        }
        pc = c;
    }
    return false;
}

bool isNice2(std::string_view str)
{
    bool hasTwoPair = false;
    for (int pairStartIdx = 0; pairStartIdx < str.size() - 1; ++pairStartIdx)
    {
        std::string_view pair{str.begin() + pairStartIdx, 2};
        if (hasPair(std::string_view{str.begin(), str.begin() + pairStartIdx}, pair) ||
            hasPair(std::string_view{str.begin() + pairStartIdx + 2, str.end()}, pair))
        {
            hasTwoPair = true;
            break;
        }
    }
    bool hasSeparatedRepetition = false;
    for (int repStartIdx = 0; repStartIdx < str.size() - 2; ++repStartIdx)
    {
        if (str[repStartIdx] == str[repStartIdx + 2])
        {
            hasSeparatedRepetition = true;
            break;
        }
    }
    return hasTwoPair && hasSeparatedRepetition;
}

int main()
{
    std::ifstream input{"day05/input.txt"};
    std::string line;

    int nnice = 0;
    int nnice2 = 0;

    ADVENT_FOREACH_LINE(input, line)
    {
        if (isNice(line))
        {
            ++nnice;
        }
        if (isNice2(line))
        {
            ++nnice2;
        }
    }

    std::cout << "Part 1: " << nnice << '\n';
    std::cout << "Part 2: " << nnice2 << '\n';
}
