#include <advent.hpp>
#include <iostream>
#include <unordered_set>

struct Coordinates
{
    int x;
    int y;

    bool operator==(const Coordinates& c) const
    {
        return x == c.x && y == c.y;
    }
};

struct CoordinateHasher
{
    size_t operator()(const Coordinates& c) const
    {
        constexpr std::hash<int> h{};
        size_t seed = h(c.x);
        advent::hashCombine(seed, c.y);
        return seed;
    }
};

enum class Mover
{
    Santa,
    RoboSanta,
};

int main()
{
    Coordinates part1Coords{0, 0};
    Coordinates santa{0, 0};
    Coordinates roboSanta{0, 0};
    auto mover = Mover::Santa;
    std::unordered_set<Coordinates, CoordinateHasher> visited;
    visited.emplace(part1Coords);
    size_t nvisited = 1;
    std::unordered_set<Coordinates, CoordinateHasher> visited2;
    visited2.emplace(santa);
    size_t nvisited2 = 1;
    for (const std::string text = advent::readEntireFile("day03/input.txt"); char c : text)
    {
        Coordinates* mx = nullptr;
        switch (mover)
        {
        case Mover::Santa:
            mover = Mover::RoboSanta;
            mx = &santa;
            break;
        case Mover::RoboSanta:
            mover = Mover::Santa;
            mx = &roboSanta;
            break;
        }
        switch (c)
        {
        case '^':
            mx->x += 1;
            part1Coords.x += 1;
            break;
        case 'v':
            mx->x -= 1;
            part1Coords.x -= 1;
            break;
        case '<':
            mx->y -= 1;
            part1Coords.y -= 1;
            break;
        case '>':
            mx->y += 1;
            part1Coords.y += 1;
            break;
        default:
            break;
        }
        if (!visited.contains(part1Coords))
        {
            visited.emplace(part1Coords);
            nvisited += 1;
        }
        if (!visited2.contains(*mx))
        {
            visited2.emplace(*mx);
            nvisited2 += 1;
        }
    }
    std::cout << "Part 1: " << nvisited << "\n";
    std::cout << "Part 2: " << nvisited2 << "\n";
    return 0;
}
