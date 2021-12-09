#include <fstream>
#include <iostream>

int main()
{
    char c;
    int floor = 0;
    std::ifstream inf{"day01/input.txt"};
    if (!inf.is_open())
    {
        throw std::runtime_error{"Failed to open input.txt"};
    }
    while (inf >> c)
    {
        switch (c)
        {
        case '(':
            ++floor;
            break;
        case ')':
            --floor;
            break;
        default:
            break;
        }
    }
    std::cout << floor << "\n";
    return 0;
}
