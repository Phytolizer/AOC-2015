#include <advent.hpp>
#include <fstream>
#include <iostream>

int main()
{
    char c;
    int floor = 0;
    size_t index = 0;
    bool indexFound = false;
    std::ifstream inf{"day01/input.txt"};
    if (!inf.is_open())
    {
        throw std::runtime_error{"Failed to open input.txt"};
    }
    ADVENT_FOREACH_CHAR(inf, c)
    {
        switch (c)
        {
        case '(':
            ++floor;
            break;
        case ')':
            --floor;
            if (!indexFound && floor < 0)
            {
                std::cout << "Part 2: " << index << "\n";
                indexFound = true;
            }
            break;
        default:
            break;
        }
        ++index;
    }
    std::cout << "Part 1: " << floor << "\n";
    return 0;
}
